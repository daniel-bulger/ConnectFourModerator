#include "ControlPanel.h"
#define GO_BUTTON_START_TEXT "GO!"
#define GO_BUTTON_STOP_TEXT "STOP"
#define PAUSE_BUTTON_RESUME_TEXT "Resume"
#define PAUSE_BUTTON_PAUSE_TEXT "Pause"
ControlPanel::ControlPanel()
    : QWidget(0)
{
    settings = new QSettings("Minds and Machines", "Connect Four");
    parent = new Moderator();
    gameBoard = new Board(this);
    //init menu bar
    menuBar = new QMenuBar(this);
    fileMenu = menuBar->addMenu("File");
    fileMenu->addAction("Change AI dir",this,SLOT(chooseDirectory()));
    fileMenu->addAction("New Tournament",this,SLOT(initTournament()));
    fileMenu->addSeparator();
    fileMenu->addAction("Exit",this,SLOT(close()));
    preferencesMenu = menuBar->addMenu("Preferences");
    boardBackgroundPreference = preferencesMenu->addAction("Board background");
    boardBackgroundPreference->setCheckable(true);
    boardRed = settings->value("boardred").toInt();
    boardGreen = settings->value("boardgreen").toInt();
    boardBlue = settings->value("boardblue").toInt();
    connect(boardBackgroundPreference,SIGNAL(toggled(bool)),this,SLOT(boardBackChanged(bool)));
    boardBackgroundPreference->setChecked(settings->value("boardback").toBool());
    boardBackgroundColor = preferencesMenu->addAction(("Background color..."));
    connect(boardBackgroundColor,SIGNAL(triggered()),this,SLOT(chooseBoardColors()));
    boardSizeSubmenu = preferencesMenu->addMenu("Board size");
    boardSmall = boardSizeSubmenu->addAction("Small");
    boardSmall->setCheckable(true);
    boardSmall->setChecked(settings->value("boardsize").toInt()==1);
    boardMedium = boardSizeSubmenu->addAction("Medium");
    boardMedium->setCheckable(true);
    boardMedium->setChecked(settings->value("boardsize").toInt()==2);
    boardLarge = boardSizeSubmenu->addAction("Large");
    boardLarge->setCheckable(true);
    boardLarge->setChecked(settings->value("boardsize").toInt()==3);
    boardAuto = boardSizeSubmenu->addAction("Auto");
    boardAuto->setCheckable(true);
    boardAuto->setChecked(settings->value("boardsize").toInt()==0);

    connect(boardAuto,SIGNAL(toggled(bool)),this,SLOT(boardAutoChanged(bool)));
    connect(boardSmall,SIGNAL(toggled(bool)),this,SLOT(boardSmallChanged(bool)));
    connect(boardMedium,SIGNAL(toggled(bool)),this,SLOT(boardMediumChanged(bool)));
    connect(boardLarge,SIGNAL(toggled(bool)),this,SLOT(boardLargeChanged(bool)));


    boardSizeChoices = new QActionGroup(this);
    boardSizeChoices->addAction(boardSmall);
    boardSizeChoices->addAction(boardMedium);
    boardSizeChoices->addAction(boardLarge);
    boardSizeChoices->addAction(boardAuto);

    boardLockedPreference = preferencesMenu->addAction("Lock board in place");
    boardLockedPreference->setCheckable(true);
    boardLockedPreference->setChecked(settings->value("boardlock").toBool());

    doubleClickToPlacePiecePreference = preferencesMenu->addAction("Double click to place piece");
    doubleClickToPlacePiecePreference->setCheckable(true);
    doubleClickToPlacePiecePreference->setChecked(settings->value("dblclicktoplace").toBool());

    preferencesMenu->addSeparator();

    timeLimitPreference = preferencesMenu->addAction("Enforce time limit");
    timeLimitPreference->setCheckable(true);
    timeLimitPreference->setChecked(settings->value("timelimit").toBool());

    showOnlyGoodPrograms = preferencesMenu->addAction("Only display usable programs");
    showOnlyGoodPrograms->setCheckable(true);
    showOnlyGoodPrograms->setChecked(settings->value("showgood").toBool());
    connect(showOnlyGoodPrograms,SIGNAL(triggered()),this,SLOT(populateComboBoxes()));
    trainerMenu = menuBar->addMenu("Training");
    trainerMenu->addAction("Run trials with current AIs",this,SLOT(runMassTrials()));
    trainerMenu->addAction("Advanced training...",this,SLOT(chooseAIsToTrain()));
    helpMenu = menuBar->addMenu("Help");
    helpMenu->addAction("README",this,SLOT(displayReadme()));
    helpMenu->addSeparator();
    helpMenu->addAction("About this program...",this,SLOT(displayAbout()));

    //initialize move history test area.
    layout = new QVBoxLayout();
    moveHistory = new QTextEdit("Welcome to Connect Four!  There will be instructions here at some point.");
    moveHistory->setReadOnly(true);
    //Player labels.
    player1Label = new QLabel("Player 1:");
    player2Label = new QLabel("Player 2:");
    //choose your directory
    AIFolder = new QString(settings->value("AI_DIRECTORY").toString());
    chooseDirectoryButton = new QPushButton("Load from...");
    chooseDirectoryText = new QLineEdit();
    chooseDirectoryText->setText(*(AIFolder));
    //initializing players' AI file name combo boxes.
    player1FileName = new QComboBox();
    player1FileName->setMinimumWidth(100);
    player2FileName = new QComboBox();
    player2FileName->setMinimumWidth(100);
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
    chooseDirectorySection = new QHBoxLayout();
    chooseDirectorySection->addWidget(chooseDirectoryButton);
    chooseDirectorySection->addWidget(chooseDirectoryText);
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
    layout = new QVBoxLayout(this);
    layout->setMenuBar(menuBar);
    layout->addWidget(moveHistory);
    layout->addLayout(chooseDirectorySection);
    layout->addLayout(player1Section);
    layout->addLayout(player2Section);
    layout->addLayout(delaySection);
    layout->addWidget(pause);
    layout->addWidget(go);
    setLayout(layout);
    moveToStartingLocation();

    connect(go, SIGNAL(clicked()), this, SLOT(goButtonPressed()),Qt::QueuedConnection);
    connect(pause, SIGNAL(clicked()), this, SLOT(pauseButtonPressed()),Qt::QueuedConnection);
    connect(parent,SIGNAL(gameOver(int)),this,SLOT(displayWinner(int)),Qt::QueuedConnection);
    connect(chooseDirectoryText,SIGNAL(textEdited()),this,SLOT(directoryTextBoxEdited()),Qt::QueuedConnection);
    connect(chooseDirectoryButton,SIGNAL(clicked()),this,SLOT(chooseDirectory()),Qt::QueuedConnection);
    connect(parent,SIGNAL(timeUntilMoveChanged(int,bool)),this,SLOT(updateTimeRemainingSlider(int,bool)));
    connect(parent->timer,SIGNAL(timeout()),this,SLOT(updateCurrentPlayerTimeRemainingSlider()));
    connect(parent,SIGNAL(consoleOutput(QString)),this,SLOT(console(QString)));
    connect(parent,SIGNAL(piecePlaced(int,int)),this,SLOT(placePieceOnBoard(int,int)));
    connect(parent,SIGNAL(player1ToMove(bool)),this,SLOT(playerToMoveBoardUpdater(bool)));
    connect(parent,SIGNAL(player2ToMove(bool)),this,SLOT(playerToMoveBoardUpdater(bool)));
    connect(parent,SIGNAL(loadFailed(QString)),this,SLOT(loadFailed(QString)));
    connect(parent,SIGNAL(gameHasEnded()),this,SLOT(gameHasEnded()));
    connect(parent,SIGNAL(loadSuccess(QStringList)),this,SLOT(showSuccessfulLoad(QStringList)));
    connect(this->timePerTurnSlider,SIGNAL(valueChanged(int)),parent,SLOT(setTimeUntilMove(int)));
    connect(parent,SIGNAL(acceptManualInput()),this,SLOT(connectManualInputToModerator()));
    parent->setTimeUntilMove(timePerTurnSlider->value());
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)),
      this, SLOT(onApplicationFocusChanged(QWidget*,QWidget*)));

    this->show();
    QtConcurrent::run(this, &ControlPanel::populateComboBoxes);
}

