#ifndef DRAUGHTS_H
#define DRAUGHTS_H

#include <QDialog>
#include <QMouseEvent>
#include <QTextCodec>
#include <QtNetwork/QTcpSocket>

#include <QString>

class DraughtsBase;
class ChessButton;

namespace Ui {
class Dialog;
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


    void recvMessage();
    void sendInfo(int,int,int,int);

    void renshuAct();
    void qiuheAct();

    void gamefinished(int);

    void writeIP();
    void writeport();

public:
    void connectHost();
protected:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
signals:
    //void newDst(int,int);
private:
    Ui::Dialog *ui;
    DraughtsBase* logicBase;
    ChessButton* black;
    ChessButton* white;

    QTcpSocket* readWriteSocket;
    int curnum;
    int qiuheSwitch;
    int step;
    QString IP;
    int port;
};

#endif // DRAUGHTS_H
