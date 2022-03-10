#include "draughtsbase.h"
#include <QDebug>
#include <QSound>

int DraughtsBase::a[10][10][2]={{{-1,-1},{0,0},{-1,-1},{0,1},{-1,-1},{0,2},{-1,-1},{0,3},{-1,-1},{0,4}},
           {{0,5},{-1,-1},{0,6},{-1,-1},{0,7},{-1,-1},{0,8},{-1,-1},{0,9},{-1,-1}},
           {{-1,-1},{0,10},{-1,-1},{0,11},{-1,-1},{0,12},{-1,-1},{0,13},{-1,-1},{0,14}},
           {{0,15},{-1,-1},{0,16},{-1,-1},{0,17},{-1,-1},{0,18},{-1,-1},{0,19},{-1,-1}},
           {{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}},
           {{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}},
           {{-1,-1},{1,0},{-1,-1},{1,1},{-1,-1},{1,2},{-1,-1},{1,3},{-1,-1},{1,4}},
           {{1,5},{-1,-1},{1,6},{-1,-1},{1,7},{-1,-1},{1,8},{-1,-1},{1,9},{-1,-1}},
           {{-1,-1},{1,10},{-1,-1},{1,11},{-1,-1},{1,12},{-1,-1},{1,13},{-1,-1},{1,14}},
           {{1,15},{-1,-1},{1,16},{-1,-1},{1,17},{-1,-1},{1,18},{-1,-1},{1,19},{-1,-1}}};

DraughtsBase::DraughtsBase(QObject *parent):
    QObject(parent)
{
   /* a[10][10][2]={{{-1,-1},{0,0},{-1,-1},{0,1},{-1,-1},{0,2},{-1,-1},{0,3},{-1,-1},{0,4}},
               {{0,5},{-1,-1},{0,6},{-1,-1},{0,7},{-1,-1},{0,8},{-1,-1},{0,9},{-1,-1}},
               {{-1,-1},{0,10},{-1,-1},{0,11},{-1,-1},{0,12},{-1,-1},{0,13},{-1,-1},{0,14}},
               {{0,15},{-1,-1},{0,16},{-1,-1},{0,17},{-1,-1},{0,18},{-1,-1},{0,19},{-1,-1}},
               {{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}},
               {{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}},
               {{-1,-1},{1,0},{-1,-1},{1,1},{-1,-1},{1,2},{-1,-1},{1,3},{-1,-1},{1,4}},
               {{1,5},{-1,-1},{1,6},{-1,-1},{1,7},{-1,-1},{1,8},{-1,-1},{1,9},{-1,-1}},
               {{-1,-1},{1,10},{-1,-1},{1,11},{-1,-1},{1,12},{-1,-1},{1,13},{-1,-1},{1,14}},
               {{1,15},{-1,-1},{1,16},{-1,-1},{1,17},{-1,-1},{1,18},{-1,-1},{1,19},{-1,-1}}};
               *///这个记住，这样的初始化形式不能用来赋值，类里的数组如果想用这样初始化必须为static，如上
    memset(flag,0,sizeof(flag));

    dy[0]=-1;
    dy[1]=1;
    dy[2]=1;
    dy[3]=-1;

    dx[0]=-1;
    dx[1]=-1;
    dx[2]=1;
    dx[3]=1;

    //curchess=new Chess();
    curchess=NULL;

    dstr=-1;
    dstc=-1;
    curline=-1;
    colorlock=-1;
    winFlag=-1;
    black=new Chess[20];
    for(int i=0;i<20;i++)
    {
        black[i].color=0;
        black[i].num=i;
        black[i].isKing=false;
        black[i].isAlive=true;
        black[i].xpos=i/5;
        black[i].ypos=2*(i%5);
        if((i/5)%2==0)
        {
            black[i].ypos++;
        }
    }

    white=new Chess[20];
    for(int i=0;i<20;i++)
    {
        white[i].color=1;
        white[i].num=i;
        white[i].isKing=false;
        white[i].isAlive=true;
        white[i].xpos=i/5+6;
        white[i].ypos=2*(i%5);
        if((i/5)%2==0)
        {
            white[i].ypos++;
        }
    }

    //white[0].isKing=true;

}//初始棋子放置完成