ControlPanel::~ControlPanel()
{

}
// Application will exit when this pane is closed.  Any handling before exit should go here.
void ControlPanel::closeEvent(QCloseEvent *event){
    settings->setValue("showgood",showOnlyGoodPrograms->isChecked());
    settings->setValue("boardlock",boardLockedPreference->isChecked());
    settings->setValue("boardback",boardBackgroundPreference->isChecked());
    settings->setValue("boardred",boardRed);
    settings->setValue("boardblue",boardBlue);
    settings->setValue("boardgreen",boardGreen);
    settings->setValue("dblclicktoplace",doubleClickToPlacePiecePreference->isChecked());
    settings->setValue("timelimit",timeLimitPreference->isChecked());
    if(boardSmall->isChecked()) settings->setValue("boardsize",1);
    if(boardMedium->isChecked()) settings->setValue("boardsize",2);
    if(boardLarge->isChecked()) settings->setValue("boardsize",3);
    if(boardAuto->isChecked()) settings->setValue("boardsize",0);
    QApplication::quit();
}
void ControlPanel::showEvent(QShowEvent*){
    if((gameBoard)!=NULL){
        gameBoard->setWindowState(Qt::WindowActive);
    }
}
void ControlPanel::focusInEvent(QFocusEvent *){
    qDebug("Focus in");
    if((gameBoard)!=NULL){
        gameBoard->setWindowState(Qt::WindowActive);
    }
}
void ControlPanel::focusOutEvent(QFocusEvent *){
    qDebug("Focus out");
    if((gameBoard)!=NULL){
        gameBoard->setWindowState(Qt::WindowMinimized);
    }
}
void ControlPanel::mousePressEvent(QMouseEvent *){
    qDebug("Click");
    if((gameBoard)!=NULL){
        gameBoard->setWindowState(Qt::WindowActive);
    }
}

