#ifndef PIECE_H
#define PIECE_H

#include <QGraphicsObject>
#include <QGraphicsView>
#include "Board.h"
class Board;
class Piece : public QGraphicsObject
{
    Q_OBJECT
public:
    Board* parent;
    QPixmap* image;
    int player;
    Piece(int player, Board *parent = 0);
    int height() const { return width; }
    void moveToCol(int col);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    float OFFSET_LEFT();
    float COL_WIDTH();
    float COL_HEIGHT();
    float H_SPACING();
    float V_SPACING();
    float OFFSET_BOTTOM();
    QPixmap* img;
    int width;
    int initialX;
public slots:
    void removeFromScene();
};



#endif // PIECE_H
