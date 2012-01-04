
#include "Moderator.h"

Moderator::Moderator(QWidget *parent)
    : QMainWindow(parent)
{
    this->setVisible(false);
    //choose your directory
    settings = new QSettings("Minds and Machines", "Connect Four");
    AIFolder = new QString(settings->value("AI_DIRECTORY").toString());
    //initialize board
    gameBoard = new Board(this);
    //initialize control panel
    controlPanel = new ControlPanel(this);
    qDebug("here");


    connect(controlPanel->chooseDirectoryText,SIGNAL(textEdited()),this,SLOT(directoryTextBoxEdited()));
    connect(controlPanel->chooseDirectoryButton,SIGNAL(clicked()),this,SLOT(chooseDirectory()));
    connect(controlPanel->player1FileName, SIGNAL(currentIndexChanged(int)), this, SLOT(loadPlayer1Program(int)));
    connect(controlPanel->player2FileName, SIGNAL(currentIndexChanged(int)), this, SLOT(loadPlayer2Program(int)));
    connect(controlPanel->timePerTurnTimer,SIGNAL(timeout()),this,SLOT(decrementTimePerTurnTimer()));
    connect(controlPanel->timer, SIGNAL(timeout()), this, SLOT(updateTimer()));

    //initializing game start/stop button
    gamestate = GAME_STOPPED;
    connect(controlPanel->go, SIGNAL(clicked()), this, SLOT(goButtonPressed()));
    connect(controlPanel->pause, SIGNAL(clicked()), this, SLOT(pauseButtonPressed()));

    player1GoesFirst = true;
    player1 = NULL;
    player2 = NULL;
}

Moderator::~Moderator()
{

}

void Moderator::chooseDirectory(){
    QString folder = QFileDialog::getExistingDirectory(0,"Choose the folder where your AIs are located");
    controlPanel->chooseDirectoryText->setText(folder);
    AIFolder = new QString(folder);
    settings->setValue("AI_DIRECTORY",folder);
    controlPanel->populateComboBox(true);
    controlPanel->populateComboBox(false);
}

void Moderator::directoryTextBoxEdited(){

}
bool Moderator::playerMove(bool isPlayer1,int input){
    bool retVal = true;
    QPair<int,QPair<int,int> > playerOutput = currentGame.playMove(input);
    if(playerOutput.first==-1){
        if(isPlayer1){
            console("Player 1 placed a piece in an invalid location.");
            player2Wins();
        }
        else{
            console("Player 2 placed a piece in an invalid location.");
            player1Wins();
        }
        return false;
    }
    gameBoard->place(playerOutput.second.first,playerOutput.second.second);

    if(playerOutput.first==-2){
        tieGame();
        retVal = false;
    }

    if(playerOutput.first==0){
        if(isPlayer1){
            player1Wins();
        }
        else{
            player2Wins();
        }
        retVal = false;
    }
    return retVal;
}