void ControlPanel::onApplicationFocusChanged(QWidget* from, QWidget* to){
    if (from == 0 && isAncestorOf(to) == true){
        if((gameBoard)!=NULL){
            gameBoard->setWindowState(Qt::WindowActive);
        }
    }
    else if (isAncestorOf(from) == true && to == 0){
        if((gameBoard)!=NULL){
            gameBoard->setWindowState(Qt::WindowMinimized);
        }
    }
}

void ControlPanel::chooseBoardColors(){
    QColor col;
    col.setRed(boardRed);
    col.setBlue(boardBlue);
    col.setGreen(boardGreen);
    QColor newCol = QColorDialog::getColor(col);
    if(newCol.isValid()){
        boardRed = newCol.red();
        boardBlue = newCol.blue();
        boardGreen = newCol.green();
        if(this->boardBackgroundPreference->isChecked())
            gameBoard->setBackgroundBrush(newCol);
    }
}

void ControlPanel::hideEvent(QHideEvent *){
    if((gameBoard)!=NULL){
        qDebug("HIDING CONTROLPANEL");
        gameBoard->setWindowState(Qt::WindowMinimized);
    }
}

void ControlPanel::populateComboBoxes(){
    player1FileName->clear();
    player2FileName->clear();

    player1FileName->insertItem(player1FileName->count(),"Choose...","NONE_SELECTED");
    player2FileName->insertItem(player2FileName->count(),"Choose...","NONE_SELECTED");
    player1FileName->setCurrentIndex(0);
    player2FileName->setCurrentIndex(0);
    player1FileName->insertItem(player1FileName->count(),"MANUAL","MANUAL_MODE");
    player2FileName->insertItem(player2FileName->count(),"MANUAL","MANUAL_MODE");
    player1FileName->insertItem(player1FileName->count(),"COMMAND","COMMAND_MODE");
    player2FileName->insertItem(player2FileName->count(),"COMMAND","COMMAND_MODE");
    QDirIterator it(*(AIFolder), QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString next(it.next());
        QStringList strings = next.split('/');
        if(strings.last()!="."&&strings.last()!=".."){
            if((!this->showOnlyGoodPrograms->isChecked())||(parent->testProgram(next))){
            player1FileName->insertItem(player1FileName->count(), strings.last(),QVariant(QString(next)));
            player2FileName->insertItem(player2FileName->count(), strings.last(),QVariant(QString(next)));
        }
        }
    }
}

