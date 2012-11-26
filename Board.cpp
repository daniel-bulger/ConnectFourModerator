#include "Board.h"
#include "Moderator.h"
#include <QtGui>

Board::Board(ControlPanel *parent) :parent(parent),
    QGraphicsView()
{
    moderator = parent->parent;
    //this->setRenderHint(QPainter::SmoothPixmapTransform);
    isResizing = false;
    isMoving = false;
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
    this->show(); // for some reason, this makes the board appear in Ubuntu
    int width = QApplication::desktop()->width();
    int height = QApplication::desktop()->height();
    if((width * 1574 / 1260) > (height)){
        this->resize(width*2/3,(width*2/3*1260/1574));
    }
    else{
        this->resize(height*2/3*1574/1260,height);
    }
    originalWidth = this->width();
    originalHeight = this->height();
    // Center (ish)!
    int center_x = QDesktopWidget().availableGeometry().center().x();
    int center_y = QDesktopWidget().availableGeometry().center().y();
    int total_width = QDesktopWidget().availableGeometry().width();
    this->move(center_x - this->width()/2 + (total_width / 8),center_y - this->height()/2);

    // Track the mouse wherever it goes!
    //this->setMouseTracking(true);

    // Paint the picture of the board
    boardImage = new QPixmap(":/images/board.png");
    QPixmap scaledBoardImage = boardImage->scaled(this->size(), Qt::KeepAspectRatio);
    highlightImage = new QPixmap(":/images/Highlight.png");
    QPixmap scaledHighlightImage = highlightImage->scaled(this->size(), Qt::KeepAspectRatio);
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
    if(parent->settings->value("boardsize").toInt()==1) resizeBoard(500);
    if(parent->settings->value("boardsize").toInt()==2) resizeBoard(1000);
    if(parent->settings->value("boardsize").toInt()==3) resizeBoard(1500);
    this->show();
    currentPlayer = 2;
    playerGoesFirst = 2;
    currentPiece = new Piece(currentPlayer, this);
}
float Board::OFFSET_LEFT(){return (-1.5/1574.0)*(originalWidth);}
float Board::COL_WIDTH(){return float((169.0/1574.0)*(originalWidth));}
float Board::COL_HEIGHT(){return COL_WIDTH();}
float Board::H_SPACING(){return float((40.6/1574)*(originalWidth));}
float Board::V_SPACING(){return float((21.0/1260.0)*(originalHeight));}
float Board::OFFSET_BOTTOM(){return float((68.0/1260)*(originalHeight));}

void Board::place(int col, int row)
{

    pieces.push_back(currentPiece);
    currentPiece->moveToCol(col);

    int destination = this->originalHeight - currentPiece->height() - (currentPiece->OFFSET_BOTTOM()) - (currentPiece->COL_HEIGHT()+currentPiece->V_SPACING())*(row-1);

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
void Board::resizeBoard(int width){
    oldWidth = this->width();
    if(oldWidth>width&&width<100) return;
    qDebug() << oldWidth;
    this->scale(float(width)/float(oldWidth),float(width)/float(oldWidth));
    this->scene->setSceneRect(0,0,this->width(),height());
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    this->resize(width,width*1260/1574);

    /*float ratio = float(width)/float(oldWidth);
    qDebug() << ratio;
    QPixmap scaledBoardImage = boardImage->scaled(this->size(), Qt::KeepAspectRatio);
    QPixmap scaledHighlightImage = highlightImage->scaled(this->size(), Qt::KeepAspectRatio);
    highlightGraphic->setPixmap(scaledHighlightImage);
    board->setPixmap(scaledBoardImage);
    for(int i = 0; i<pieces.size();i++){
        pieces[i]->width = this->width()*.109;
        *(pieces[i]->img) = pieces[i]->image->scaled(QSize(pieces[i]->width,pieces[i]->width));
        pieces[i]->setX(pieces[i]->x()*ratio);
        pieces[i]->setY(pieces[i]->y()*ratio);

    }*/
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
        animation->setEndValue(QPointF(pieces[i]->x(), this->originalHeight));
        animation->setEasingCurve(QEasingCurve::InQuad);
        connect(animation,SIGNAL(finished()),animation->targetObject(),SLOT(removeFromScene()));
        clearAnimations->addAnimation(animation);
    }
    clearAnimations->start();
    currentPiece = new Piece(currentPlayer, this);
}

