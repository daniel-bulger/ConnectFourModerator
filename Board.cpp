#include "Board.h"
#include "Moderator.h"
#include <QtGui>

Board::Board(Moderator *parent) :parent(parent),
    QGraphicsView()
{
    // Create the view and the scene!
    scene = new QGraphicsScene();
    this->setScene(scene);
    // Make it all cool and transparent and frameless and everything like that!
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->window()->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setAutoFillBackground(false);
    this->setStyleSheet( "QGraphicsView { border-style: none; background: transparent; }" );

    // Resize!
    int width = QApplication::desktop()->width();
    int height = QApplication::desktop()->height();
    if((width * 1574 / 1260) > (height)){
        this->resize(width*2/3,(width*2/3*1260/1574));
    }
    else{
        this->resize(height*2/3*1574/1260,height);
    }

    // Center (ish)!
    int center_x = QDesktopWidget().availableGeometry().center().x();
    int center_y = QDesktopWidget().availableGeometry().center().y();
    int total_width = QDesktopWidget().availableGeometry().width();
    this->move(center_x - this->width()/2 + (total_width / 8),center_y - this->height()/2);

    // Track the mouse wherever it goes!
    //this->setMouseTracking(true);

    // Paint the picture of the board
    QPixmap boardImage(":/images/board.png");
    QPixmap scaledBoardImage = boardImage.scaled(this->size(), Qt::KeepAspectRatio);
    QPixmap highlightImage(":/images/Highlight.png");
    QPixmap scaledHighlightImage = highlightImage.scaled(this->size(), Qt::KeepAspectRatio);
    highlightGraphic = new QGraphicsPixmapItem(scaledHighlightImage);
    board = new QGraphicsPixmapItem(scaledBoardImage);
    board->setAcceptsHoverEvents(true);
    board->setAcceptHoverEvents(true);
    scene->installEventFilter(this);
    scene->addItem(highlightGraphic);
    highlightGraphic->hide();
    scene->addItem(board);
    gameResultText = new QGraphicsSimpleTextItem();
    gameResultText->setBrush(QBrush(Qt::white));
    QFont font = QFont("Arial Black", 100);
    gameResultText->setFont(font);
    gameResultText->setPen(QPen(Qt::black));
    scene->addItem(gameResultText);

    this->show();

    currentPlayer = 2;
    playerGoesFirst = 2;
    currentPiece = new Piece(currentPlayer, this);
    OFFSET_LEFT   = (-1.5/1574.0)*(this->width());
    COL_WIDTH     = (169.0/1574.0)*(this->width());
    COL_HEIGHT    = COL_WIDTH;
    H_SPACING     = (41.9/1574)*(this->width());
    V_SPACING     = (21.5/1260.0)*(this->height());
    OFFSET_BOTTOM = (68.0/1260)*(this->height());
}

void Board::place(int col, int row)
{

    pieces.push_back(currentPiece);
    currentPiece->moveToCol(col);

    int destination = this->height() - currentPiece->height() - (currentPiece->OFFSET_BOTTOM) - (currentPiece->COL_HEIGHT+currentPiece->V_SPACING)*(row-1);

    QPropertyAnimation* animation = new QPropertyAnimation(currentPiece, "pos");
    animations.push_back(animation);
    animation->setDuration(1000);
    animation->setStartValue(QPointF(currentPiece->x(), 0));
    animation->setEndValue(QPointF(currentPiece->x(), destination));
    animation->setEasingCurve(QEasingCurve::OutBounce);
    animation->start();

    scene->addItem(currentPiece);

    // Switch the current players
    if (currentPlayer == 1) currentPlayer = 2;
    else currentPlayer = 1;

    currentPiece = new Piece(currentPlayer, this);
}

void Board::clearPieces()
{
    if (playerGoesFirst == 1)
        playerGoesFirst = currentPlayer = 2;
    else
        playerGoesFirst = currentPlayer = 1;
    gameResultText->hide();
    /*for (int i=0; i<animations.size(); i++)
    {
        delete animations[i];
    }
    animations.clear();*/
    clearAnimations = new QParallelAnimationGroup();
    QPropertyAnimation* animation;
    for (int i=0; i<pieces.size(); i++)
    {
        animation = new QPropertyAnimation(pieces[i], "pos");
        animation->setDuration(1000);
        animation->setStartValue(QPointF(pieces[i]->x(), pieces[i]->y()));
        animation->setEndValue(QPointF(pieces[i]->x(), this->height()));
        animation->setEasingCurve(QEasingCurve::InQuad);
        clearAnimations->addAnimation(animation);
    }
    clearAnimations->start();
    currentPiece = new Piece(currentPlayer, this);
}

