#include "ControlPanel.h"

ControlPanel::ControlPanel(Moderator *theParent)
    : QWidget(0)
{
    parent = theParent;
    //init menu bar
    menuBar = new QMenuBar(this);
    fileMenu = menuBar->addMenu("File");
    fileMenu->addAction("Change AI dir",parent,SLOT(chooseDirectory()));
    fileMenu->addSeparator();
    fileMenu->addAction("Exit",this,SLOT(close()));
    preferencesMenu = menuBar->addMenu("Preferences");
    boardBackgroundPreference = preferencesMenu->addAction("Board background");
    boardBackgroundPreference->setCheckable(true);
    connect(boardBackgroundPreference,SIGNAL(toggled(bool)),this,SLOT(boardBackChanged(bool)));
    boardBackgroundPreference->setChecked(parent->settings->value("boardback").toBool());

    boardSizeSubmenu = preferencesMenu->addMenu("Board size");
    boardSmall = boardSizeSubmenu->addAction("Small");
    boardSmall->setCheckable(true);
    boardSmall->setChecked(parent->settings->value("boardsize").toInt()==1);
    boardMedium = boardSizeSubmenu->addAction("Medium");
    boardMedium->setCheckable(true);
    boardMedium->setChecked(parent->settings->value("boardsize").toInt()==2);
    boardLarge = boardSizeSubmenu->addAction("Large");
    boardLarge->setCheckable(true);
    boardLarge->setChecked(parent->settings->value("boardsize").toInt()==3);
    boardAuto = boardSizeSubmenu->addAction("Auto");
    boardAuto->setCheckable(true);
    boardAuto->setChecked(parent->settings->value("boardsize").toInt()==0);

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
    boardLockedPreference->setChecked(parent->settings->value("boardlock").toBool());
    doubleClickToPlacePiecePreference = preferencesMenu->addAction("Double click to place piece");
    doubleClickToPlacePiecePreference->setCheckable(true);
    doubleClickToPlacePiecePreference->setChecked(parent->settings->value("dblclicktoplace").toBool());
    preferencesMenu->addSeparator();
    showOnlyGoodPrograms = preferencesMenu->addAction("Only display usable programs");
    showOnlyGoodPrograms->setCheckable(true);
    showOnlyGoodPrograms->setChecked(parent->settings->value("showgood").toBool());

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
    chooseDirectoryButton = new QPushButton("Load from...");
    chooseDirectoryText = new QLineEdit();
    chooseDirectoryText->setText(*(parent->AIFolder));
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
    this->show();
    populateComboBoxes();

}

ControlPanel::~ControlPanel()
{

}
// Application will exit when this pane is closed.  Any handling before exit should go here.
void ControlPanel::closeEvent(QCloseEvent *event){
    parent->settings->setValue("showgood",showOnlyGoodPrograms->isChecked());
    parent->settings->setValue("boardlock",boardLockedPreference->isChecked());
    parent->settings->setValue("boardback",boardBackgroundPreference->isChecked());
    parent->settings->setValue("dblclicktoplace",doubleClickToPlacePiecePreference->isChecked());
    if(boardSmall->isChecked()) parent->settings->setValue("boardsize",1);
    if(boardMedium->isChecked()) parent->settings->setValue("boardsize",2);
    if(boardLarge->isChecked()) parent->settings->setValue("boardsize",3);
    if(boardAuto->isChecked()) parent->settings->setValue("boardsize",0);



    QApplication::quit();
}
void ControlPanel::showEvent(QShowEvent*){
    qDebug("SHOWING CONTROLPANEL");
    if((parent->gameBoard)!=NULL){
        parent->gameBoard->activateWindow();
    }
}

void ControlPanel::populateComboBoxes(){
    player1FileName->clear();
    player2FileName->clear();

    player1FileName->insertItem(player1FileName->count(),"Choose...","NONE_SELECTED");
    player2FileName->insertItem(player2FileName->count(),"Choose...","NONE_SELECTED");
    player1FileName->insertItem(player1FileName->count(),"MANUAL","MANUAL_MODE");
    player2FileName->insertItem(player2FileName->count(),"MANUAL","MANUAL_MODE");
    player1FileName->insertItem(player1FileName->count(),"COMMAND","COMMAND_MODE");
    player2FileName->insertItem(player2FileName->count(),"COMMAND","COMMAND_MODE");
    QDirIterator it(*(parent->AIFolder), QDirIterator::Subdirectories);
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
    QDirIterator it(*(parent->AIFolder), QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString next(it.next());
        QStringList strings = next.split('/');
        if(strings.last()!="."&&strings.last()!=".."){
            if((!this->showOnlyGoodPrograms->isChecked())||(parent->testProgram(next)))
            playerFileName->insertItem(playerFileName->count(), strings.last(),QVariant(QString(next)));
        }
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
void ControlPanel::boardBackChanged(bool isChecked){
    qDebug() << "back changed";
    if(isChecked){
        parent->gameBoard->setBackgroundBrush(Qt::SolidPattern);
    }
    else{
        parent->gameBoard->setBackgroundBrush(Qt::NoBrush);
    }
}

void ControlPanel::boardAutoChanged(bool isChecked,bool recur){

    if(isChecked){
        int width = QApplication::desktop()->width();
        int height = QApplication::desktop()->height();
        if((width * 1574 / 1260) > (height)){
            parent->gameBoard->resizeBoard(width*2/3);
        }
        else{
            parent->gameBoard->resizeBoard(height*2/3*1574/1260);
        }
    }
    if(recur) boardAutoChanged(isChecked,false);
}
void ControlPanel::boardSmallChanged(bool isChecked,bool recur){

    if(isChecked){
        parent->gameBoard->resizeBoard(500);
    }
    if(recur) boardSmallChanged(isChecked,false);
}
void ControlPanel::boardMediumChanged(bool isChecked,bool recur){

    if(isChecked){
        parent->gameBoard->resizeBoard(1000);
    }
    if(recur) boardMediumChanged(isChecked,false);

}
void ControlPanel::boardLargeChanged(bool isChecked,bool recur){

    if(isChecked){
        parent->gameBoard->resizeBoard(1500);
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
    QScrollBar *sb = moveHistory->verticalScrollBar();
    sb->setValue(sb->maximum());
}

