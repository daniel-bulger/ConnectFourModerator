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
    playerListLabel = new QLabel("Player paths:");
    playerList = new QListWidget();
    winsLabel = new QLabel("W");
    wins = new QListWidget();
    wins->setFixedSize(50,200);
    lossesLabel = new QLabel("L");
    losses = new QListWidget();
    losses->setFixedSize(50,200);
    tiesLabel = new QLabel("T");
    ties = new QListWidget();
    ties->setFixedSize(50,200);
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
    //simpleLayout->addWidget(percentStarted);
    simpleLayout->addWidget(percentFinished);
    simpleLayout->addWidget(timePerTurnSlider);
    simpleLayout->addWidget(goButton);
    simpleLayout->addWidget(pauseButton);

    tournamentLayout = new QVBoxLayout();
    tournamentLayout->addWidget(addPlayerButton);
    connect(addPlayerButton,SIGNAL(clicked()),SLOT(addPlayer()));
    tournamentLayout->addWidget(removePlayerButton);
    connect(removePlayerButton,SIGNAL(clicked()),SLOT(removePlayer()));
    playerStatsLayout = new QHBoxLayout();
    playerListLayout = new QVBoxLayout();
    winsLayout = new QVBoxLayout();
    lossesLayout = new QVBoxLayout();
    tiesLayout = new QVBoxLayout();
    playerListLayout->addWidget(playerListLabel);
    playerListLayout->addWidget(playerList);
    playerStatsLayout->addLayout(playerListLayout);
    winsLayout->addWidget(winsLabel);
    winsLayout->addWidget(wins);
    playerStatsLayout->addLayout(winsLayout);
    lossesLayout->addWidget(lossesLabel);
    lossesLayout->addWidget(losses);
    playerStatsLayout->addLayout(lossesLayout);
    tiesLayout->addWidget(tiesLabel);
    tiesLayout->addWidget(ties);
    playerStatsLayout->addLayout(tiesLayout);
    tournamentLayout->addLayout(playerStatsLayout);
    tournamentLayout->addWidget(numTrialsLabel);
    tournamentLayout->addWidget(percentFinished);
    tournamentLayout->addWidget(goButton);
    tournamentLayout->addWidget(pauseButton);

    connect(goButton, SIGNAL(clicked()), this, SLOT(goButtonPressed()));
    connect(pauseButton, SIGNAL(clicked()),this,SLOT(pauseButtonPressed()));
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
    gamesRemainingToBeFinished=1;
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
    updateScores(winner);
    winnerState=(Tournament::win_state)winner;
    gameWinners.push_back(winnerState);
}
void Trainer::updateScores(int winner){
    int firstPlayerIndex;
    int secondPlayerIndex;
    if(switchSides){   // if this is set, then the previous players were not switched
        firstPlayerIndex = player1PullIndex;
        secondPlayerIndex = player2PullIndex;
    }
    else{
        firstPlayerIndex = player2PullIndex;
        secondPlayerIndex = player1PullIndex;
    }
    int player1Wins = wins->item(firstPlayerIndex)->data(Qt::DisplayRole).toInt();
    int player1Losses = losses->item(firstPlayerIndex)->data(Qt::DisplayRole).toInt();
    int player1Ties = ties->item(firstPlayerIndex)->data(Qt::DisplayRole).toInt();
    int player2Wins = wins->item(secondPlayerIndex)->data(Qt::DisplayRole).toInt();
    int player2Losses = losses->item(secondPlayerIndex)->data(Qt::DisplayRole).toInt();
    int player2Ties = ties->item(secondPlayerIndex)->data(Qt::DisplayRole).toInt();

    if(winner==Tournament::WIN){
        wins->item(firstPlayerIndex)->setData(Qt::DisplayRole,QVariant(player1Wins+1));
        losses->item(secondPlayerIndex)->setData(Qt::DisplayRole,QVariant(player2Losses+1));
    }
    if(winner==Tournament::LOSS){
        wins->item(secondPlayerIndex)->setData(Qt::DisplayRole,QVariant(player2Wins+1));
        losses->item(firstPlayerIndex)->setData(Qt::DisplayRole,QVariant(player1Losses+1));
    }
    if(winner==Tournament::TIE){
        ties->item(firstPlayerIndex)->setData(Qt::DisplayRole,QVariant(player1Ties+1));
        ties->item(secondPlayerIndex)->setData(Qt::DisplayRole,QVariant(player2Ties+1));
    }
}

