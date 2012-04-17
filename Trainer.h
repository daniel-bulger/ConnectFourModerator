#ifndef TRAINER_H
#define TRAINER_H

#include "Moderator.h"
#include "ControlPanel.h"
#include "Player.h"
#include <QtGui>
#include <QtCore>
#include <QMainWindow>
class ControlPanel;
class Trainer : public QWidget
{
    Q_OBJECT

public:
    Trainer(ControlPanel* parent = 0, bool isSimple = true);
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
    QPushButton* goButton;
    QPushButton* pauseButton;
    QString* AIFolder;
    QPushButton* chooseDirectoryButton;
    QLineEdit* chooseDirectoryText;
    QVector<QComboBox*>* playerFileNames;
    QPushButton* addPlayer;
    QPushButton* removePlayer;
    QLabel* fewestGamesPerCombinationLabel;
    QSpinBox* fewestGamesPerCombinationSpinBox;
    QLabel* numTrialsLabel;
    QLineEdit* numTrialsInput;
    Player* player1;
    Player* player2;
    QProgressBar* percentStarted;
    QProgressBar* percentFinished;
    bool switchSides;
    int player1PullIndex;
    int player2PullIndex;
    int gamesRemainingToBeStarted;
    int gamesRemainingToBeFinished;
    bool gamesRunning;
    bool gamesArePaused;
    int numberOfCores;
    void closeEvent(QCloseEvent *event);

public slots:
    void populateComboBoxes();
    void goButtonPressed();
    void chooseDirectory();
    void pause();
    void resume();
    void stop();
    void start(int recursion = 0);
    QPair<QStringList,QStringList> getFileNamesFromPlayerSelectors();
    void setGoButton();
    void resetGoButton();
    void showPauseButton();
    void hidePauseButton();
    void setPauseButton();
    void resetPauseButton();
    void gameHasEnded();
    QPair<int,int> getNextPlayers();
    void sleep(int mSecs);

};

#endif // TRAINER_H