void Board::gameResult(int player)
{
    if (player==-1) gameResultText->setText("PLAYER 1\n  ERROR");
    if (player==-2) gameResultText->setText("PLAYER 2\n  ERROR");
    if (player==1) gameResultText->setText("PLAYER 1\n    WINS");
    if (player==2) gameResultText->setText("PLAYER 2\n    WINS");
    if (player==3) gameResultText->setText("   GAME\nSTOPPED");
    if (!player) gameResultText->setText("TIE");

    gameResultText->setPos(originalWidth/2-gameResultText->boundingRect().width()/2, (originalHeight/2)-gameResultText->boundingRect().height()/2);
    gameResultText->show();
}

void Board::mouseMoveEvent(QMouseEvent *event)
{
    if(event!=0){
        if( event->buttons().testFlag(Qt::LeftButton) && isMoving)
    {
        this->move(this->pos() + (event->globalPos() - lastMousePos));
        lastMousePos = event->globalPos();
    }
        if(isResizing){
            resizeBoard(widthAtPress-mapFromGlobal(lastMousePos).x()+event->x());
        }
    }
    int x;
    if(event==0){
        x = this->mapFromGlobal(QCursor::pos()).x();
        x= round(float(x)/float(this->width())*(float)this->originalWidth);
    }
    else{
        x = round(float(event->x())/float(this->width())*(float)this->originalWidth);
    }
    if((parent->parent->gamestate==parent->PLAYER_1_TO_MOVE && parent->parent->player1->isManual)||
            (parent->parent->gamestate==parent->parent->PLAYER_2_TO_MOVE && parent->parent->player2->isManual)){
        int col = ceil((x+OFFSET_LEFT()-H_SPACING())/(COL_WIDTH()+H_SPACING()));
        if(col>7)
            col = 7;
        if(col<1)
            col = 1;

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
    if(parent->parent->gamestate==parent->PLAYER_1_TO_MOVE && parent->parent->player1->isManual){
        highlightGraphic->show();
    }
    else if(parent->parent->gamestate==parent->PLAYER_2_TO_MOVE && parent->parent->player2->isManual){
        highlightGraphic->show();
    }
}
void Board::hoverExitEvent(QEvent *event)
{
    highlightGraphic->hide();
}
void Board::mouseDoubleClickEvent(QMouseEvent *event){
    event->accept();
    int col = ceil((event->x()+OFFSET_LEFT()-H_SPACING())/(COL_WIDTH()+H_SPACING()));
    if(col>7)
        col = 7;
    if(parent->parent->controlPanel->doubleClickToPlacePiecePreference->isChecked())
        placePieceIfManual(col);
}

void Board::mousePressEvent(QMouseEvent *event){
    event->accept();
    if((event->button()==Qt::LeftButton)&&(((event->y()>this->height()*.925))&&(event->x()>this->width()*.925)))  {
        lastMousePos = event->globalPos();
        widthAtPress = this->width();
        isResizing = true;
    }
    else {
        if((event->button() == Qt::LeftButton)&&(!parent->boardLockedPreference->isChecked()))
        {
            lastMousePos = event->globalPos();
            isMoving = true;

        }
        int col = ceil((round(float(event->x())/float(this->width())*(float)this->originalWidth)+OFFSET_LEFT()-H_SPACING())/(COL_WIDTH()+H_SPACING()));
        if(col>7)
            col = 7;
        if(!parent->doubleClickToPlacePiecePreference->isChecked())
            placePieceIfManual(col);
    }

}
void Board::placePieceIfManual(int col){
    if(parent->parent->gamestate==parent->parent->PLAYER_1_TO_MOVE && parent->parent->player1->isManual){
        moderator->plyr1Move = col;
        moderator->player1MadeAMove = true;
    }
    else if(parent->parent->gamestate==parent->parent->PLAYER_2_TO_MOVE && parent->parent->player2->isManual){
        moderator->plyr2Move = col;
        moderator->player2MadeAMove = true;
    }
}

void Board::mouseReleaseEvent(QMouseEvent* event){
    if(event->button() == Qt::LeftButton){
        isMoving = false;
        isResizing = false;
    }
}

void Board::highlight(int col)
{
    int x = (H_SPACING()+OFFSET_LEFT()+(COL_WIDTH()+H_SPACING())*(col-1));
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