void DraughtsBase::findWays(int color,int num)
{
    //给这枚棋子寻路。
    qDebug()<<"find ways for chess :"<<color<<' '<<num;

    //选定当前控制棋子
    if(color==0)
    {
        curchess=&black[num];
    }
    else
    {
        curchess=&white[num];
    }
    qDebug()<<"curchess: "<<curchess->xpos<<' '<<curchess->ypos;

    if(curchess->color!=colorlock)
    {
        colorlock=curchess->color;
        prepare(colorlock);
    }




    emit highlight(curchess->num);//

}

void DraughtsBase::setDst(int row, int col)
{
    this->dstr=row;
    this->dstc=col;
    //qDebug()<<"logicBase::setDst is complete";
    qDebug()<<this->dstr<<" "<<this->dstc;

    //如果合法就将当前操控棋子移到制定位置
    if(islegal())
    {
        qDebug()<<"is legal ";
        int i=curchess->xpos;//原位置
        int j=curchess->ypos;
        int num=curchess->num;
        int color=curchess->color;
        a[i][j][0]=-1;//颜色恢复为无
        a[i][j][1]=-1;//编号恢复为无

        qDebug()<<"原位置恢复："<<i<<' '<<j<<" color: "<<a[i][j][0]<<" num: "<<a[i][j][1];

        curchess->xpos=dstr;
        curchess->ypos=dstc;
        //位置完成改变

        //std::vector<RoadsNode> temp=allroads[curline];//确定路径   小心，如果么有吃子的话这个是空的，程序会崩掉

        //下面开始删除棋子
        if(curline>=0&&!allroads.empty())//如果这个非空，说明有棋子可以删去
        {
            qDebug()<<"开始删除棋子";
            std::vector<RoadsNode> temp=allroads[curline];
            if(color==0)//黑色
            {
                qDebug()<<"如果是黑色";
                int delx=(temp[0].xpos-i)/(abs(temp[0].xpos-i));
                int dely=(temp[0].ypos-j)/(abs(temp[0].ypos-j));

                int xx=i+delx;
                int yy=j+dely;

                while(xx!=temp[0].xpos)
                {
                    white[a[xx][yy][1]].isAlive=false;
                    a[xx][yy][0]=-1;
                    a[xx][yy][1]=-1;

                    xx+=delx;
                    yy+=dely;
                }
                for(int m=0;m<temp.size()-1;m++)
                {
                    delx=(temp[m+1].xpos-temp[m].xpos)/(abs(temp[m+1].xpos-temp[m].xpos));
                    dely=(temp[m+1].ypos-temp[m].ypos)/(abs(temp[m+1].ypos-temp[m].ypos));

                    xx=temp[m].xpos+delx;
                    yy=temp[m].ypos+dely;

                    while(xx!=temp[m+1].xpos)
                    {
                        white[a[xx][yy][1]].isAlive=false;
                        a[xx][yy][0]=-1;
                        a[xx][yy][1]=-1;

                        xx+=delx;
                        yy+=dely;
                    }
                }

                emit eatWhite();

            }
            else if(color==1)
            {
                int delx=(temp[0].xpos-i)/(abs(temp[0].xpos-i));
                int dely=(temp[0].ypos-j)/(abs(temp[0].ypos-j));

                int xx=i+delx;
                int yy=j+dely;

                while(xx!=temp[0].xpos)
                {
                    black[a[xx][yy][1]].isAlive=false;
                    a[xx][yy][0]=-1;
                    a[xx][yy][1]=-1;

                    xx+=delx;
                    yy+=dely;
                }
                for(int m=0;m<temp.size()-1;m++)
                {
                    delx=(temp[m+1].xpos-temp[m].xpos)/(abs(temp[m+1].xpos-temp[m].xpos));
                    dely=(temp[m+1].ypos-temp[m].ypos)/(abs(temp[m+1].ypos-temp[m].ypos));

                    xx=temp[m].xpos+delx;
                    yy=temp[m].ypos+dely;

                    while(xx!=temp[m+1].xpos)
                    {
                        black[a[xx][yy][1]].isAlive=false;
                        a[xx][yy][0]=-1;
                        a[xx][yy][1]=-1;

                        xx+=delx;
                        yy+=dely;
                    }
                }

                emit eatBlack();

            }

        }//吃子完毕

        if(curchess->color==0)
        {
            if(curchess->xpos==9)
            {
                curchess->isKing=true;
            }
        }
        else if(curchess->color==1)
        {
            if(curchess->xpos==0)
            {
                curchess->isKing=true;
            }
        }


        //这里加上是否为王的判定

        i=curchess->xpos;
        j=curchess->ypos;

        a[i][j][0]=color;
        a[i][j][1]=num;

        qDebug()<<"新位置： "<<i<<' '<<j<<" color: "<<a[i][j][0]<<" num: "<<a[i][j][1];
        locationowner.clear();
        xvector.clear();
        yvector.clear();
        numvec.clear();
        allroads.clear();

        //这里加上胜负判定

        gameover(color);




        emit chat(curchess->color,curchess->num,curchess->xpos,curchess->ypos);
        emit chessmove(curchess->num);
        emit highlight(curchess->num);

        //curchess=NULL;
        dstr=-1;
        dstc=-1;
        QSound::play("D:\\8407.wav");
    }
}

