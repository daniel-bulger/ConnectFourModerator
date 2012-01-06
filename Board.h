#ifndef BOARD_H
#define BOARD_H

#include <QtGui>
#include "Piece.h"
#include <QGraphicsObject>
#include <QGraphicsView>
class Piece;
class Moderator;
class Board : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Board(Moderator *parent = 0);
    void clearPieces();
    void place(int row, int col);
    void resizeBoard(int width);
    void gameResult(int player);
    void mouseMoveEvent(QMouseEvent *event = 0);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void placePieceIfManual(int col);
    void mouseReleaseEvent(QMouseEvent *event);
    bool eventFilter(QObject *, QEvent *);
    void hoverExitEvent(QEvent *event);
    void hoverEnterEvent(QEvent *event);
    void highlight(int col);
    void moveEvent(QMoveEvent * event);
    int oldWidth;
    int originalWidth;
    int widthAtPress;
    int originalHeight;
    float OFFSET_LEFT();
    float COL_WIDTH();
    float COL_HEIGHT();
    float H_SPACING();
    float V_SPACING();
    float OFFSET_BOTTOM();
    QPoint lastMousePos;
    bool isMoving;
    bool isResizing;
    Moderator* parent;

    int currentPlayer;
    int playerGoesFirst;
    QPixmap* boardImage;
    QPixmap* highlightImage;
    QGraphicsScene* scene;
    QGraphicsPixmapItem* board;
    QGraphicsPixmapItem* highlightGraphic;
    Piece* currentPiece;
    QVector<Piece*> pieces;
    QVector<QPropertyAnimation*> animations;
    QParallelAnimationGroup* clearAnimations;
    QGraphicsSimpleTextItem* gameResultText;
signals:
    void pieceDroppedByPlayer(int col);
};

#endif // BOARD_H
