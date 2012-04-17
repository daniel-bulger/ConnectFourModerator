#include "Trainer.h"
#include <QThread>
#define GO_BUTTON_START_TEXT "GO!"
#define GO_BUTTON_STOP_TEXT "STOP"
#define PAUSE_BUTTON_RESUME_TEXT "Resume"
#define PAUSE_BUTTON_PAUSE_TEXT "Pause"
Trainer::Trainer(ControlPanel *parent, bool simple) :parent(parent),
    QWidget(0)
{
    testerModerator = new Moderator;
    trainerModerator = new Moderator;
    connect(trainerModerator,SIGNAL(gameHasEnded()),this,SLOT(gameHasEnded()),Qt::QueuedConnection);
    numberOfCores = QThread::idealThreadCount();
    if(numberOfCores==-1)
        numberOfCores=1;
    gamesRunning = false;
    gamesArePaused = false;
    settings = new QSettings("Minds and Machines", "Connect Four");
    timePerTurnSlider = new QSlider(Qt::Horizontal);
    timePerTurnSlider->setMinimum(0);
    timePerTurnSlider->setMaximum(100);
    timePerTurnSlider->setValue(30);
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
    chooseDirectoryText = new QLineEdit();
    chooseDirectoryText->setText(*(AIFolder));
    playerFileNames = new QVector<QComboBox*>();
    playerFileNames->push_back(new QComboBox());
    playerFileNames->push_back(new QComboBox());

    addPlayer = new QPushButton("Add player");
    removePlayer = new QPushButton("Remove player");
    fewestGamesPerCombinationLabel = new QLabel("Number of games to be played between each pair:");
    fewestGamesPerCombinationSpinBox = new QSpinBox();
    simpleLayout = new QVBoxLayout();
    advancedLayout = new QVBoxLayout();
    advancedLayout->addWidget(playerFileNames->at(0));
    advancedLayout->addWidget(playerFileNames->at(1));
    advancedLayout->addWidget(chooseDirectoryButton);
    advancedLayout->addWidget(numTrialsLabel);
    advancedLayout->addWidget(numTrialsInput);
    advancedLayout->addWidget(percentStarted);
    advancedLayout->addWidget(percentFinished);
    advancedLayout->addWidget(timePerTurnSlider);
    advancedLayout->addWidget(goButton);
    advancedLayout->addWidget(pauseButton);
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
    connect(goButton, SIGNAL(clicked()), this, SLOT(goButtonPressed()));
    connect(chooseDirectoryButton,SIGNAL(clicked()),this,SLOT(chooseDirectory()));
    if(simple==true){
        this->setLayout(simpleLayout);
    }
    else{
        this->setLayout(advancedLayout);
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
void Trainer::stop(){
    gamesRunning = false;
    gamesRemainingToBeStarted=0;
    gamesRemainingToBeFinished=0;
    percentStarted->setValue(0);
    percentFinished->setValue(0);

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
    QPair<QStringList,QStringList> fileNames = getFileNamesFromPlayerSelectors();
    trainerModerator->setTimeUntilMove(timePerTurnSlider->value());
    QStringList file1MinusFirst = fileNames.first;
    file1MinusFirst.removeFirst();
    QStringList file2MinusFirst = fileNames.second;
    file2MinusFirst.removeFirst();
        if(trainerModerator->startGame(fileNames.first,fileNames.second,*AIFolder)){
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
QPair<QStringList,QStringList> Trainer::getFileNamesFromPlayerSelectors(){
    QComboBox* playerFileName;
    QString progName;
    QStringList args;
    QPair<QStringList,QStringList> retVals;
    QPair<int,int> selectorNums = getNextPlayers();
    int index;
    int i = 0;
    while(i<2){
        if(i==0)
            index=selectorNums.first;
        else
            index=selectorNums.second;
        i++;
        playerFileName = playerFileNames->at(index);
        QString friendlyName = playerFileName->itemText(playerFileName->currentIndex());
        progName = playerFileName->itemData(playerFileName->currentIndex()).toString();
        if(friendlyName!="COMMAND")playerFileName->setItemData(playerFileName->findText("COMMAND"),QVariant("COMMAND_MODE"));
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
QPair<int,int> Trainer::getNextPlayers(){
    if(switchSides==false){
        if(player2PullIndex<playerFileNames->size()-1)
            player2PullIndex++;
        else{
            if(player1PullIndex<playerFileNames->size()-2)
                player1PullIndex++;
            else
                player1PullIndex=0;
            player2PullIndex=player1PullIndex+1;
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
