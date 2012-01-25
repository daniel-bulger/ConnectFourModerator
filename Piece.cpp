#include "Piece.h"
#include <QPainter>
#include <QDebug>

Piece::Piece(int player, Board *parent):parent(parent),player(player)
{
    image = new QPixmap();
    if (player == 1)
        image->load(":/images/red.png");
    else
        image->load(":/images/black.png");
    width = (parent->originalWidth*.109);
    initialX = (parent->originalWidth*.046);
    img = new QPixmap(image->scaled(QSize(width,width)));
    this->setZValue(-1);

}
float Piece::OFFSET_LEFT(){
    return parent->OFFSET_LEFT();
}
float Piece::COL_WIDTH(){return parent->COL_WIDTH();}
float Piece::COL_HEIGHT(){return COL_WIDTH();}
float Piece::H_SPACING(){return parent->H_SPACING();}
float Piece::V_SPACING(){return parent->V_SPACING();}
float Piece::OFFSET_BOTTOM(){return parent->OFFSET_BOTTOM();}
void Piece::moveToCol(int col)
{
    this->setX(OFFSET_LEFT()+(COL_WIDTH()+H_SPACING())*(col-1));
    qDebug() << col << endl;
}

void Piece::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
   painter->drawPixmap(QPoint(initialX,0), *img);
}

QRectF Piece::boundingRect() const
{
    return QRectF(initialX, 0, width, width);
}
void Piece::removeFromScene(){
    parent->scene->removeItem(this);
    int i;
    for(i = 0; i<parent->pieces.size(); i++){
        if((parent->pieces[i])==(this)) break;
    }
    parent->pieces.remove(i);
}
