#include "draughts.h"
#include "ui_draughts.h"
#include "draughtsbase.h"
#include "chessbutton.h"

#include <QPainter>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QColor>


Draughts::Draughts(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Draughts)
{
    ui->setupUi(this);
    this->resize(675,600);

    ui->renshu->move(550,100);
    ui->qiuhe->move(550,150);
    connect(ui->renshu,SIGNAL(clicked(bool)),this,SLOT(renshuAct()));
    connect(ui->qiuhe,SIGNAL(clicked(bool)),this,SLOT(qiuheAct()));

    ui->stepboard->move(550,200);
    ui->stepboard->resize(75,40);

    this->setStyleSheet("background-color: rgb(238,232,170);");

    logicBase=new DraughtsBase(this);

    black=new ChessButton[20];
    white=new ChessButton[20];
    for(int i=0;i<20;i++)
    {
        black[i].setParent(this);
        white[i].setParent(this);

        black[i].setFixedSize(30,30);
        white[i].setFixedSize(30,30);

        black[i].setText("B"+QString::number(i));
        white[i].setText("W"+QString::number(i));

        black[i].color=0;
        white[i].color=1;//黑为0，白为1；

        black[i].setStyleSheet("background-color: black;border-radius: 15px; checked {color: blue;};");
        white[i].setStyleSheet("background-color: white;border-radius: 15px; checked {color: green;};");

        black[i].num=i;
        white[i].num=i;

        connect(&black[i],SIGNAL(chessselected(int,int)),this,SLOT(selectedAction(int,int)));
        connect(&white[i],SIGNAL(chessselected(int,int)),this,SLOT(selectedAction(int,int)));

        this->place(i);
        black[i].setEnabled(false);
        white[i].setEnabled(false);
    }

    //connect(logicBase,SIGNAL(chessmove(int)),this,SLOT(place(int)));
    connect(logicBase,SIGNAL(chessmove(int)),this,SLOT(place(int)));
    connect(logicBase,SIGNAL(highlight(int)),this,SLOT(redraw(int)));
    connect(logicBase,SIGNAL(eatBlack()),this,SLOT(eatBlackChess()));
    connect(logicBase,SIGNAL(eatWhite()),this,SLOT(eatWhiteChess()));
    connect(logicBase,SIGNAL(chat(int,int,int,int)),this,SLOT(sendInfo(int,int,int,int)));
    connect(logicBase,SIGNAL(gameresult(int)),this,SLOT(gamefinished(int)));

    curnum=-1;
    qiuheSwitch=0;
    step=0;

    gamestate=-1;// -1表示游戏未开始,0表示自定义，1表示开始
    blackcount=0;
    whitecount=0;


    //this->logicBase->prepare(1);
    //black[18].setFocus();  //这样就可以直接触发这个空间的focusinevent 完美，用socket将棋子颜色，编号，和目的地传过来，在这边
    //set一下focus，直接moveTodst，完事了
    QString ip=getIP();
    ui->IPlabel->resize(155,12);
    ui->IPlabel->setText("LocalHost IP: "+ip);
    ui->IPlabel->move(100,50);
    ui->port->resize(50,12);
    ui->port->move(280,50);
    ui->lineEdit->move(320,50);
    ui->lineEdit->resize(40,12);
    connect(ui->lineEdit,SIGNAL(editingFinished()),this,SLOT(initServer()));
    ui->portselect->move(360,50);
    ui->portselect->resize(30,12);
    ui->portselect->setText("ok");

    ui->clearall->move(550,300);
    connect(ui->clearall,SIGNAL(clicked(bool)),this,SLOT(clearallAndDefine()));

    ui->gamestart->move(550,350);
    connect(ui->gamestart,SIGNAL(clicked(bool)),this,SLOT(gamestart()));

}

Draughts::~Draughts()
{
    delete ui;
    delete logicBase;
    delete black;
    delete white;
}

void Draughts::initServer()
{
    int port=ui->lineEdit->text().toInt();
    qDebug()<<"port: "<<port;
    if(port!=0)
    {
    this->listenSocket =new QTcpServer();
    this->listenSocket->listen(QHostAddress::Any,port);
    connect(this->listenSocket,SIGNAL(newConnection()),this,SLOT(acceptConnection()));
    ui->lineEdit->setEnabled(false);
    }
}

void Draughts::acceptConnection()
{
    qDebug()<<"new connection";
    this->readWriteSocket=this->listenSocket->nextPendingConnection();
    connect(this->readWriteSocket,SIGNAL(readyRead()),this,SLOT(recvMessage()));
}

