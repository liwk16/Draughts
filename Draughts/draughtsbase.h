#ifndef DRAUGHTSBASE_H
#define DRAUGHTSBASE_H

#include <QObject>
#include <vector>

struct Chess
{
    int xpos;
    int ypos;
    int color;
    bool isKing;
    bool isAlive;
    int num;
};

struct RoadsNode
{
    int owner;
    int xpos;
    int ypos;
};

class DraughtsBase:public QObject//这是一个单例
{
    Q_OBJECT

public:
    DraughtsBase(QObject* parent=0);

    void findWays(int,int);

    void prepare(int);

    void setDst(int,int);
    bool islegal();

    void dfs(int,int,int,int);//x,y,color,num
    void dfsKing(int, int, int, int);//
    bool caneat(Chess*) const;
    bool kingcaneat(Chess*) const;
    bool checkloc(int,int) const;
    void gameover(int);
signals:
    void chessmove(int);//信号，几号棋子被移动了。
    void highlight(int);
    void eatWhite();
    void eatBlack();
    void chat(int,int,int,int);
    void gameresult(int);
public:
    Chess* black;
    Chess* white;
    int dstr;
    int dstc;
    Chess* curchess;
    static int a[10][10][2];
    int flag[10][10];
    int dx[4];
    int dy[4];
    int curline;
    std::vector<int> locationowner;
    std::vector<int> xvector;
    std::vector<int> yvector;
    std::vector<int> numvec;//记录道路索引
    std::vector<RoadsNode> roads;
    //std::vector<int> eatvec;
    std::vector<std::vector<RoadsNode>> allroads;
    int colorlock;
    int winFlag;
};

#endif // DRAUGHTSBASE_H
