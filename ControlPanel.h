#ifndef CONTROL_H
#define CONTROL_H
#include "Game.h"
#include "Board.h"
#include "Moderator.h"
#include "Tournament.h"
#include "Trainer.h"
#include "Player.h"
#include <QtGui>
#include <QtCore>
#include <QMainWindow>
#include <algorithm>
#include <cmath>
class Moderator;  // forward declaration
class Trainer;
class ControlPanel : public QWidget
{
    Q_OBJECT

public:
    ControlPanel();
    ~ControlPanel();
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent* event);
    void hideEvent(QHideEvent *);
    void focusInEvent(QFocusEvent *);
    void focusOutEvent(QFocusEvent *);
    void mousePressEvent(QMouseEvent *);
    enum gamestates{GAME_STOPPED,PLAYER_1_TO_MOVE,PLAYER_2_TO_MOVE,PLAYER_1_QUESTION_MARK,PLAYER_2_QUESTION_MARK};
    int gamestate;
    // Child windows
    //Board board;
    //Debug debug;
    // GUI
    Moderator* parent;
    Board* gameBoard;
    QVBoxLayout* layout;
    QTextEdit* moveHistory;
    QVBoxLayout* player1Section;
    QVBoxLayout* player2Section;
    QHBoxLayout* player1Input;
    QHBoxLayout* player2Input;
    QHBoxLayout* player1Status;
    QHBoxLayout* player2Status;
    QHBoxLayout* delaySection;
    QHBoxLayout* chooseDirectorySection;
    QLabel* player1Label;
    QLabel* player2Label;
    QComboBox* player1FileName;
    QComboBox* player2FileName;
    QLabel* player1TimeRemainingLabel;
    QLabel* player2TimeRemainingLabel;
    QProgressBar* player1TimeRemainingBar;
    QProgressBar* player2TimeRemainingBar;
    QPushButton* go;
    QPushButton* pause;
    QImage* image;
    QLabel* ConnectFourBoard;
    QScrollArea* scrollArea;
    QLabel* timePerTurnLabel;
    QSlider* timePerTurnSlider;
    QTimer* timePerTurnTimer;
    QTimer* timer;
    QPushButton* chooseDirectoryButton;
    QLineEdit* chooseDirectoryText;
    QMenuBar* menuBar;
    QString* AIFolder;
    QVector<Trainer*> trainers;
    QMenu* fileMenu;
    QMenu* helpMenu;
    QMenu* preferencesMenu;
    QMenu* boardSizeSubmenu;
    QMenu* trainerMenu;
    QActionGroup* boardSizeChoices;
    QAction* boardBackgroundPreference;
    QAction* boardLockedPreference;
    QAction* boardBackgroundColor;
    int boardRed;
    int boardBlue;
    int boardGreen;
    QAction* timeLimitPreference;
    QAction* doubleClickToPlacePiecePreference;
    QAction* showOnlyGoodPrograms;
    QAction* boardSmall;
    QAction* boardMedium;
    QAction* boardLarge;
    QAction* boardAuto;
    QSettings* settings;
    QString player1ProgramName;
    QString player2ProgramName;
    QStringList player1ProgramArgs;
    QStringList player2ProgramArgs;
    bool player1IsManual;
    bool player2IsManual;
    bool startingGame;
    bool player1Move(int);
    bool player2Move(int);
    void lookForMove();
    void alert(QString message);
    void resetGoButton();
    void setGoButton();
    void resetPauseButton();
    void setPauseButton();
    void showPauseButton();
    void hidePauseButton();
    void player1Wins();
    void player2Wins();
    void showHTML(QString data);
    QStringList getFileNameFromPlayerSelector(bool isPlayer1);
    static const int MOVE_TIME_LIMIT = 10;
    Tournament* tournament;

 public slots:
    void onApplicationFocusChanged(QWidget*, QWidget*);
    void connectManualInputToModerator();
    void gameHasEnded();
    void loadFailed(QString player);
    void displayWinner(int winner);
    void console(QString message);
    void chooseAIsToTrain();
    void runMassTrials();
    void chooseBoardColors();
    void player1Debug();
    void player2Debug();
    void populateComboBoxes();
    void populateComboBox(bool isPlayer1);
    void moveToStartingLocation();
    void displayReadme();
    void displayAbout();
    void boardBackChanged(bool isChecked);
    void boardAutoChanged(bool isChecked,bool recur = true);
    void boardSmallChanged(bool isChecked, bool recur = true);
    void boardMediumChanged(bool isChecked, bool recur = true);
    void boardLargeChanged(bool isChecked,bool recur = true);
    void updateTimeRemainingSlider(int timeRemaining, bool isPlayer1);
    void updateCurrentPlayerTimeRemainingSlider();
    void placePieceOnBoard(int x, int y);
    void playerToMoveBoardUpdater(bool isManual);
    void goButtonPressed();
    void pauseButtonPressed();
    void chooseDirectory();
    void showSuccessfulLoad(QStringList);
    void initTournament();
    void startTournament(Tournament::players_results_t results);

};

#endif
