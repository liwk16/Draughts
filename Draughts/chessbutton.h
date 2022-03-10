#ifndef CHESSBUTTON_H
#define CHESSBUTTON_H

#include <QPushButton>

class ChessButton:public QPushButton
{
    Q_OBJECT
public:
    ChessButton();
    ChessButton(QWidget* parent);
protected:
    void focusInEvent(QFocusEvent *e);
   // void focusOutEvent();
signals:
    void chessselected(int,int);
private:
    static int bxpos;
    static int bypos;
public:

    int color;
    int num;
};

#endif // CHESSBUTTON_H