bool DraughtsBase::islegal()
{
    if(curchess==NULL)
    {
        qDebug()<<"curchess is NULL";
        return false;
    }
    for(int i=0;i<xvector.size();i++)
    {
        if(curchess->num==locationowner[i]&&dstr==xvector[i]&&dstc==yvector[i])
        {
            if(numvec[i]>=0)
            {
                curline=numvec[i];
            }
            return true;
        }
    }
    return false;
}


void DraughtsBase::dfs(int x, int y, int color,int num)//这个dfs只进行能够吃子的判定，先写兵的走法。complete
{
    int xx,yy,xxx,yyy;
    int i=0;
    int count=0;
    for(i=0;i<4;i++)
    {
        qDebug()<<"第 "<<i<<" 方向";

        xx=x+dx[i];//需要跳过的格子
        yy=y+dy[i];

        xxx=xx+dx[i];//需要到位的格子
        yyy=yy+dy[i];

        //越界就尝试下一个走法
        if(xxx<0||xxx>9)continue;
        if(yyy<0||yyy>9)continue;

        if(a[xxx][yyy][0]!=-1&&!(a[xxx][yyy][1]==num&&a[xxx][yyy][0]==color))continue;//wu chu luo jiao

        if(a[xx][yy][0]==-1)continue;//该方向无棋可吃

        if(a[xx][yy][0]==color)continue;//同色，尝试下一个

        //接下来就代表可以跳吃
        if(flag[xx][yy]!=1)
        {
            count++;
            flag[xx][yy]=1;//gai qizi yi bei tiaoguo
            //eatvec.push_back(a[xx][yy][1]);//记录吃子
            RoadsNode newNode;
            newNode.owner=num;
            newNode.xpos=xxx;
            newNode.ypos=yyy;
            roads.push_back(newNode);//记录所经位置
            qDebug()<<"new Node:"<<xxx<<' '<<yyy;
            dfs(xxx,yyy,color,num);//由此继续深探


            flag[xx][yy]=0;
            //eatvec.pop_back();
            roads.pop_back();
        }

    }
    qDebug()<<"i: "<<i<<" count: "<<count;
    if(i==4&&count==0)
    {
        qDebug()<<"线路查找完成";
        allroads.push_back(roads);

    }
}