void Draughts::recvMessage()
{
    QString info="";
    info+=this->readWriteSocket->readAll();
    if(info=="0")
    {
        //求和
        //QMessageBox::
        QMessageBox::StandardButton reply;
        reply=QMessageBox::question(this,"提示","对方请求和棋，您是否同意？（若选择拒绝，您需要在40步之内战胜他）",
                                    QMessageBox::Yes|QMessageBox::No);

        if(reply==QMessageBox::Yes)
        {
            QString info1="yes";
            QByteArray* array=new QByteArray();
            array->clear();
            array->append(info1);
            this->readWriteSocket->write(array->data());

            QMessageBox::information(this,"提示","和棋！");

        }
        else
        {
            QString info2="no";
            QByteArray* array=new QByteArray();
            array->clear();
            array->append(info2);
            this->readWriteSocket->write(array->data());

            qiuheSwitch=1;

            QMessageBox::information(this,"提示","您拒绝了和棋。现在开始记步.");
        }



    }
    else if(info=="1")
    {
        //认输
        QMessageBox::information(this,"提示","对方已认输，恭喜您获胜啦！");

    }
    else if(info=="yes")
    {
        QMessageBox::information(this,"提示","对方同意了和棋。");
    }
    else if(info=="no")
    {
        qiuheSwitch=1;
        QMessageBox::information(this,"提示","对方拒绝和棋，现在开始计算步数。");

    }
    else
    {
        int color=0;
        QStringList strList=info.split(",");
        if(strList.at(0)=="0")
        {
            qDebug()<<"white located";
            color=0;
            int num=strList.at(1).toInt();
            //white[19-num].setFocus();
            selectedAction(1,19-num);
        }
        else if(strList.at(0)=="1")
        {
            qDebug()<<"black located";
            color=1;
            int num=strList.at(1).toInt();
            //black[19-num].setFocus();
            selectedAction(0,19-num);
        }
        else
        {
            qDebug()<<"no focus";
        }


        int x=strList.at(2).toInt();
        int y=strList.at(3).toInt();

        this->moveToDst(9-x,9-y);


        //this->logicBase->prepare(color);
    }

}
void Draughts::sendInfo(int color, int num, int xdst, int ydst)
{
    QString info="";
    info+=QString::number(color);
    info+=',';
    info+=QString::number(num);
    info+=',';
    info+=QString::number(xdst);
    info+=',';
    info+=QString::number(ydst);
    qDebug()<<info;
    QByteArray* array=new QByteArray();
    array->clear();
    array->append(info);
    qDebug()<<info;
    this->readWriteSocket->write(array->data());
}


void Draughts::place(int i)//将base中各个棋子的位置同步到Gui
{
    if(qiuheSwitch==1)
    {
        step++;
        ui->stepboard->display(step);
    }
    black[i].move(100+(this->logicBase->black[i].ypos)*40,100+(this->logicBase->black[i].xpos)*40);
    white[i].move(100+(this->logicBase->white[i].ypos)*40,100+(this->logicBase->white[i].xpos)*40);
    if(this->logicBase->black[i].isKing==true)
    {
        black[i].setText("King"+QString::number(i));
    }
    if(this->logicBase->white[i].isKing==true)
    {
        white[i].setText("King"+QString::number(i));
    }

    if(step==40)
    {
        QMessageBox::information(this,"提示","40步已到");
    }

}

void Draughts::selectedAction(int color, int num)
{
    //让logicbase给这个按钮寻路。
    /*
    this->logicBase->roads.clear();
    this->logicBase->allroads.clear();

    this->logicBase->xvector.clear();
    this->logicBase->yvector.clear();
    this->logicBase->numvec.clear();
*/
    QSound::play("D:\\8378.wav");
    this->logicBase->findWays(color,num);
}

void Draughts::mousePressEvent(QMouseEvent *event)
{
    if(gamestate==1)
    {
        int x=event->x();
        int y=event->y();
        if(x<100||x>500)return ;
        if(y<100||y>500)return ;

        int row=(y-100)/40;
        int col=(x-100)/40;

        //将目的地坐标传给logicbase
        moveToDst(row,col);
    }

    else if(gamestate==0)
    {
        //自定义棋盘
        int x=event->x();
        int y=event->y();
        if(x<100||x>500)return ;
        if(y<100||y>500)return ;

        int row=(y-100)/40;
        int col=(x-100)/40;

        if(event->button()==Qt::LeftButton&&whitecount<20)
        {
            this->logicBase->white[whitecount].isAlive=true;
            this->logicBase->white[whitecount].xpos=row;
            this->logicBase->white[whitecount].ypos=col;
            this->logicBase->white[whitecount].isKing=false;

            this->logicBase->a[row][col][0]=1;
            this->logicBase->a[row][col][1]=whitecount;

            this->white[whitecount].move(100+40*col,100+row*40);
                    //white[i].move(100+(this->logicBase->white[i].ypos)*40,100+(this->logicBase->white[i].xpos)*40);
            this->white[whitecount].show();
            whitecount++;
        }
        else if(event->button()==Qt::RightButton&&blackcount<20)
        {
            this->logicBase->black[blackcount].isAlive=true;
            this->logicBase->black[blackcount].xpos=row;
            this->logicBase->black[blackcount].ypos=col;
            this->logicBase->black[blackcount].isKing=false;

            this->logicBase->a[row][col][0]=0;
            this->logicBase->a[row][col][1]=blackcount;

            this->black[blackcount].move(100+40*col,100+40*row);
            this->black[blackcount].show();
            blackcount++;

        }

    }

    else
    {
        qDebug()<<"游戏未开始，未处在自定义模式";
    }

}