void Trainer::addPlayer(){

    QStringList players = QFileDialog::getOpenFileNames(0,"Choose a program...",settings->value("AI_DIRECTORY").toString());

    bool found = false;
    for(int playerNum = 0; playerNum<players.size();playerNum++){
        for (int i = 0; i < playerList->count(); ++i) {
            if (playerList->item(i)->data(Qt::UserRole).toString() == players[playerNum]) {
                found = true;
                break;
            }
        }
        if (!found) {
            playerList->insertItem(playerList->count(),players[playerNum]);
            playerList->item(playerList->count()-1)->setData(Qt::UserRole,players[playerNum]);
            playerList->item(playerList->count()-1)->setData(Qt::DisplayRole,players[playerNum].split("\\").last());
            wins->insertItem(wins->count(),"0");
            losses->insertItem(losses->count(),"0");
            ties->insertItem(ties->count(),"0");
            numTrialsLabel->setText("Number of trials: "+QString().setNum((playerList->count()) * (playerList->count()-1)));
            numTrialsInput->setText(QString().setNum((playerList->count()) * (playerList->count()-1)));

        }
    }

}
void Trainer::removePlayer(){
    QModelIndex modIndex;
    int index;
    foreach(modIndex, playerList->selectionModel()->selectedIndexes()){
        index = modIndex.row();
        playerList->model()->removeRow(index);
        wins->model()->removeRow(index);
        losses->model()->removeRow(index);
        ties->model()->removeRow(index);
    }
    numTrialsLabel->setText("Number of trials: "+QString().setNum((playerList->count()) * (playerList->count()-1)));
    numTrialsInput->setText(QString().setNum((playerList->count()) * (playerList->count()-1)));


}

void Trainer::stop(){

    percentStarted->setValue(0);
    percentFinished->setValue(0);
    QVector<QPair<QString,QVector<QPair<QPair<QString,Tournament::win_state>,QString> > > > allData;
    QVector<QString> allPlayers;
    if(gamesRemainingToBeFinished==0){  // if we actually finished all of the trials
        qDebug() << "Number of games played: " << playerNamesVector.size();
        for( int i = 0; i<playerNamesVector.size();i++){
            if(!allPlayers.contains(playerNamesVector[i])){
                allPlayers.push_back(playerNamesVector[i]);
                allData.push_back(qMakePair(playerNamesVector[i],QVector<QPair<QPair<QString,Tournament::win_state>,QString> >() ) );
            }
            int j = allPlayers.indexOf(playerNamesVector[i]);
            allData[j].second.push_back(qMakePair(qMakePair(oppNamesVector[i],gameWinners[i]),gameStrings[i]));
        }
        qDebug() << "EMIT FINISHED ";
        emit finished(allData);
    }
    allData.clear();
    gamesRunning = false;
    gamesRemainingToBeStarted=0;
    gamesRemainingToBeFinished=0;
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
void Trainer::pauseButtonPressed(){
    if(pauseButton->text()==PAUSE_BUTTON_PAUSE_TEXT){
        qDebug() << "Pause button pressed" << endl;
        this->setPauseButton();
        trainerModerator->pauseGame();
    }
    else{
        if(pauseButton->text()==PAUSE_BUTTON_RESUME_TEXT){
            qDebug() << "Resume button pressed" << endl;
            this->resetPauseButton();
            trainerModerator->resumeGame();
        }
    }}
void Trainer::start(int recursion, QPair<QStringList,QStringList> fileNames){
    if(recursion>5){
        if(type==2){
            playerNamesVector.push_back(fileNames.first.join("/"));
            oppNamesVector.push_back(fileNames.second.join("/"));
        }
        addGamestringToVector(QString(""));
        QStringList stringList1 = fileNames.first;
        stringList1.removeFirst();
        if(!testerModerator->startProgram(fileNames.first,true))
            appendToWinnersVector(Tournament::LOSS);
        else{
            qDebug() << "Second player failed";
            QStringList stringList2 = fileNames.second;
            stringList2.removeFirst();
            appendToWinnersVector(Tournament::WIN);
        }
        gamesRemainingToBeStarted-=1;
        gameHasEnded();
        return;
    }
    if(gamesRemainingToBeStarted<=0){
        gamesRemainingToBeFinished = 1;  // Hack to make sure we don't try to calculate a tournament tree if no games were played
        stop();
        return;
    }
    if(fileNames == qMakePair(QStringList(),QStringList()))
        fileNames = getFileNamesFromPlayerSelectors(type==2);
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
        start(recursion+1, fileNames);
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
    else{
        stop();
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
    qDebug() << "Go button pressed";
    if(gamesRunning==false){
        qDebug() << "GO!";
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
        gamesRunning = true;
        start();
    }
    else{
        qDebug()<<"stop!";
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
            QString playerListItem = playerList->item(index)->data(Qt::UserRole).toString();
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
            if(player2PullIndex<(playerList->count()-1))
                player2PullIndex++;
            else{
                if(player1PullIndex<(playerList->count()-2))
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
        qDebug() << "player1PullIndex = " << player1PullIndex;
        qDebug() << "player2PullIndex = " << player2PullIndex;
        return qMakePair(player1PullIndex,player2PullIndex);
    }
    else{
        switchSides = false;
        qDebug() << "player1PullIndex = " << player1PullIndex;
        qDebug() << "player2PullIndex = " << player2PullIndex;
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
