#ifndef MODERATOR_H
#define MODERATOR_H
#include "ControlPanel.h"
#include "Game.h"
#include "Board.h"
#include "Player.h"
#include <QtGui>
#include <QtCore>
class Board;
class ControlPanel; // forward declaration
class Moderator : public QMainWindow
{
    Q_OBJECT

public:
    Moderator(QWidget *parent = 0);
    ~Moderator();
    ControlPanel* controlPanel;
    Board* gameBoard;
    Game currentGame;
    enum gamestates{GAME_STOPPED,PLAYER_1_TO_MOVE,PLAYER_2_TO_MOVE,PLAYER_1_QUESTION_MARK,PLAYER_2_QUESTION_MARK,GAME_PAUSED_PLAYER_1_TO_MOVE,GAME_PAUSED_PLAYER_2_TO_MOVE};
    int gamestate;
    QString* AIFolder;
    int plyr1Move;
    int plyr2Move;
    int timeUntilMove;
    Player* player1;
    Player* player2;
    bool player1GoesFirst;
    bool player1MadeAMove;
    bool player2MadeAMove;
    QSettings* settings;
    bool playerMove(bool,int);
    bool eventFilter(QObject *, QEvent *);
    void lookForMove();
    void alert(QString message);
    void loadFailed(QString player);
    void console(QString message);
    void endGame();
    void resetGoButton();
    void player1Wins(bool dueToError = false);
    void player2Wins(bool dueToError = false);
    void tieGame();
    bool testProgram(QString progName, QStringList args = QStringList());

    static const int MOVE_TIME_LIMIT = 10;

 public slots:
    void chooseDirectory();
    void directoryTextBoxEdited();
    void decrementTimePerTurnTimer();
    void updateTimer();
    void playerHasMoved(bool isPlayer1);
    void player1HasMoved();
    void player2HasMoved();
    void player1DroppedPiece(int col);
    void player2DroppedPiece(int col);
    void pauseButtonPressed();
    void goButtonPressed();
    bool loadPlayer1Program(int index);
    bool loadPlayer2Program(int index);
    bool loadPlayerProgram(bool isPlayer1, int index);
    bool goPlayer1Program(int index);
    bool goPlayer2Program(int index);
};

#endif