void DraughtsBase::dfsKing(int x, int y, int color, int num)
{
    int xx,yy;
    int i=0;
    int count=0;
    for(i=0;i<4;i++)
    {
        qDebug()<<"第 "<<i<<" 方向";
        xx=x+dx[i];
        yy=y+dy[i];
        while(checkloc(xx,yy))
        {
            if(a[xx][yy][0]==color&&a[xx][yy][1]!=num)
            {
                qDebug()<<"same color ";
                break;
            }
            if(a[xx][yy][0]!=-1&&a[xx][yy][0]!=color)//有的吃
            {
                if(flag[xx][yy]==0)//可以吃
                {
                    int xxx,yyy;
                    xxx=xx+dx[i];
                    yyy=yy+dy[i];
                    qDebug()<<"wang to eat: "<<xx<<' '<<yy;
                    qDebug()<<"check: "<<xxx<<' '<<yyy;
                    if(a[xxx][yyy][0]!=-1&&!(a[xxx][yyy][0]==color&&a[xxx][yyy][1]==num)){break;}
                    //有子就退出循环
                    while(checkloc(xxx,yyy))
                    {
                        if(a[xxx][yyy][0]!=-1&&!(a[xxx][yyy][0]==color&&a[xxx][yyy][1]==num)){break;}
                        //有子就退出循环
                        qDebug()<<"eat "<<xx<<' '<<yy;
                        count++;
                        flag[xx][yy]=1;
                        RoadsNode newnode;
                        newnode.owner=num;
                        newnode.xpos=xxx;
                        newnode.ypos=yyy;
                        roads.push_back(newnode);
                        qDebug()<<"new node :"<<newnode.xpos<<' '<<newnode.ypos;
                        dfsKing(xxx,yyy,color,num);

                        flag[xx][yy]=0;
                        roads.pop_back();

                        xxx+=dx[i];
                        yyy+=dy[i];
                    }
                    if(a[xxx][yyy][0]!=-1&&!(a[xxx][yyy][0]==color&&a[xxx][yyy][1]==num)){break;}
                    //有子就退出循环
                }
                else if(flag[xx][yy]==1)
                {
                    break;
                }
            }
           xx+=dx[i];
           yy+=dy[i];
        }

    }
    if(i==4&&count==0)
    {
        qDebug()<<"new roads pushed back!";
        allroads.push_back(roads);
    }

}



bool DraughtsBase::caneat(Chess * chess) const//complete
{
    int x=chess->xpos;
    int y=chess->ypos;

    int xxx,yyy,xx,yy;
        for(int i=0;i<4;i++)
        {
            xx=x+dx[i];
            xxx=xx+dx[i];
            yy=y+dy[i];
            yyy=yy+dy[i];
            //越界就过
            if(xxx<0||xxx>9)continue;
            if(yyy<0||yyy>9)continue;

            if(a[xxx][yyy][0]!=-1)continue;//落位有子就过
            if(a[xx][yy][0]==-1)continue;//无子可吃就过

            if(a[xx][yy][0]!=chess->color)return true;

        }


    return false;

}

bool DraughtsBase::kingcaneat(Chess * chess) const
{
    int x=chess->xpos;
    int y=chess->ypos;
    int color=chess->color;

    int xx,yy;
    for(int i=0;i<4;i++)
    {
        xx=x+dx[i];
        yy=y+dy[i];

        while(checkloc(xx,yy))
        {
            if(a[xx][yy][0]!=-1&&a[xx][yy][0]!=color)
            {
                int xxx=xx+dx[i];
                int yyy=yy+dy[i];

                if(checkloc(xxx,yyy)&&a[xxx][yyy][0]==-1)
                {
                    qDebug()<<"能吃"<<xxx<<' '<<yyy<<"颜色 "<<a[xxx][yyy][0];
                    return true;
                }
                else
                {
                    break;
                }
            }
            xx+=dx[i];
            yy+=dy[i];
        }
    }

    return false;

}