void Moderator::lookForMove(){
    if(gamestate==PLAYER_1_TO_MOVE)
        player1HasMoved();
    if(gamestate==PLAYER_2_TO_MOVE)
        player2HasMoved();
    if(gamestate==PLAYER_1_TO_MOVE && player1MadeAMove){
        int move = plyr1Move;
        console(QString("Player 1's move: " + QString().setNum(move)));
        QString moveChars = QString().setNum(move);
        moveChars.append("\n");
        qDebug() << moveChars;

        if(playerMove(true,move)==false){
            return;
        }
        if(!player2->isManual){
            player2->write(moveChars.toStdString().c_str());
            player2->waitForBytesWritten();
            qDebug() << "DOME" ;
        }
        player1MadeAMove = false;
        gamestate = PLAYER_2_TO_MOVE;
        if(!player2->isManual){
            gameBoard->highlightGraphic->hide();
        }
        else{
            gameBoard->mouseMoveEvent();
        }
        controlPanel->player1TimeRemainingBar->setValue(MOVE_TIME_LIMIT*1000);
        controlPanel->player2TimeRemainingBar->setValue(MOVE_TIME_LIMIT*1000);
        timeUntilMove = controlPanel->timePerTurnSlider->sliderPosition() * 10;
    }
    else{
    if(gamestate==PLAYER_2_TO_MOVE && player2MadeAMove){
        int move = plyr2Move;
        console(QString("Player 2's move: " + QString().setNum(move)));
        QString moveChars = QString().setNum(move);
        moveChars.append("\n");
        if(playerMove(false, move)==false){
            return;
        }
        if(!player1->isManual){
            player1->write(moveChars.toStdString().c_str());
            player1->waitForBytesWritten();
        }

        player2MadeAMove = false;
        gamestate = PLAYER_1_TO_MOVE;
        if(!player1->isManual){
            gameBoard->highlightGraphic->hide();
        }
        else{
            gameBoard->mouseMoveEvent();
        }
        controlPanel->player1TimeRemainingBar->setValue(MOVE_TIME_LIMIT*1000);
        controlPanel->player2TimeRemainingBar->setValue(MOVE_TIME_LIMIT*1000);
        timeUntilMove = controlPanel->timePerTurnSlider->sliderPosition() * 10;
    }
    }
}

void Moderator::player1HasMoved(){
    if(gamestate!=GAME_STOPPED){
        qDebug() << "MEH";
        QString input = QString(player1->readAllStandardOutput());
        if(input=="")
            return;
        player1MadeAMove = true;

        if(QString(input).toInt()<=0||QString(input).toInt()>7)
            player2Wins();
        plyr1Move = QString(input).toInt();
    }
}
void Moderator::player2HasMoved(){
    if(gamestate!=GAME_STOPPED){
        qDebug() << "MAAAAAAAAAAH";

        QString input = QString(player2->readAllStandardOutput());
        if(input=="")
            return;
        player2MadeAMove = true;

        if(QString(input).toInt()<=0||QString(input).toInt()>7)
            player1Wins();
        plyr2Move = QString(input).toInt();
    }
}

void Moderator::player1DroppedPiece(int col){
    if(gamestate!=GAME_STOPPED){
        player1MadeAMove = true;
        plyr1Move = col;
    }
}
void Moderator::player2DroppedPiece(int col){
    player2MadeAMove = true;
    plyr2Move = col;
}

void Moderator::player1Wins(bool dueToError){
    gameBoard->gameResult(1);
    console("PLAYER 1 WINS");
    if(dueToError){
        player1->write("0\n");
        player2->write("0\n");
    }
    else{
        player1->write("-1\n");
        player2->write("-1\n");
    }
    endGame();
}
void Moderator::player2Wins(bool dueToError){
    gameBoard->gameResult(2);
    console("PLAYER 2 WINS");
    if(dueToError){
        player1->write("0\n");
        player2->write("0\n");
    }
    else{
        player1->write("-2\n");
        player2->write("-2\n");
    }
    endGame();
}
void Moderator::tieGame(){
    gameBoard->gameResult(0);
    console("TIE GAME");
    player1->write("-3\n");
    player2->write("-3\n");
    endGame();
}

void Moderator::endGame(){
    if(!player1->isManual){
        player1->disconnect();
        player1->close();
    }
    if(!player2->isManual){
        player2->disconnect();
        player2->close();
    }
    QString moveString = currentGame.getMoveString();
    if(moveString!=""){
        moveString +="\n";
        QString logFilePath = *AIFolder;
        logFilePath+= "/log.txt";
        qDebug() <<logFilePath;
        QFile file(logFilePath);
        file.open(QIODevice::WriteOnly | QIODevice::Append);
        file.write(QByteArray(moveString.toStdString().c_str()));
        file.close();
    }
    delete player1;
    player1 = NULL;
    delete player2;
    player2 = NULL;
    gamestate = GAME_STOPPED;
    controlPanel->timePerTurnTimer->stop();
    controlPanel->timer->stop();
    controlPanel->player1FileName->setEnabled(true);
    controlPanel->player2FileName->setEnabled(true);
    controlPanel->player1TimeRemainingBar->setValue(MOVE_TIME_LIMIT*1000);
    controlPanel->player2TimeRemainingBar->setValue(MOVE_TIME_LIMIT*1000);
    controlPanel->resetGoButton();
}

