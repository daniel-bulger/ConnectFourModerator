#ifndef MODERATOR_H
#define MODERATOR_H
#include "ControlPanel.h"
#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "Trainer.h"
#include <QtGui>
#include <QtCore>
#include <stdio.h>
#include <iostream>
class Board;
class Trainer;
class ControlPanel; // forward declaration
class Moderator:public QObject
{
    Q_OBJECT

public:
    int getCurrentPlayer();
    Moderator();
    ~Moderator();
    ControlPanel* controlPanel;
    Game currentGame;
    enum gamestates{GAME_STOPPED,PLAYER_1_TO_MOVE,PLAYER_2_TO_MOVE,PLAYER_1_QUESTION_MARK,PLAYER_2_QUESTION_MARK,GAME_PAUSED_PLAYER_1_TO_MOVE,GAME_PAUSED_PLAYER_2_TO_MOVE};
    int gamestate;
    void qSleep(int ms);

    int plyr1Move;
    int plyr2Move;
    int timeUntilMove;
    int timePerMove;
    bool commandLine;
    Player* player1;
    Player* player2;
    bool player1GoesFirst;
    bool player1MadeAMove;
    bool player2MadeAMove;
    Trainer* trainer;
    QTimer* timer;
    QTimer* timePerTurnTimer;
    QString logFilePath;
    bool playerMove(bool,int);
    void lookForMove();
    void alert(QString message);
    void console(QString message);
    void endGame();
    void resetGoButton();
    void player1Wins(bool dueToError = false);
    void player2Wins(bool dueToError = false);
    void tieGame();
    bool testProgram(QString progName, QStringList args = QStringList(), int timeToRespond = 100);
    bool initializeProgram(Player* &player, QString progName, QStringList args = QStringList(), int timeToRespond = 100);
    static const int MOVE_TIME_LIMIT = 10;
private:
    int timeRemaining;
    int delayBeforeMove;
signals:
    void timeUntilMoveChanged(int timeRemaining, bool isPlayer1);
    void consoleOutput(QString message);
    void gameOver(int winner);
    void piecePlaced(int x, int y);
    void player2ToMove(bool isManual);
    void player1ToMove(bool isManual);
    void loadFailed(QString player);
    void gameHasEnded();
    void loadSuccess(QStringList programName);
    void acceptManualInput();
    void gamestring(QString gamestring);
public slots:
    void writeLineToTerminal(QString message);
    void directoryTextBoxEdited();
    void decrementTimePerTurnTimer();
    void updateTimer();
    void playerHasMoved(bool isPlayer1);
    void player1HasMoved();
    void player2HasMoved();
    void player1DroppedPiece(int col);
    void player2DroppedPiece(int col);
    void pauseGame();
    void resumeGame();
    bool startGame(QStringList player1FileName, QStringList player2FileName, QString logFolder = "", bool swapTurns = true, bool cmd = false, int timeToRespond = 100);
    void setTimeUntilMove(int msecs);
    bool startProgram(QStringList programName, bool isPlayer1, int timeToRespond = 100);
};

#endif