void Draughts::moveToDst(int row, int col)
{
    this->logicBase->setDst(row,col);
}

void Draughts::redraw(int num)
{
    curnum=num;
    qDebug()<<"当前棋子: "<<num;
    this->update();
}

void Draughts::paintEvent(QPaintEvent *event)//complete
{
    QPainter p(this);

    int size=this->logicBase->xvector.size();

    QPen pen(Qt::black);
    pen.setWidth(3);
    pen.setColor(QColor(255,193,37));

    p.setPen(pen);
    p.translate(94,94);
    p.drawLine(0,0,404,0);
    p.drawLine(0,0,0,404);
    p.drawLine(404,0,404,404);
    p.drawLine(0,404,404,404);//画线

    p.translate(2,2);

    p.setPen(Qt::NoPen);
    p.setBrush(QColor(255,193,37));
    p.save();

    for(int i=0;i<5;i++)
    {
        for(int j=0;j<5;j++)
        {
            p.drawRect(80*j+40,0,40,40);
        }
        p.translate(0,40);
        for(int k=0;k<5;k++)
        {
            p.drawRect(80*k,0,40,40);
        }
        p.translate(0,40);
    }
    //涂色块

    p.restore();
    p.setBrush(Qt::green);
    qDebug()<<"Qpaintevent:: "<<size;
    for(int i=0;i<size;i++)
    {
        if(curnum==this->logicBase->locationowner[i])
        {
            int y=this->logicBase->xvector[i];
            int x=this->logicBase->yvector[i];
            qDebug()<<"can go: "<<y<<' '<<x;
            p.drawRect(40*x,40*y,40,40);

            if(this->logicBase->numvec[i]!=-1)
            {
                int num=this->logicBase->numvec[i];
                std::vector<RoadsNode> temp=this->logicBase->allroads[num];
                for(int k=0;k<temp.size();k++)
                {
                    int y=temp[k].xpos;
                    int x=temp[k].ypos;
                    p.drawEllipse(40*x,40*y,40,40);
                }
            }
        }
    }
    //高亮
}

void Draughts::eatBlackChess()
{
    for(int i=0;i<20;i++)
    {
        if(this->logicBase->black[i].isAlive==false)
        {
            black[i].hide();
            black[i].setEnabled(false);
            qDebug()<<"第 "<<i<<" 黑子被吃";
        }

    }
}

void Draughts::eatWhiteChess()
{
    for(int i=0;i<20;i++)
    {
        if(this->logicBase->white[i].isAlive==false)
        {
            white[i].hide();
            white[i].setEnabled(false);
            qDebug()<<"第 "<<i<<" 白子被吃";
        }
    }
}

void Draughts::renshuAct()
{
    QString info="1";
    QByteArray* array=new QByteArray();
    array->clear();
    array->append(info);

    this->readWriteSocket->write(array->data());

    QMessageBox::information(this,"提示","您认输了");
}
void Draughts::qiuheAct()
{
    QString info="0";
    QByteArray* array=new QByteArray();
    array->clear();
    array->append(info);

    this->readWriteSocket->write(array->data());

    QMessageBox::information(this,"提示","等待对方同意");

}

void Draughts::gamefinished(int color)
{
    if(color==0)
    {
        QMessageBox::information(this,"提示","黑子获胜");
    }
    else if(color==1)
    {
        QMessageBox::information(this,"提示","白子获胜");
    }
}

QString Draughts::getIP()
{
    QString localHostName = QHostInfo::localHostName();

    qDebug() <<"localHostName:"<<localHostName;
    QHostInfo info = QHostInfo::fromName(localHostName);
    foreach(QHostAddress address,info.addresses())
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            qDebug() <<"IPV4 Address: "<< address.toString();
            return address.toString();
        }
    }
    return "";
}

void Draughts::clearallAndDefine()
{

    for(int i=0;i<10;i++)
    {
        for(int j=0;j<10;j++)
        {
            this->logicBase->a[i][j][0]=-1;
            this->logicBase->a[i][j][1]=-1;
        }
    }

    for(int i=0;i<20;i++)
    {
        this->logicBase->black[i].isAlive=false;
        this->logicBase->black[i].isKing=false;
        this->logicBase->white[i].isAlive=false;
        this->logicBase->white[i].isKing=false;
    }

    this->eatBlackChess();
    this->eatWhiteChess();

    gamestate=0;
    //之后交给鼠标点击事件处理

}


void Draughts::gamestart()
{
    gamestate=1;

    for(int i=0;i<20;i++)
    {
        if(this->logicBase->black[i].isAlive==true)
        {
            black[i].show();
            black[i].setEnabled(true);
        }
        if(this->logicBase->white[i].isAlive==true)
        {
            white[i].show();
            white[i].setEnabled(true);
        }
    }
}