void Moderator::goButtonPressed(){
    if(gamestate==GAME_STOPPED){
        if(goPlayer1Program(controlPanel->player1FileName->currentIndex())&&goPlayer2Program(controlPanel->player2FileName->currentIndex())){
            controlPanel->setGoButton();
            controlPanel->player1TimeRemainingBar->setValue(MOVE_TIME_LIMIT*1000);
            controlPanel->player2TimeRemainingBar->setValue(MOVE_TIME_LIMIT*1000);
            gameBoard->clearPieces();
            if (player1GoesFirst) {
                player1GoesFirst = false;
                if(!player1->isManual){
                    player1->write("1\n");
                    player1->waitForBytesWritten();
                }
                if(!player2->isManual){
                    player2->write("2\n");
                    player2->waitForBytesWritten();
                    gamestate=PLAYER_2_QUESTION_MARK;
                }
                else{
                    gamestate=PLAYER_1_TO_MOVE;
                }
            }
            else {
                player1GoesFirst = true;
                if(!player2->isManual){
                    player2->write("1\n");
                    player2->waitForBytesWritten();
                }
                if(!player1->isManual){
                    player1->write("2\n");
                    player1->waitForBytesWritten();
                    gamestate=PLAYER_1_QUESTION_MARK;
                }
                else{
                    gamestate=PLAYER_2_TO_MOVE;
                }
            }
            if(gamestate==PLAYER_1_QUESTION_MARK){
                if(player1->getQuestionMark()){
                    gamestate = PLAYER_2_TO_MOVE;
                }
                else{
                    console("Player 1 failed to output a '?', so player 2 wins.");
                    player2Wins();
                    return;
                }
            }
            if(gamestate==PLAYER_2_QUESTION_MARK){

                if(player2->getQuestionMark()){
                    gamestate = PLAYER_1_TO_MOVE;
                }
                else{
                    console("Player 2 failed to output a '?', so player 1 wins.");
                    player1Wins();
                    return;
                }

            }
            controlPanel->player1FileName->setEnabled(false);
            controlPanel->player2FileName->setEnabled(false);
            player1MadeAMove = false;
            player2MadeAMove = false;
            if(!player1->isManual){
                connect(player1,SIGNAL(readyReadStandardOutput()),this,SLOT(player1HasMoved()));
                connect(player1,SIGNAL(readyReadStandardError()),this,SLOT(player1Debug()));
            }
            else{
                connect(gameBoard,SIGNAL(pieceDroppedByPlayer(int)),this,SLOT(playerDroppedPiece(int)));
            }
            if(!player2->isManual){
                connect(player2,SIGNAL(readyReadStandardOutput()),this,SLOT(player2HasMoved()));
                connect(player2,SIGNAL(readyReadStandardError()),this,SLOT(player2Debug()));
            }
            else{
                connect(gameBoard,SIGNAL(pieceDroppedByPlayer(int)),this,SLOT(playerDroppedPiece(int)));
            }
            timeUntilMove = controlPanel->timePerTurnSlider->sliderPosition() * 10;
            controlPanel->timePerTurnTimer->start(10);
            controlPanel->timer->start(10);
            currentGame = Game();
        }
    }
    else{
        endGame();
    }
}

void Moderator::pauseButtonPressed(){
    if(gamestate==PLAYER_1_TO_MOVE){
        gamestate = GAME_PAUSED_PLAYER_1_TO_MOVE;
        controlPanel->setPauseButton();
    }
    else if(gamestate==PLAYER_2_TO_MOVE){
        gamestate = GAME_PAUSED_PLAYER_2_TO_MOVE;
        controlPanel->setPauseButton();
    }
    else if(gamestate==GAME_PAUSED_PLAYER_1_TO_MOVE){
        gamestate = PLAYER_1_TO_MOVE;
        controlPanel->resetPauseButton();
    }
    else if(gamestate==GAME_PAUSED_PLAYER_2_TO_MOVE){
        gamestate = PLAYER_2_TO_MOVE;
        controlPanel->resetPauseButton();
    }
}

