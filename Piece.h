#ifndef PIECE_H
#define PIECE_H

#include <QGraphicsObject>
#include <QGraphicsView>

class Piece : public QGraphicsObject
{
    Q_OBJECT
public:
    Piece(int player, QGraphicsView *parent = 0);
    int height() const { return width; }
    void moveToCol(int col);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    int OFFSET_LEFT;
    int COL_WIDTH;
    int COL_HEIGHT;
    int H_SPACING;
    float V_SPACING;
    int OFFSET_BOTTOM;
protected:
    QPixmap* img;
    int width;
    int initialX;
};



#endif // PIECE_H
