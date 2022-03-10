#ifndef DRAUGHTS_H
#define DRAUGHTS_H

#include <QtMultimedia/QSound>
#include <QDialog>
#include <QMouseEvent>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include <QTextCodec>
#include <QtNetwork/QHostInfo>
#include <QString>

class DraughtsBase;
class ChessButton;

namespace Ui {
class Draughts;
}

class Draughts : public QDialog
{
    Q_OBJECT

public:
    explicit Draughts(QWidget *parent = 0);
    ~Draughts();
public slots:
    void place(int);
    void selectedAction(int,int);
    void moveToDst(int,int);
    void redraw(int);
    void eatBlackChess();
    void eatWhiteChess();

    void acceptConnection();
    void recvMessage();
    void sendInfo(int,int,int,int);

    void renshuAct();
    void qiuheAct();

    void gamefinished(int);
    void initServer();

    void clearallAndDefine();

    void gamestart();

public:
    //void initServer(int);
    QString getIP();
protected:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
signals:
    //void newDst(int,int);
private:
    Ui::Draughts *ui;
    DraughtsBase* logicBase;
    ChessButton* black;
    ChessButton* white;
    QTcpServer* listenSocket;
    QTcpSocket* readWriteSocket;
    int curnum;
    int qiuheSwitch;
    int step;
    int gamestate;

    int blackcount;
    int whitecount;
};

#endif // DRAUGHTS_H