bool Moderator::loadPlayer1Program(int boxIndex){
    return loadPlayerProgram(true,boxIndex);
}
bool Moderator::loadPlayer2Program(int boxIndex){
    return loadPlayerProgram(false,boxIndex);
}

bool Moderator::loadPlayerProgram(bool isPlayer1, int boxIndex){
    QComboBox* playerFileName;
    QString progName;
    QStringList args;
    Player* player;
    bool* playerIsManual;
    if(isPlayer1){
        if(player1!=NULL){
            player1->disconnect();
            player1->close();
            delete player1;
            player1 = NULL;
        }
        playerFileName = controlPanel->player1FileName;
    }
    else{
        player = player2;
        if(player2!=NULL){
            player2->disconnect();
            player2->close();
            delete player2;
            player2 = NULL;
        }
        playerFileName = controlPanel->player2FileName;
    }

    QString friendlyName = playerFileName->itemText(boxIndex);
    progName = playerFileName->itemData(playerFileName->currentIndex()).toString();
    if(progName=="NONE_SELECTED") return false;
    if(progName=="") return false;
    if(progName=="COMMAND_MODE"){
        bool ok = false;
        QString text = QInputDialog::getText(this, tr("Advanced program entry"),tr("Enter a command that will run your AI."), QLineEdit::Normal,"", &ok);
        if(ok&&text!=""){
            progName = text.split(' ')[0];
            args = text.split(' ');
            args.pop_front();
        }
    }
    try{
    player = new Player(progName,args);
    if(isPlayer1){
        player1 = player;
    }
    else{
        player2 = player;
    }
    }
    catch(bool){
        if(progName.endsWith(".exe")){
            try{
                player = new Player("wine",QStringList(progName));
                if(isPlayer1){
                    player1 = player;
                }
                else{
                    player2 = player;
                }
            }
            catch(bool){
            loadFailed(progName);
            return false;
            }
        }
        else{
            loadFailed(progName);
            return false;
        }
    }
    console("Player " + friendlyName + " ready!");
    return true;
}

bool Moderator::goPlayer1Program(int boxIndex){
return loadPlayerProgram(true,boxIndex);
}

bool Moderator::goPlayer2Program(int boxIndex){
    return loadPlayerProgram(false,boxIndex);
}

void Moderator::alert(QString message){
    QMessageBox myBox;
    myBox.setText(message);
    myBox.exec();
}

void Moderator::loadFailed(QString player)
{
    QMessageBox fail;
    QString message = "Player " + player + " failed to load!";
    console(message);
    fail.setText(message);
    fail.exec();
}

void Moderator::console(QString message){
    controlPanel->console(message);
}

void Moderator::decrementTimePerTurnTimer(){
    timeUntilMove-=1;
    if((timeUntilMove<=0)||((gamestate==PLAYER_1_TO_MOVE)&&(player1->isManual)
            )||((gamestate==PLAYER_2_TO_MOVE)&&(player2->isManual))){
        lookForMove();
    }
}

void Moderator::updateTimer()
{
    if (gamestate==PLAYER_1_TO_MOVE) {
        controlPanel->player1TimeRemainingBar->setValue(controlPanel->player1TimeRemainingBar->value()-10);
        if(controlPanel->player1TimeRemainingBar->value()<=0) player2Wins();
    } else if (gamestate==PLAYER_2_TO_MOVE) {
        controlPanel->player2TimeRemainingBar->setValue(controlPanel->player2TimeRemainingBar->value()-10);
        if(controlPanel->player2TimeRemainingBar->value()<=0) player1Wins();

    } else {
        return;
    }
}
