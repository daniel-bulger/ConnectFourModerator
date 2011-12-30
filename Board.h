#ifndef BOARD_H
#define BOARD_H

#include <QtGui>
#include "Piece.h"

class Board : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Board(QObject *parent = 0);
    void clearPieces();
    void place(int row, int col);
    void gameResult(int player);
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
