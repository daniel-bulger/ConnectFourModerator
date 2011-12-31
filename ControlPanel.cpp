#include "ControlPanel.h"

ControlPanel::ControlPanel(Moderator *theParent)
    : QWidget(0)
{
    parent = theParent;
    //initialize move history test area.
    layout = new QVBoxLayout();
    moveHistory = new QTextEdit("Welcome to Connect Four!  There will be instructions here at some point.");
    moveHistory->setReadOnly(true);
    //Player labels.
    player1Label = new QLabel("Player 1:");
    player2Label = new QLabel("Player 2:");
    //choose your directory
    chooseDirectoryButton = new QPushButton("Load from...");
    chooseDirectoryText = new QLineEdit();
    chooseDirectoryText->setText(*(parent->AIFolder));
    //initializing players' AI file name combo boxes.
    player1FileName = new QComboBox();
    player1FileName->setMinimumWidth(100);
    player2FileName = new QComboBox();
    player2FileName->setMinimumWidth(100);
    populateComboBox1();
    populateComboBox2();
    //initializing players' time remaining bars and time per turn slider
    player1TimeRemainingBar = new QProgressBar;
    player1TimeRemainingBar->setRange(0, MOVE_TIME_LIMIT*1000);
    player1TimeRemainingBar->setValue(MOVE_TIME_LIMIT*1000);
    player1TimeRemainingBar->setTextVisible(true);
    player2TimeRemainingBar = new QProgressBar;
    player2TimeRemainingBar->setRange(0, MOVE_TIME_LIMIT*1000);
    player2TimeRemainingBar->setValue(MOVE_TIME_LIMIT*1000);
    player2TimeRemainingBar->setTextVisible(true);
    player1TimeRemainingLabel = new QLabel("Time remaining:");
    player2TimeRemainingLabel = new QLabel("Time remaining:");
    timePerTurnLabel = new QLabel("Delay:");
    timePerTurnSlider = new QSlider(Qt::Horizontal);
    timePerTurnSlider->setMinimum(0);
    timePerTurnSlider->setMaximum(100);
    timePerTurnSlider->setValue(30);
    timePerTurnSlider->setTickPosition(QSlider::TicksBelow);
    timePerTurnSlider->setTickInterval(10);
    timePerTurnSlider->setSingleStep(5);
    timePerTurnTimer = new QTimer();
    timer = new QTimer();
    //initializing game start/stop button
    go = new QPushButton("GO!");
    go->setFixedHeight(100);
    QFont goFont = go->font();
    goFont.setPointSize(36);
    goFont.setBold(true);
    go->setFont(goFont);
    QPalette pal = go->palette();
    pal.setColor(QPalette::ButtonText, Qt::darkGreen);
    go->setPalette(pal);
    //initializing pause button
    pause = new QPushButton("Pause");
    pause->setVisible(false);
    //initializing layout of control panel.
    setWindowTitle(tr("Connect Four"));
    layout = new QVBoxLayout();
    layout->addWidget(moveHistory);
    chooseDirectorySection = new QHBoxLayout();
    chooseDirectorySection->addWidget(chooseDirectoryButton);
    chooseDirectorySection->addWidget(chooseDirectoryText);
    layout->addLayout(chooseDirectorySection);
    player1Input = new QHBoxLayout();
    player2Input = new QHBoxLayout();
    player1Status = new QHBoxLayout();
    player2Status = new QHBoxLayout();
    player1Section = new QVBoxLayout();
    player2Section = new QVBoxLayout();
    delaySection = new QHBoxLayout();
    delaySection->addWidget(timePerTurnLabel);
    delaySection->addWidget(timePerTurnSlider);
    player1Section->addLayout(player1Input);
    player2Section->addLayout(player2Input);
    player1Input->addWidget(player1Label);
    player2Input->addWidget(player2Label);
    player1Input->addWidget(player1FileName);
    player2Input->addWidget(player2FileName);
    player1Section->addLayout(player1Status);
    player2Section->addLayout(player2Status);
    player1Status->addWidget(player1TimeRemainingLabel);
    player2Status->addWidget(player2TimeRemainingLabel);
    player1Status->addWidget(player1TimeRemainingBar);
    player2Status->addWidget(player2TimeRemainingBar);
    layout->addLayout(player1Section);
    layout->addLayout(player2Section);
    layout->addLayout(delaySection);
    layout->addWidget(pause);
    layout->addWidget(go);
    setLayout(layout);
    moveToStartingLocation();
    this->show();
}

