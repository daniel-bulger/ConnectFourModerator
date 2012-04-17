#include "Trainer.h"
#include <QThread>
#define GO_BUTTON_START_TEXT "GO!"
#define GO_BUTTON_STOP_TEXT "STOP"
#define PAUSE_BUTTON_RESUME_TEXT "Resume"
#define PAUSE_BUTTON_PAUSE_TEXT "Pause"

Trainer::Trainer(ControlPanel *parent, int type) :parent(parent),type(type),
    QWidget(0)
{
    testerModerator = new Moderator;
    trainerModerator = new Moderator;
    connect(trainerModerator,SIGNAL(gameHasEnded()),this,SLOT(gameHasEnded()));
    numberOfCores = QThread::idealThreadCount();
    if(numberOfCores==-1)
        numberOfCores=1;
    gamesRunning = false;
    gamesArePaused = false;
    settings = new QSettings("Minds and Machines", "Connect Four");
    timePerTurnSlider = new QSlider(Qt::Horizontal);
    timePerTurnSlider->setMinimum(0);
    timePerTurnSlider->setMaximum(100);
    timePerTurnSlider->setValue(1);
    timePerTurnSlider->setTickPosition(QSlider::TicksBelow);
    timePerTurnSlider->setTickInterval(10);
    timePerTurnSlider->setSingleStep(5);
    connect(timePerTurnSlider,SIGNAL(valueChanged(int)),trainerModerator,SLOT(setTimeUntilMove(int)));
    lastPlayers =QVector<QPair<int,int> >();
    player1PullIndex = 0;
    player2PullIndex = 0;
    percentStarted = new QProgressBar();
    percentStarted->setRange(0, 100);
    percentStarted->setValue(0);
    percentStarted->setTextVisible(true);
    percentFinished = new QProgressBar();
    percentFinished->setRange(0, 100);
    percentFinished->setValue(0);
    percentFinished->setTextVisible(true);
    this->goButton = new QPushButton("GO");
    goButton->setFixedHeight(100);
    QFont goFont = goButton->font();
    goFont.setPointSize(36);
    goFont.setBold(true);
    goButton->setFont(goFont);
    QPalette pal = goButton->palette();
    pal.setColor(QPalette::ButtonText, Qt::darkGreen);
    goButton->setPalette(pal);
    this->pauseButton = new QPushButton("Pause");
    pauseButton->setVisible(false);
    this->numTrialsLabel = new QLabel("Number of trials:");
    this->numTrialsInput = new QLineEdit();
    AIFolder = new QString(settings->value("AI_DIRECTORY").toString());
    chooseDirectoryButton = new QPushButton("Load from...");
    addPlayerButton = new QPushButton("Add player");
    removePlayerButton = new QPushButton("Remove player");
    playerList = new QListWidget();
    chooseDirectoryText = new QLineEdit();
    chooseDirectoryText->setText(*(AIFolder));
    playerFileNames = new QVector<QComboBox*>();
    playerFileNames->push_back(new QComboBox());
    playerFileNames->push_back(new QComboBox());
    fewestGamesPerCombinationLabel = new QLabel("Number of games to be played between each pair:");
    fewestGamesPerCombinationSpinBox = new QSpinBox();
    simpleLayout = new QVBoxLayout();
    simpleLayout->addWidget(playerFileNames->at(0));
    simpleLayout->addWidget(playerFileNames->at(1));
    simpleLayout->addWidget(chooseDirectoryButton);
    simpleLayout->addWidget(numTrialsLabel);
    simpleLayout->addWidget(numTrialsInput);
    simpleLayout->addWidget(percentStarted);
    simpleLayout->addWidget(percentFinished);
    simpleLayout->addWidget(timePerTurnSlider);
    simpleLayout->addWidget(goButton);
    simpleLayout->addWidget(pauseButton);

    tournamentLayout = new QVBoxLayout();
    tournamentLayout->addWidget(addPlayerButton);
    connect(addPlayerButton,SIGNAL(clicked()),SLOT(addPlayer()));
    tournamentLayout->addWidget(removePlayerButton);
    connect(removePlayerButton,SIGNAL(clicked()),SLOT(removePlayer()));
    tournamentLayout->addWidget(playerList);
    tournamentLayout->addWidget(numTrialsLabel);
    tournamentLayout->addWidget(percentStarted);
    tournamentLayout->addWidget(percentFinished);
    tournamentLayout->addWidget(goButton);
    tournamentLayout->addWidget(pauseButton);

    connect(goButton, SIGNAL(clicked()), this, SLOT(goButtonPressed()));
    connect(chooseDirectoryButton,SIGNAL(clicked()),this,SLOT(chooseDirectory()));
    if(type==0){
        this->setLayout(simpleLayout);
    }
    if(type==2){
        this->setLayout(tournamentLayout);
        connect(trainerModerator,SIGNAL(gamestring(QString)),this,SLOT(addGamestringToVector(QString)));
        connect(trainerModerator,SIGNAL(gameOver(int)),this,SLOT(appendToWinnersVector(int)));
    }
    this->show();
    QtConcurrent::run(this, &Trainer::populateComboBoxes);

}