void Board::gameResult(int player)
{
    if (player==1) gameResultText->setText("PLAYER 1\n    WINS");
    if (player==2) gameResultText->setText("PLAYER 2\n    WINS");
    if (!player) gameResultText->setText("TIE");

    gameResultText->setPos(width()/2-gameResultText->boundingRect().width()/2, (height()/2)-gameResultText->boundingRect().height()/2);
    gameResultText->show();
}
void Board::mouseMoveEvent(QMouseEvent *event)
{
    int x;
    if(event==0){
        x = this->mapFromGlobal(QCursor::pos()).x();
    }
    else{
        x = event->x();
    }
    if((parent->gamestate==parent->PLAYER_1_TO_MOVE && parent->player1->isManual)||
            (parent->gamestate==parent->PLAYER_2_TO_MOVE && parent->player2->isManual)){
        int col = ceil((x+OFFSET_LEFT-H_SPACING)/(COL_WIDTH+H_SPACING));
        if(col>6)
            col = 6;
        if(col<0)
            col = 0;
        col+=1;
        this->highlight(col);
    }
}
bool Board::eventFilter(QObject *object, QEvent *event)
 {
//qobject_cast<QGraphicsItem*>(object) == board)
     if(event->type() == QEvent::Enter) {
         qDebug() << "Enter";
         hoverEnterEvent((event));
     }
     if (event->type() == QEvent::Leave) {
         qDebug() << "exit";
         hoverExitEvent(event);
     }
    return false;
 }
void Board::hoverEnterEvent(QEvent *event)
{
    if(parent->gamestate==parent->PLAYER_1_TO_MOVE && parent->player1->isManual){
        highlightGraphic->show();
    }
    else if(parent->gamestate==parent->PLAYER_2_TO_MOVE && parent->player2->isManual){
        highlightGraphic->show();
    }
}
void Board::hoverExitEvent(QEvent *event)
{
    highlightGraphic->hide();
}

void Board::mousePressEvent(QMouseEvent *event){
    event->accept();
    int col = ceil((event->x()+OFFSET_LEFT-H_SPACING)/(COL_WIDTH+H_SPACING));
    if(col>6)
        col = 6;
    col+=1;
    if(parent->gamestate==parent->PLAYER_1_TO_MOVE && parent->player1->isManual){
        parent->plyr1Move = col;
        parent->player1MadeAMove = true;
    }
    else if(parent->gamestate==parent->PLAYER_2_TO_MOVE && parent->player2->isManual){
        parent->plyr2Move = col;
        parent->player2MadeAMove = true;
    }
}

void Board::highlight(int col)
{
    qDebug() << col;
    int x = (H_SPACING+OFFSET_LEFT+(COL_WIDTH+H_SPACING)*(col-1));
    qDebug() << x;
    this->highlightGraphic->setPos(x,0);
    highlightGraphic->show();
}


void Board::moveEvent(QMoveEvent * event)
{
    this->hide();
}
    /*************************************************
      KEEPING THIS AROUND JUST IN CASE!
      ************************************************
    const int width = QApplication::desktop()->width();
    const int height = QApplication::desktop()->height();
    //this->resize(width*1/3,height*1/3);
    aspectRatio = 1574/1260;
    if((width * 1574 / 1260)>(height)){
        this->resize(width*2/3,(width*2/3*1260/1574));
    }
    else{
        this->resize(height*2/3*1574/1260,height);
    }

    layout = new QVBoxLayout;
    //gs = new QGraphicsScene;
    //gv = new QGraphicsView(gs);
    //gv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //gv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //gv->resize(size());
    boardImage = new QPixmap(":/images/board.png");
    qDebug() << boardImage->width();
    qDebug() << boardImage->height();
    scaledBoardImage = boardImage->scaled(size(),Qt::KeepAspectRatio);
    label = new QLabel();
    label->setPixmap(scaledBoardImage);
    label->resize(size());
    //gpi = new QGraphicsPixmapItem(scaledBoardImage);
    //gs->addItem(gpi);
    layout->setMargin(0);
    //layout->addWidget(gv);

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

   // QPalette myPalette;
   // myPalette.setBrush(QPalette::Background,scaledBoardImage);
    ///this->setPalette(myPalette);
    layout->addWidget(label);
    setLayout(layout);
    //setStyleSheet("background-image: url(:/images/board.png)");
   moveToCenter();

}

void Board::moveToCenter(){
    int center_x = QDesktopWidget().availableGeometry().center().x();
    int center_y = QDesktopWidget().availableGeometry().center().y();
    int total_width = QDesktopWidget().availableGeometry().width();
    move(center_x-width()/2 + (total_width / 8),center_y-height()/2);

}*/
