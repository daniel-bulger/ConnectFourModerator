#ifndef CONTROL_H
#define CONTROL_H
#include "Game.h"
#include "Board.h"
#include "Moderator.h"
#include <QtGui>
#include <QtCore>
class Moderator;  // forward declaration
class ControlPanel : public QWidget
{
    Q_OBJECT

public:
    ControlPanel(Moderator* parent = 0);
    ~ControlPanel();
    void closeEvent(QCloseEvent *event);

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
    QSettings* settings;
    QString player1ProgramName;
    QString player2ProgramName;
    QStringList player1ProgramArgs;
    QStringList player2ProgramArgs;
    bool player1IsManual;
    bool player2IsManual;
    bool player1Move(int);
    bool player2Move(int);
    void lookForMove();
    void alert(QString message);
    void loadFailed(QString player);
    void console(QString message);
    void endGame();
    void resetGoButton();
    void setGoButton();
    void resetPauseButton();
    void setPauseButton();
    void showPauseButton();
    void hidePauseButton();
    void player1Wins();
    void player2Wins();

    static const int MOVE_TIME_LIMIT = 10;

 public slots:
    void player1Debug();
    void player2Debug();
    void populateComboBox1();
    void populateComboBox2();
    void moveToStartingLocation();
};

#endif
