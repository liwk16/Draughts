#include "chessbutton_client.h"
#include <QDebug>

int bxpos=-1;
int bypos=-1;

ChessButton::ChessButton(){}
ChessButton::ChessButton(QWidget *parent):
    QPushButton(parent)

{

}

void ChessButton::focusInEvent(QFocusEvent *e)
{
    qDebug()<<"focus Changed!";
    qDebug()<<this->color<<" "<<this->num;
    //qDebug()<<this->bxpos<<" "<<this->bypos;
    //将自己的颜色和编号传递给base棋盘，让base知道该给谁寻路
    emit chessselected(this->color,this->num);
}