void ControlPanel::populateComboBox(bool isPlayer1){
    QComboBox* playerFileName;
    if(isPlayer1){
        playerFileName = player1FileName;
    }
    else{
        playerFileName = player2FileName;
    }
    playerFileName->clear();
    playerFileName->insertItem(playerFileName->count(),"Choose...","NONE_SELECTED");
    playerFileName->insertItem(playerFileName->count(),"MANUAL","MANUAL_MODE");
    playerFileName->insertItem(playerFileName->count(),"COMMAND","COMMAND_MODE");
    QDirIterator it(*(AIFolder), QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString next(it.next());
        QStringList strings = next.split('/');
        if(strings.last()!="."&&strings.last()!=".."){
            if((!this->showOnlyGoodPrograms->isChecked())||(parent->testProgram(next)))
            playerFileName->insertItem(playerFileName->count(), strings.last(),QVariant(QString(next)));
        }
    }
}
void ControlPanel::goButtonPressed(){
    go->setEnabled(false);
    if(parent->gamestate==GAME_STOPPED){
        if(parent->startGame(getFileNameFromPlayerSelector(true),getFileNameFromPlayerSelector(false),*AIFolder)){
            setGoButton();
            player1TimeRemainingBar->setValue(MOVE_TIME_LIMIT*1000);
            player2TimeRemainingBar->setValue(MOVE_TIME_LIMIT*1000);
            gameBoard->clearPieces();
            player1FileName->setEnabled(false);
            player2FileName->setEnabled(false);
        }
        else{
        }
    }
    else{
        gameBoard->gameResult(3);
        parent->endGame();
        timePerTurnTimer->stop();
        timer->stop();
        player1FileName->setEnabled(true);
        player2FileName->setEnabled(true);
        player1TimeRemainingBar->setValue(MOVE_TIME_LIMIT*1000);
        player2TimeRemainingBar->setValue(MOVE_TIME_LIMIT*1000);
        resetGoButton();
    }
    go->setEnabled(true);

}
void ControlPanel::pauseButtonPressed(){
    if(pause->text()==PAUSE_BUTTON_PAUSE_TEXT){
        qDebug() << "Pause button pressed" << endl;
        this->setPauseButton();
        parent->pauseGame();
    }
    else{
        if(pause->text()==PAUSE_BUTTON_RESUME_TEXT){
            qDebug() << "Resume button pressed" << endl;
            this->resetPauseButton();
            parent->resumeGame();
        }
    }
}
void ControlPanel::gameHasEnded(){
    timePerTurnTimer->stop();
    timer->stop();
    player1FileName->setEnabled(true);
    player2FileName->setEnabled(true);
    player1TimeRemainingBar->setValue(MOVE_TIME_LIMIT*1000);
    player2TimeRemainingBar->setValue(MOVE_TIME_LIMIT*1000);
    resetGoButton();
}