Trainer::~Trainer()
{

}
void Trainer::closeEvent(QCloseEvent *event){
    this->stop();
}
void Trainer::addGamestringToVector(QString gamestring){
    gameStrings.push_back(gamestring);
}
void Trainer::appendToWinnersVector(int winner){
    Tournament::win_state winnerState;
    if(winner==-2 || winner==1)
        winner=Tournament::WIN;
    if(winner==-1 || winner==2)
        winner=Tournament::LOSS;
    if(winner==0)
        winner=Tournament::TIE;
    winnerState=(Tournament::win_state)winner;
    gameWinners.push_back(winnerState);
}

void Trainer::addPlayer(){

    QString player = QFileDialog::getOpenFileName(0,"Choose a program...",settings->value("AI_DIRECTORY").toString());

    bool found = false;
    for (int i = 0; i < playerList->count(); ++i) {
        if (playerList->item(i)->data(Qt::DisplayRole).toString() == player) {
            found = true;
            break;
        }
    }
    if (!found) {
        playerList->insertItem(playerList->count(),player);
        numTrialsInput->setText(QString().setNum((playerList->count()) * (playerList->count()-1)));
    }

}
void Trainer::removePlayer(){
    qDeleteAll(playerList->selectedItems());

}

void Trainer::stop(){
    gamesRunning = false;
    gamesRemainingToBeStarted=0;
    gamesRemainingToBeFinished=0;
    percentStarted->setValue(0);
    percentFinished->setValue(0);
    QVector<QPair<QString,QVector<QPair<QPair<QString,Tournament::win_state>,QString> > > > allData;
    QVector<QString> allPlayers;
    for( int i = 0; i<playerNamesVector.size();i++){
        if(!allPlayers.contains(playerNamesVector[i])){
            allPlayers.push_back(playerNamesVector[i]);
            allData.push_back(qMakePair(playerNamesVector[i],QVector<QPair<QPair<QString,Tournament::win_state>,QString> >() ) );
            allData.last().first=playerNamesVector[i];
        }
        int j = allPlayers.indexOf(playerNamesVector[i]);
        allData[j].second.push_back(qMakePair(qMakePair(oppNamesVector[i],gameWinners[i]),gameStrings[i]));
    }
    qDebug() << "EMIT FINISHED ";
    emit fin(20);
    emit finished(allData);

    Moderator* moderator;
    foreach(moderator,trainerModerators){
        moderator->endGame();
    }
    resetGoButton();
    QComboBox* box;
    foreach(box,*playerFileNames){
        box->setEnabled(true);
    }
}
void Trainer::pause(){
    //pause running those trials
}
void Trainer::resume(){
    //resume running those trials
}
void Trainer::start(int recursion){
    if(recursion>5){
        stop();
        return;
    }
    int index = 0;
    if(gamesRemainingToBeStarted<=0)
        return;
    QPair<QStringList,QStringList> fileNames = getFileNamesFromPlayerSelectors(type==2);
    qDebug() << fileNames;
    trainerModerator->setTimeUntilMove(timePerTurnSlider->value());
    QStringList file1MinusFirst = fileNames.first;
    file1MinusFirst.removeFirst();
    QStringList file2MinusFirst = fileNames.second;
    file2MinusFirst.removeFirst();

    if(trainerModerator->startGame(fileNames.first,fileNames.second,*AIFolder,false)){
        if(type==2){
            playerNamesVector.push_back(fileNames.first.join("/"));
            oppNamesVector.push_back(fileNames.second.join("/"));
        }
        gamesRemainingToBeStarted-=1;
        percentStarted->setValue(-1*gamesRemainingToBeStarted);
    }
    else{
        start(recursion+1);
    }
}
void Trainer::sleep(int mSecs){
    QTime dieTime = QTime::currentTime().addMSecs(mSecs);
    while( QTime::currentTime() < dieTime )
            QCoreApplication::processEvents(QEventLoop::AllEvents, mSecs);
}
void Trainer::gameHasEnded(){
    qDebug() << "A game has ended";
    gamesRemainingToBeFinished-=1;
    percentFinished->setValue(-1*gamesRemainingToBeFinished);
    if(gamesRemainingToBeStarted>=1)
        start();
    if(gamesRemainingToBeFinished==0){
        stop();
        qDebug() << "WE ALL DONE HEAH";
    }
}
void Trainer::populateComboBoxes(){
    QComboBox* fileName;
    foreach(fileName,*playerFileNames){
        fileName->clear();
        fileName->insertItem(fileName->count(),"Choose...","NONE_SELECTED");
        fileName->setCurrentIndex(0);
        fileName->insertItem(fileName->count(),"MANUAL","MANUAL_MODE");
        fileName->insertItem(fileName->count(),"COMMAND","COMMAND_MODE");

    }
    QDirIterator it(*(AIFolder), QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString next(it.next());
        QStringList strings = next.split('/');
        if(strings.last()!="."&&strings.last()!=".."){
            if((!parent->showOnlyGoodPrograms->isChecked())||(testerModerator->testProgram(next))){
                foreach(fileName,*playerFileNames){

                    fileName->insertItem(fileName->count(), strings.last(),QVariant(QString(next)));
                }
            }
        }
    }
}
void Trainer::goButtonPressed(){
    if(gamesRunning==false){
        if(type==2){
            playerNamesVector.clear();
            playerNamesVector.clear();
            oppNamesVector.clear();
            gameWinners.clear();
        }
        qDebug() << "Num trials: " << numTrialsInput->text().toInt();
        gamesRemainingToBeStarted = numTrialsInput->text().toInt();
        gamesRemainingToBeFinished = gamesRemainingToBeStarted;
        percentStarted->setRange(-1*gamesRemainingToBeStarted,0);
        percentFinished->setRange(-1*gamesRemainingToBeFinished,0);
        percentStarted->setValue(-1*gamesRemainingToBeStarted);
        percentFinished->setValue(-1*gamesRemainingToBeFinished);

        QComboBox* box;
        foreach(box,*playerFileNames){
            box->setEnabled(false);
        }
        setGoButton();
        start();
        gamesRunning = true;
    }
    else{
        stop();
        QComboBox* box;
        foreach(box,*playerFileNames){
            box->setEnabled(true);
        }
        resetGoButton();
        gamesRunning = false;
    }
}
QPair<QStringList,QStringList> Trainer::getFileNamesFromPlayerSelectors(bool isTournamentMode){
    QComboBox* playerFileName;
    QString progName;
    QStringList args;
    QPair<QStringList,QStringList> retVals;
    QPair<int,int> selectorNums = getNextPlayers(isTournamentMode);
    int index;
    int i = 0;
    while(i<2){
        if(i==0)
            index=selectorNums.first;
        else
            index=selectorNums.second;
        i++;
        QString friendlyName;
        if(isTournamentMode){
            QString playerListItem = playerList->item(index)->text();
            friendlyName = playerListItem.split("/").last();
            progName = playerListItem;
        }
        else{
            playerFileName = playerFileNames->at(index);
            friendlyName = playerFileName->itemText(playerFileName->currentIndex());
            progName = playerFileName->itemData(playerFileName->currentIndex()).toString();
        }
        if(friendlyName!="COMMAND"&&!isTournamentMode)playerFileName->setItemData(playerFileName->findText("COMMAND"),QVariant("COMMAND_MODE"));
        if(progName=="NONE_SELECTED"){
            if(i==1)
                retVals.first=QStringList();
            else
                retVals.second=QStringList();
            continue;
        }
        if(progName==""){
            if(i==1)
                retVals.first=QStringList();
            else
                retVals.second=QStringList();
            continue;        }
        if(progName=="COMMAND_MODE"&&!isTournamentMode){
            bool ok = false;
            QString text = QInputDialog::getText(this, tr("Advanced program entry"),tr("Enter a command that will run your AI."), QLineEdit::Normal,"", &ok);
            if(ok&&text!=""){
                playerFileName->setItemData(playerFileName->currentIndex(),QVariant(text));
                progName = text.split(' ')[0];
                args = text.split(' ');
                if(!args.isEmpty())
                    args.pop_front();
            }
            else{
                if(i==1)
                    retVals.first=QStringList();
                else
                    retVals.second=QStringList();
                continue;
            }
        }
        QStringList retVal = QStringList(progName);
        retVal.append(args);
        qDebug() << retVal;
        if(i==1)
            retVals.first=retVal;
        else
            retVals.second=retVal;
        continue;
    }
    return retVals;
}
QPair<int,int> Trainer::getNextPlayers(bool isTournamentMode){
    if(switchSides==false){
        if(isTournamentMode){
            if(player2PullIndex<playerList->count()-1)
                player2PullIndex++;
            else{
                if(player1PullIndex<playerList->count()-2)
                    player1PullIndex++;
                else
                    player1PullIndex=0;
                player2PullIndex=player1PullIndex+1;
            }
        }
        else{
            if(player2PullIndex<playerFileNames->size()-1)
                player2PullIndex++;
            else{
                if(player1PullIndex<playerFileNames->size()-2)
                    player1PullIndex++;
                else
                    player1PullIndex=0;
                player2PullIndex=player1PullIndex+1;
            }
        }
        switchSides = true;
        return qMakePair(player1PullIndex,player2PullIndex);
    }
    else{
        switchSides = false;
        return qMakePair(player2PullIndex,player1PullIndex);
    }
}

