#include "Piece.h"
#include <QPainter>
#include <QDebug>

Piece::Piece(int player, QGraphicsView *parent)
{
    QPixmap image;
    if (player == 1)
        image.load(":/images/red.png");
    else
        image.load(":/images/black.png");
    width = (parent->width()*.109);
    initialX = (parent->width()*.046);
    image = image.scaled(QSize(width, width));
    img = new QPixmap(image);
    this->setZValue(-1);

    // Set up these super annoying constant-ish variables
    OFFSET_LEFT   = (-1.5/1574.0)*(parent->width());
    COL_WIDTH     = (169.0/1574.0)*(parent->width());
    COL_HEIGHT    = COL_WIDTH;
    H_SPACING     = (41.9/1574)*(parent->width());
    V_SPACING     = (21.5/1260.0)*(parent->height());
    OFFSET_BOTTOM = (68.0/1260)*(parent->height());
}

void Piece::moveToCol(int col)
{
    this->setX(OFFSET_LEFT+(COL_WIDTH+H_SPACING)*(col-1));
    qDebug() << H_SPACING << endl;
}

void Piece::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
   painter->drawPixmap(QPoint(initialX,0), *img);
}

QRectF Piece::boundingRect() const
{
    return QRectF(initialX, 0, width, width);
}