bool DraughtsBase::checkloc(int x,int y) const
{
    if(x<0||x>9)return false;
    if(y<0||y>9)return false;

    return true;
}

void DraughtsBase::prepare(int color)
{
    xvector.clear();
    yvector.clear();
    locationowner.clear();
    allroads.clear();
    numvec.clear();

    if(color==0)//给黑子寻路
    {
        qDebug()<<"给黑子寻路";
        for(int i=0;i<20;i++)
        {
            if(black[i].isAlive==false)continue;
            if(!black[i].isKing)
            {
                if(!caneat(&black[i]))
                {
                    if((black[i].xpos+1)<=9&&(black[i].ypos-1)>=0)
                    {
                        if(a[black[i].xpos+1][black[i].ypos-1][0]==-1)
                        {
                            locationowner.push_back(i);
                            xvector.push_back(black[i].xpos+1);
                            yvector.push_back(black[i].ypos-1);
                            numvec.push_back(-1);
                        }
                    }
                    if((black[i].xpos+1)<=9&&(black[i].ypos+1)<=9)
                    {
                        if(a[black[i].xpos+1][black[i].ypos+1][0]==-1)
                        {
                            locationowner.push_back(i);
                            xvector.push_back(black[i].xpos+1);
                            yvector.push_back(black[i].ypos+1);
                            numvec.push_back(-1);
                        }
                    }
                }//不能吃
                else
                {
                    qDebug()<<"can eat";
                   dfs(black[i].xpos,black[i].ypos,black[i].color,black[i].num);
                }

            }

            else if(black[i].isKing)
            {
                if(!kingcaneat(&black[i]))
                {
                    int x=black[i].xpos;
                    int y=black[i].ypos;
                    int xx,yy;
                    for(int k=0;k<4;k++)
                    {
                        xx=x+dx[k];
                        yy=y+dy[k];

                        while(checkloc(xx,yy))
                        {

                            if(a[xx][yy][0]!=-1)break;

                            locationowner.push_back(i);
                            xvector.push_back(xx);
                            yvector.push_back(yy);
                            numvec.push_back(-1);

                            xx+=dx[k];
                            yy+=dy[k];
                        }

                    }
                }//不能吃子处理完毕
                else
                {
                    qDebug()<<"king can eat";
                    dfsKing(black[i].xpos,black[i].ypos,black[i].color,black[i].num);
                }

            }//是王
        }
    }//黑子寻路

    else if(color==1)
    {
        qDebug()<<"给白子寻路";
        for(int i=0;i<20;i++)
        {
            if(white[i].isAlive==false)continue;
            if(!white[i].isKing)
            {
                qDebug()<<"白 "<<i<<" 寻路中";
                if(!caneat(&white[i]))
                {
                    qDebug()<<"白 "<<i<<" 不能吃";
                    if((white[i].xpos-1)>=0&&(white[i].ypos-1)>=0)
                    {
                        if(a[white[i].xpos-1][white[i].ypos-1][0]==-1)
                        {
                            locationowner.push_back(i);
                            qDebug()<<"locationowner top:"<<locationowner[locationowner.size()-1];
                            xvector.push_back(white[i].xpos-1);
                            yvector.push_back(white[i].ypos-1);
                            numvec.push_back(-1);
                            qDebug()<<"白 "<<i<<" 可以去"<<white[i].xpos-1<<' '<<white[i].ypos-1;
                        }
                    }
                    if((white[i].xpos-1)>=0&&(white[i].ypos+1)<=9)
                    {
                        if(a[white[i].xpos-1][white[i].ypos+1][0]==-1)
                        {
                            locationowner.push_back(i);
                            xvector.push_back(white[i].xpos-1);
                            yvector.push_back(white[i].ypos+1);
                            numvec.push_back(-1);
                            qDebug()<<"白 "<<i<<" 可以去"<<white[i].xpos-1<<' '<<white[i].ypos+1;
                        }
                    }
                }//不能吃
                else
                {
                    qDebug()<<"can eat";
                   dfs(white[i].xpos,white[i].ypos,white[i].color,white[i].num);
                }

            }

            else if(white[i].isKing)
            {
                qDebug()<<"白 "<<i<<" 是王";
                if(!kingcaneat(&white[i]))
                {
                    qDebug()<<"王不能吃";

                    int x=white[i].xpos;
                    int y=white[i].ypos;
                    int xx,yy;
                    for(int j=0;j<4;j++)
                    {
                        xx=x+dx[j];
                        yy=y+dy[j];

                        while(checkloc(xx,yy))
                        {

                            if(a[xx][yy][0]!=-1)break;

                            locationowner.push_back(i);
                            //qDebug()<<"locationowner top:"<<locationowner[locationowner.size()-1];
                            xvector.push_back(xx);
                            yvector.push_back(yy);
                            numvec.push_back(-1);
                            qDebug()<<"王可以去： "<<xx<<' '<<yy;
                            xx+=dx[j];
                            yy+=dy[j];
                        }

                    }
                }//不能吃子处理完毕
                else
                {
                    qDebug()<<"king can eat";
                    dfsKing(white[i].xpos,white[i].ypos,white[i].color,white[i].num);
                    qDebug()<<"dfsKing done";
                }

            }//是王
        }
    }//白子
    //allroads构建完成


    int totalroadcount=allroads.size();
    qDebug()<<"道路总数: "<<totalroadcount;
    int maxsize=0;
    for(int e=0;e<totalroadcount;e++)
    {
        qDebug()<<"allroads.size(): "<<allroads.size();
        if(maxsize<allroads[e].size())
        {
            maxsize=allroads[e].size();
            qDebug()<<"maxsize: "<<maxsize;
        }
    }//最大步数确认，一下将这些路径进行处理就行
    for(int j=0;j<allroads.size();j++)
    {
        if(allroads[j].size()==maxsize)
        {
            //这里需要将这个索引号进行记录。
            std::vector<RoadsNode> temp=allroads[j];
            int tsize=temp.size();
            locationowner.push_back(temp[0].owner);
            xvector.push_back(temp[tsize-1].xpos);
            yvector.push_back(temp[tsize-1].ypos);
            numvec.push_back(j);//记录每个终点在所有线路中的对应线路的索引号
        }
    }

    //这个时候xvector等四个的结构是这样 棋子编号，落点位置，在all例的索引号
    int luodiansize=xvector.size();
    int nengchi=-2;
    for(int m=0;m<luodiansize;m++)
    {
        qDebug()<<"num: "<<locationowner[m]<<"can go to point: "<<xvector[m]<<' '<<yvector[m];
        if(nengchi<numvec[m])
        {
            nengchi=numvec[m];
        }
    }
    if(nengchi>-1)
    {
        qDebug()<<"能吃";
        for(int l=0;l<luodiansize;l++)
        {
            if(numvec[l]==-1)
            {
                qDebug()<<"第 "<<l<<" 棋子不能走";
                locationowner[l]=-10;
            }
        }
    }

}


void DraughtsBase::gameover(int color)
{
    int opponentcount=0;

    if(color==0)
    {
        for(int i=0;i<20;i++)
        {
            if(white[i].isAlive==true)
            {
                opponentcount++;
            }
        }

    }
    else if(color==1)
    {
        for(int j=0;j<20;j++)
        {
            if(black[j].isAlive==true)
            {
                opponentcount++;
            }
        }
    }

    if(opponentcount==0)
    {
        winFlag=color;
        emit gameresult(color);
        return;
    }

    if(color==0)
    {
        prepare(1);
        if(xvector.size()==0)
        {
            winFlag=color;
            emit gameresult(color);
            return ;
        }
    }
    else if(color==1)
    {
        prepare(0);
        if(xvector.size()==0)
        {
            winFlag=color;
            emit gameresult(color);
            return ;
        }
    }
}