void ControlPanel::resetGoButton(){
    QPalette pal = go->palette();
    pal.setColor(QPalette::ButtonText, Qt::darkGreen);
    go->setPalette(pal);
    go->setText(GO_BUTTON_START_TEXT);
    hidePauseButton();
}
void ControlPanel::setGoButton(){
    go->setText(GO_BUTTON_STOP_TEXT);
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
    pause->setText(PAUSE_BUTTON_RESUME_TEXT);
    QFont pauseFont = pause->font();
    pauseFont.setPointSize(36);
    pauseFont.setBold(true);
    pause->setFont(pauseFont);
    QPalette pal = pause->palette();
    pal.setColor(QPalette::ButtonText, Qt::darkGreen);
    pause->setPalette(pal);
}
void ControlPanel::resetPauseButton(){
    pause->setText(PAUSE_BUTTON_PAUSE_TEXT);
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
void ControlPanel::boardBackChanged(bool isChecked){
    qDebug() << "back changed";
    if(isChecked){
        gameBoard->setBackgroundBrush(QBrush(QColor(boardRed,boardBlue,boardGreen),Qt::SolidPattern));
    }
    else{
        gameBoard->setBackgroundBrush(Qt::NoBrush);
    }
}
void ControlPanel::runMassTrials(){
    trainers.push_back(new Trainer(this,0));

}
void ControlPanel::chooseAIsToTrain(){
    trainers.push_back(new Trainer(this,2));
}

void ControlPanel::boardAutoChanged(bool isChecked,bool recur){

    if(isChecked){
        int width = QApplication::desktop()->width();
        int height = QApplication::desktop()->height();
        if((width * 1574 / 1260) > (height)){
            gameBoard->resizeBoard(width*2/3);
        }
        else{
            gameBoard->resizeBoard(height*2/3*1574/1260);
        }
    }
    if(recur) boardAutoChanged(isChecked,false);
}
void ControlPanel::boardSmallChanged(bool isChecked,bool recur){

    if(isChecked){
        gameBoard->resizeBoard(500);
    }
    if(recur) boardSmallChanged(isChecked,false);
}
void ControlPanel::boardMediumChanged(bool isChecked,bool recur){

    if(isChecked){
        gameBoard->resizeBoard(1000);
    }
    if(recur) boardMediumChanged(isChecked,false);

}
void ControlPanel::boardLargeChanged(bool isChecked,bool recur){

    if(isChecked){
        gameBoard->resizeBoard(1500);
    }
    if(recur) boardLargeChanged(isChecked,false);

}

void ControlPanel::displayReadme(){
    QString readmePath = ":/helpDocs/README.html";
    QFile file(readmePath);
    file.open(QIODevice::ReadOnly);
    QString data = file.readAll();
    file.close();
    showHTML(data);
}
void ControlPanel::displayAbout(){
    QString aboutPath = ":/helpDocs/about.html";
    QFile file(aboutPath);
    file.open(QIODevice::ReadOnly);
    QString data = file.readAll();
    file.close();
    showHTML(data);
}

void ControlPanel::showHTML(QString data){
    QWidget* newWindow = new QWidget();
    QVBoxLayout* newLayout = new QVBoxLayout();
    QTextBrowser* textB = new QTextBrowser();
    textB->setReadOnly(true);
    textB->setHtml(data);
    newLayout->addWidget(textB);
    newWindow->setLayout(newLayout);
    newWindow->show();
}

void ControlPanel::moveToStartingLocation(){
    int center_x = QDesktopWidget().availableGeometry().center().x();
    int center_y = QDesktopWidget().availableGeometry().center().y();
    int total_width = QDesktopWidget().availableGeometry().width();
    move(center_x-width()/2 - (total_width / 5),center_y-height()/2);
}

void ControlPanel::console(QString message){
    moveHistory->append(message);
    QScrollBar *sb = moveHistory->verticalScrollBar();
    sb->setValue(sb->maximum());
}
void ControlPanel::updateTimeRemainingSlider(int timeRemaining, bool isPlayer1){
    if(isPlayer1)
        this->player1TimeRemainingBar->setValue(timeRemaining);
    else
        this->player2TimeRemainingBar->setValue(timeRemaining);
}
void ControlPanel::updateCurrentPlayerTimeRemainingSlider(){
    int currentPlayer = parent->getCurrentPlayer();
    if(currentPlayer==1){
        this->player1TimeRemainingBar->setValue(player1TimeRemainingBar->value()-10);
        if((this->player1TimeRemainingBar->value()<=0)&&(timeLimitPreference->isChecked())){
            console("Player 1 did not make a move within 10 seconds, so Player 2 wins!");
            parent->player2Wins(true);
        }
    }
    if(currentPlayer==2){
        this->player2TimeRemainingBar->setValue(player2TimeRemainingBar->value()-10);
        if((this->player2TimeRemainingBar->value()<=0)&&(timeLimitPreference->isChecked())){
            console("Player 2 did not make a move within 10 seconds, so Player 1 wins!");
            parent->player1Wins(true);
        }

    }
}
void ControlPanel::displayWinner(int winner){
    gameBoard->gameResult(winner);
}
void ControlPanel::placePieceOnBoard(int x, int y){
    gameBoard->place(x,y);
    qDebug() << "Placed piece at " << x <<", " << y << endl;
}
void ControlPanel::playerToMoveBoardUpdater(bool isManual){
    if(!isManual)
        gameBoard->highlightGraphic->hide();
    else
        gameBoard->mouseMoveEvent();
}
void ControlPanel::chooseDirectory(){
    QString folder = QFileDialog::getExistingDirectory(0,"Choose the folder where your AIs are located",settings->value("AI_DIRECTORY").toString());
    if(folder!=""){
    chooseDirectoryText->setText(folder);
    AIFolder = new QString(folder);
    settings->setValue("AI_DIRECTORY",folder);
    populateComboBoxes();
    }
}
void ControlPanel::loadFailed(QString player)
{
    QMessageBox fail;
    QString message;
    if(player == "")
        message = "Please select an AI from the list.";
    else
        message = "Player " + player + " failed to load!";
    console(message);
    fail.setText(message);
    fail.exec();
}
QStringList ControlPanel::getFileNameFromPlayerSelector(bool isPlayer1){
    QComboBox* playerFileName;
    QString progName;
    QStringList args;
    if(isPlayer1)
        playerFileName = player1FileName;
    else
        playerFileName = player2FileName;

    QString friendlyName = playerFileName->itemText(playerFileName->currentIndex());
    progName = playerFileName->itemData(playerFileName->currentIndex()).toString();
    if(friendlyName!="COMMAND")playerFileName->setItemData(playerFileName->findText("COMMAND"),QVariant("COMMAND_MODE"));
    if(progName=="NONE_SELECTED") return QStringList();
    if(progName=="") return QStringList();
    if(progName=="COMMAND_MODE"){
        bool ok = false;
        QString text = QInputDialog::getText(this, tr("Advanced program entry"),tr("Enter a command that will run your AI."), QLineEdit::Normal,"", &ok);
        if(ok&&text!=""){
            playerFileName->setItemData(playerFileName->currentIndex(),QVariant(text));
            progName = text.split(' ')[0];
            args = text.split(' ');
            if(!args.isEmpty())
                args.pop_front();
        }
        else return QStringList();
    }
    QStringList retVal = QStringList(progName);
    retVal.append(args);
    return retVal;
}
void ControlPanel::showSuccessfulLoad(QStringList fullProgName){
    QString friendlyName = fullProgName.last().split("/").last();
    console("Player "+friendlyName+" ready");
}
void ControlPanel::connectManualInputToModerator(){
    connect(gameBoard,SIGNAL(pieceDroppedByPlayer(int)),this->parent,SLOT(playerDroppedPiece(int)));
}

void ControlPanel::initTournament()
{
    qDebug() << "TTTTTTT";
    Trainer* trainer = new Trainer(this, 2);
    connect(trainer, SIGNAL(finished(Tournament::players_results_t)),this, SLOT(startTournament(Tournament::players_results_t)));
}

void ControlPanel::startTournament(Tournament::players_results_t results)
{
    qDebug() << "start tournament";
    this->tournament = new Tournament(results);
    tournament->printTree();
}
