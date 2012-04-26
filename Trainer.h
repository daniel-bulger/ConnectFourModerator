#ifndef TRAINER_H
#define TRAINER_H

#include "Moderator.h"
#include "ControlPanel.h"
#include "Player.h"
#include "Tournament.h"
#include <QtGui>
#include <QtCore>
#include <QMainWindow>
class ControlPanel;
class Trainer : public QWidget
{
    Q_OBJECT

public:
    Trainer(ControlPanel* parent = 0, int type = 0);
    ~Trainer();


private:
    enum gamestates{GAME_STOPPED,PLAYER_1_TO_MOVE,PLAYER_2_TO_MOVE,PLAYER_1_QUESTION_MARK,PLAYER_2_QUESTION_MARK,GAME_PAUSED_PLAYER_1_TO_MOVE,GAME_PAUSED_PLAYER_2_TO_MOVE};
    Moderator* testerModerator;
    Moderator* trainerModerator;
    QSettings* settings;
    ControlPanel* parent;
    QSlider* timePerTurnSlider;
    QVector<QThread*> moderatorThreads;
    QVector<Moderator*> trainerModerators;
    QVector<QPair<int,int> > lastPlayers;
    QVBoxLayout* simpleLayout;
    QVBoxLayout* advancedLayout;
    QVBoxLayout* tournamentLayout;
    QVector<QHBoxLayout*> fileNameLayouts;
    QVBoxLayout* fileNameLayoutHolder;
    QPushButton* goButton;
    QPushButton* pauseButton;
    QPushButton* addPlayerButton;
    QPushButton* removePlayerButton;
    QListWidget* playerList;
    QString* AIFolder;
    QPushButton* chooseDirectoryButton;
    QLineEdit* chooseDirectoryText;
    QVector<QComboBox*>* playerFileNames;
    QLabel* fewestGamesPerCombinationLabel;
    QSpinBox* fewestGamesPerCombinationSpinBox;
    QLabel* numTrialsLabel;
    QLineEdit* numTrialsInput;
    Player* player1;
    Player* player2;
    QProgressBar* percentStarted;
    QProgressBar* percentFinished;
    QVector<QString> gameStrings;
    QVector<QString> playerNamesVector;
    QVector<QString> oppNamesVector;
    QVector<Tournament::win_state> gameWinners;
    bool switchSides;
    int player1PullIndex;
    int player2PullIndex;
    int gamesRemainingToBeStarted;
    int gamesRemainingToBeFinished;
    int type;
    bool gamesRunning;
    bool gamesArePaused;
    int numberOfCores;
    void closeEvent(QCloseEvent *event);
signals:
    void finished(Tournament::players_results_t results);
    void fin(int);
public slots:
    void appendToWinnersVector(int winner);
    void addPlayer();
    void removePlayer();
    void populateComboBoxes();
    void goButtonPressed();
    void chooseDirectory();
    void pauseButtonPressed();
    void stop();
    void start(int recursion = 0, QPair<QStringList,QStringList> fileNames = qMakePair(QStringList(),QStringList()));
    QPair<QStringList,QStringList> getFileNamesFromPlayerSelectors(bool isTournament = false);
    void addGamestringToVector(QString gamestring);
    void setGoButton();
    void resetGoButton();
    void showPauseButton();
    void hidePauseButton();
    void setPauseButton();
    void resetPauseButton();
    void gameHasEnded();
    QPair<int,int> getNextPlayers(bool isTournamentMode = false);
    void sleep(int mSecs);

};

#endif // TRAINER_H