ControlPanel::~ControlPanel()
{

}
// Application will exit when this pane is closed.  Any handling before exit should go here.
void ControlPanel::closeEvent(QCloseEvent *event){
  QApplication::quit();
}

void ControlPanel::populateComboBox1(){
    player1FileName->clear();
    player1FileName->insertItem(player1FileName->count(),"Choose...","NONE_SELECTED");
    //player1FileName->insertItem(player1FileName->count(),"MANUAL","MANUAL_MODE");
    player1FileName->insertItem(player1FileName->count(),"COMMAND","COMMAND_MODE");
    QDirIterator it(*(parent->AIFolder), QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString next(it.next());
        QStringList strings = next.split('/');
        if(strings.last()!="."&&strings.last()!="..")
        player1FileName->insertItem(player1FileName->count(), strings.last(),QVariant(QString(next)));
    }
}
void ControlPanel::populateComboBox2(){
    player2FileName->clear();
    player2FileName->insertItem(player2FileName->count(),"Choose...","NONE_SELECTED");
    //player2FileName->insertItem(player2FileName->count(),"MANUAL","MANUAL_MODE");
    player2FileName->insertItem(player2FileName->count(),"COMMAND","COMMAND_MODE");

    QDirIterator it(*(parent->AIFolder), QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString next(it.next());
        QStringList strings = next.split('/');
        if(strings.last()!="."&&strings.last()!="..")
        player2FileName->insertItem(player2FileName->count(), strings.last(),QVariant(QString(next)));

    }
}

void ControlPanel::player1Debug(){
    QString input = parent->player1->readAllStandardError();
    if(input!=""){
        input = "Player 1 says: " + input;
        console(input);
    }
}
void ControlPanel::player2Debug(){
    QString input = parent->player2->readAllStandardError();
    if(input!=""){
        input = "Player 2 says: " + input;
        console(input);
    }
}

void ControlPanel::resetGoButton(){
    QPalette pal = go->palette();
    pal.setColor(QPalette::ButtonText, Qt::darkGreen);
    go->setPalette(pal);
    go->setText("GO!");
    hidePauseButton();
}
void ControlPanel::setGoButton(){
    go->setText("STOP!");
    QPalette pal = go->palette();
    pal.setColor(QPalette::ButtonText, Qt::red);
    go->setPalette(pal);
    showPauseButton();
}
void ControlPanel::showPauseButton(){
    resetPauseButton();
    pause->setFixedHeight(50);
    go->setFixedHeight(50);
    pause->setVisible(true);
}
void ControlPanel::hidePauseButton(){
    pause->setVisible(false);
    go->setFixedHeight(100);
}

void ControlPanel::setPauseButton(){
    pause->setText("Resume");
    QFont pauseFont = pause->font();
    pauseFont.setPointSize(36);
    pauseFont.setBold(true);
    pause->setFont(pauseFont);
    QPalette pal = pause->palette();
    pal.setColor(QPalette::ButtonText, Qt::darkGreen);
    pause->setPalette(pal);
}
void ControlPanel::resetPauseButton(){
    pause->setText("Pause");
    QFont pauseFont = pause->font();
    pauseFont.setPointSize(36);
    pauseFont.setBold(true);
    pause->setFont(pauseFont);
    QPalette pal = pause->palette();
    pal.setColor(QPalette::ButtonText, Qt::blue);
    pause->setPalette(pal);
}

void ControlPanel::alert(QString message){
    QMessageBox myBox;
    myBox.setText(message);
    myBox.exec();
}

void ControlPanel::loadFailed(QString player)
{
    QMessageBox fail;
    QString message = "Player " + player + " failed to load!";
    console(message);
    fail.setText(message);
    fail.exec();
}

void ControlPanel::moveToStartingLocation(){
    int center_x = QDesktopWidget().availableGeometry().center().x();
    int center_y = QDesktopWidget().availableGeometry().center().y();
    int total_width = QDesktopWidget().availableGeometry().width();
    move(center_x-width()/2 - (total_width / 5),center_y-height()/2);
}

void ControlPanel::console(QString message){
    moveHistory->append(message);
}
