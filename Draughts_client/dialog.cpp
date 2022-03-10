#include "dialog.h"
#include "ui_dialog.h"

#include "draughtsbase_client.h"
#include "chessbutton_client.h"
#include <QString>
#include <QPainter>
#include <QDebug>

#include <QStringList>
#include <QHostAddress>
#include <QMessageBox>

Draughts::Draughts(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
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

        black[i].setStyleSheet("background-color: white;border-radius: 15px; checked {color: blue;};");
        white[i].setStyleSheet("background-color: black;border-radius: 15px; checked {color: green;};");

        black[i].num=i;
        white[i].num=i;

        connect(&black[i],SIGNAL(chessselected(int,int)),this,SLOT(selectedAction(int,int)));
        connect(&white[i],SIGNAL(chessselected(int,int)),this,SLOT(selectedAction(int,int)));

        this->place(i);
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
    //black[18].setFocus();  //这样就可以直接触发这个空间的focusinevent 完美，用socket将棋子颜色，编号，和目的地传过来，在这边
    //set一下focus，直接moveTodst，完事了

    ui->connect->resize(50,12);
    ui->connect->move(100,50);
    ui->IPselectEdit->resize(100,12);
    ui->IPselectEdit->move(160,50);
    ui->port->resize(40,12);
    ui->port->move(280,50);
    ui->portSelectEdit->resize(40,12);
    ui->portSelectEdit->move(320,50);
    ui->ok->resize(40,12);
    ui->ok->move(360,50);
    ui->ok->setText("ok");

    connect(ui->IPselectEdit,SIGNAL(editingFinished()),this,SLOT(writeIP()));
    connect(ui->portSelectEdit,SIGNAL(editingFinished()),this,SLOT(writeport()));

    IP="";
    port=0;

}

Draughts::~Draughts()
{
    delete ui;
    delete logicBase;
    delete black;
    delete white;
}

void Draughts::writeIP()
{

    IP=ui->IPselectEdit->text();
    connectHost();
}

void Draughts::writeport()
{

    port=ui->portSelectEdit->text().toInt();
    connectHost();
}


void Draughts::connectHost()
{
    if(IP!=""&&port!=0)
    {
    this->readWriteSocket = new QTcpSocket();
    this->readWriteSocket->connectToHost(QHostAddress(IP),port);
    QObject::connect(this->readWriteSocket,SIGNAL(readyRead()),this,SLOT(recvMessage()));

    ui->IPselectEdit->setEnabled(false);
    ui->portSelectEdit->setEnabled(false);
    qDebug()<<"connected!";
    }
}


void Draughts::recvMessage()
{
    QString info="";
    info+=this->readWriteSocket->readAll();
    if(info=="0")
    {
        //求和
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

            QMessageBox::information(this,"提示","您拒绝了和棋。");
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
        QStringList strList=info.split(",");
        if(strList.at(0)=="0")
        {
            qDebug()<<"white located";
            int num=strList.at(1).toInt();
            //white[19-num].setFocus();
            selectedAction(1,19-num);
        }
        else if(strList.at(0)=="1")
        {
            qDebug()<<"black located";
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
    this->logicBase->findWays(color,num);
}

void Draughts::mousePressEvent(QMouseEvent *event)
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

void Draughts::moveToDst(int row, int col)
{
    this->logicBase->setDst(row,col);
}

void Draughts::redraw(int num)
{
    curnum=num;
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
    p.translate(98,98);
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
            black[i].hide();
    }
}

void Draughts::eatWhiteChess()
{
    for(int i=0;i<20;i++)
    {
        if(this->logicBase->white[i].isAlive==false)
            white[i].hide();
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

