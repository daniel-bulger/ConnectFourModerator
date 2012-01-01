#ifndef BOARD_H
#define BOARD_H

#include <QtGui>
#include "Piece.h"
class Moderator;
class Board : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Board(Moderator *parent = 0);
    void clearPieces();
    void place(int row, int col);
    void gameResult(int player);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void highlight(int col);
    void moveEvent(QMoveEvent * event);
    int OFFSET_LEFT;
    int COL_WIDTH;
    int COL_HEIGHT;
    int H_SPACING;
    float V_SPACING;
    int OFFSET_BOTTOM;
    Moderator* parent;

protected:
    int currentPlayer;
    int playerGoesFirst;
    QGraphicsScene* scene;
    QGraphicsPixmapItem* board;
    Piece* currentPiece;
    QVector<Piece*> pieces;
    QVector<QPropertyAnimation*> animations;
    QParallelAnimationGroup* clearAnimations;
    QGraphicsSimpleTextItem* gameResultText;
signals:
    void pieceDroppedByPlayer(int col);
};

#endif // BOARD_H