void Trainer::resetGoButton(){
    QPalette pal = goButton->palette();
    pal.setColor(QPalette::ButtonText, Qt::darkGreen);
    goButton->setPalette(pal);
    goButton->setText(GO_BUTTON_START_TEXT);
    hidePauseButton();
}
void Trainer::setGoButton(){
    goButton->setText(GO_BUTTON_STOP_TEXT);
    QPalette pal = goButton->palette();
    pal.setColor(QPalette::ButtonText, Qt::red);
    goButton->setPalette(pal);
    showPauseButton();
}
void Trainer::showPauseButton(){
    resetPauseButton();
    pauseButton->setFixedHeight(50);
    goButton->setFixedHeight(50);
    pauseButton->setVisible(true);
}
void Trainer::hidePauseButton(){
    pauseButton->setVisible(false);
    goButton->setFixedHeight(100);
}
void Trainer::setPauseButton(){
    pauseButton->setText(PAUSE_BUTTON_RESUME_TEXT);
    QFont pauseFont = pauseButton->font();
    pauseFont.setPointSize(36);
    pauseFont.setBold(true);
    pauseButton->setFont(pauseFont);
    QPalette pal = pauseButton->palette();
    pal.setColor(QPalette::ButtonText, Qt::darkGreen);
    pauseButton->setPalette(pal);
}
void Trainer::resetPauseButton(){
    pauseButton->setText(PAUSE_BUTTON_PAUSE_TEXT);
    QFont pauseFont = pauseButton->font();
    pauseFont.setPointSize(36);
    pauseFont.setBold(true);
    pauseButton->setFont(pauseFont);
    QPalette pal = pauseButton->palette();
    pal.setColor(QPalette::ButtonText, Qt::blue);
    pauseButton->setPalette(pal);
}
void Trainer::chooseDirectory(){
    QString folder = QFileDialog::getExistingDirectory(0,"Choose the folder where your AIs are located",settings->value("AI_DIRECTORY").toString());
    if(folder!=""){
    chooseDirectoryText->setText(folder);
    AIFolder = new QString(folder);
    settings->setValue("AI_DIRECTORY",folder);
    populateComboBoxes();
    }
}
