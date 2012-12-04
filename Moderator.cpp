
#include "Moderator.h"
Moderator::Moderator()
{
    commandLine = false;
    gamestate = GAME_STOPPED;
    timer = new QTimer();
    timePerTurnTimer = new QTimer();
    connect(this->timePerTurnTimer,SIGNAL(timeout()),this,SLOT(decrementTimePerTurnTimer()));
    timePerMove = 10000;
    delayBeforeMove = 1000;
    //choose your directory
    player1GoesFirst = true;
    player1 = NULL;
    player2 = NULL;
}

Moderator::~Moderator()
{

}
#ifdef Q_OS_WIN
#include <windows.h> // for Sleep
#endif
void Moderator::qSleep(int ms)
{
#ifdef Q_OS_WIN
    Sleep(uint(ms));
#else
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
#endif
}

void Moderator::directoryTextBoxEdited(){

}
bool Moderator::playerMove(bool isPlayer1,int input){
    bool retVal = true;
    QPair<int,QPair<int,int> > playerOutput = currentGame.playMove(input);
    if(playerOutput.first==-1){
        if(isPlayer1){
            if(player1->isManual) {
                console("INVALID");
                writeLineToTerminal("INVALID");
                return false;
            }
            console("Player 1 placed a piece in an invalid location. ("+QString().setNum(input)+")");
            player2Wins(true);
        }
        else{
            if(player2->isManual) {
                console("INVALID");
                writeLineToTerminal("INVALID");
                return false;
            }
            console("Player 2 placed a piece in an invalid location. ("+QString().setNum(input)+")");
            player1Wins(true);
        }
        return false;
    }
    emit(piecePlaced(playerOutput.second.first,playerOutput.second.second));

    if(playerOutput.first==-2){
        tieGame();
        retVal = false;
    }

    if(playerOutput.first==0){
        if(isPlayer1){
            if(commandLine)
                writeLineToTerminal("PLAYER1MOVE"+QString().setNum(playerOutput.second.first)+QString().setNum(playerOutput.second.second));
            player1Wins(false);
        }
        else{
            if(commandLine)
                writeLineToTerminal("PLAYER2MOVE"+QString().setNum(playerOutput.second.first)+QString().setNum(playerOutput.second.second));
            player2Wins(false);
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

        QString moveChars = QString().setNum(move);
        moveChars.append("\n");
        player1MadeAMove = false;

        console(QString("Player 1's move: " + QString().setNum(move)));
        if(playerMove(true,move)==false){
            return;
        }
        writeLineToTerminal("PLAYER1MOVE"+QString().setNum(move)+QString().setNum(currentGame.getColFill(move-1)));  // 1 has already been added to colfill

        if(!player2->isManual){
            player2->write(moveChars.toStdString().c_str());
            player2->waitForBytesWritten();
        }
        gamestate = PLAYER_2_TO_MOVE;
        emit(player2ToMove(player2->isManual));
        timeRemaining = timePerMove;
        timeUntilMove = delayBeforeMove;
        emit timeUntilMoveChanged(timePerMove,true);
        emit timeUntilMoveChanged(timePerMove,false);


    }
    else{
    if(gamestate==PLAYER_2_TO_MOVE && player2MadeAMove){
        int move = plyr2Move;
        QString moveChars = QString().setNum(move);
        moveChars.append("\n");
        player2MadeAMove = false;
        console(QString("Player 2's move: " + QString().setNum(move)));

        if(playerMove(false, move)==false){
            return;
        }
        writeLineToTerminal("PLAYER2MOVE"+QString().setNum(move)+QString().setNum(currentGame.getColFill(move-1)));

        if(!player1->isManual){
            player1->write(moveChars.toStdString().c_str());
            player1->waitForBytesWritten();
        }

        gamestate = PLAYER_1_TO_MOVE;
        emit(player1ToMove(player1->isManual));
        timeRemaining = timePerMove;
        timeUntilMove = delayBeforeMove;
        emit timeUntilMoveChanged(timePerMove,true);
        emit timeUntilMoveChanged(timePerMove,false);
    }
    }
}
void Moderator::playerHasMoved(bool isPlayer1){
    Player* player;
    if(isPlayer1) player = player1;
    else player = player2;
    if(!player)
        return;
    if(gamestate!=GAME_STOPPED){
        QString input = QString(player->readNewInput());
        QTextStream qtin(stdin);
        if(player->isManual && commandLine){
            timer->stop();
            timePerTurnTimer->stop();
            qtin >> input;
            timer->start(10);
            timePerTurnTimer->start(10);
        }
        if((!player->isManual)&&player->state()==QProcess::NotRunning){
            if(isPlayer1)
                player2Wins(true);
            else
                player1Wins(true);
            return;
        }

        if(input=="")
            return;
        if(isPlayer1)
            player1MadeAMove = true;
        else
            player2MadeAMove = true;
        if(isPlayer1)
            plyr1Move = QString(input).toInt();
        else
            plyr2Move = QString(input).toInt();
    }
}

void Moderator::player1HasMoved(){
    playerHasMoved(true);
}
void Moderator::player2HasMoved(){
    playerHasMoved(false);
}
void Moderator::player1DroppedPiece(int col){
    if(gamestate!=GAME_STOPPED){
        player1MadeAMove = true;
        plyr1Move = col;
    }
}
void Moderator::player2DroppedPiece(int col){
    if(gamestate!=GAME_STOPPED){
        player2MadeAMove = true;
        plyr2Move = col;
    }
}

void Moderator::player1Wins(bool dueToError){

    console("PLAYER 1 WINS");
    if(commandLine)
        qSleep(1000);
    if(dueToError){
        writeLineToTerminal("PLAYER 1 WINS DUE TO PLAYER 2 ERROR");
        emit gameOver(-2);
        if(player1)
            player1->write("0\n");
        if(player2)
            player2->write("0\n");
    }
    else{
        writeLineToTerminal("PLAYER 1 WINS");

        emit gameOver(1);
        player1->write("-1\n");
        player2->write("-1\n");
    }
    endGame();
}
void Moderator::player2Wins(bool dueToError){
    console("PLAYER 2 WINS");
    if(commandLine)
        qSleep(1000);
    if(dueToError){
        writeLineToTerminal("PLAYER 2 WINS DUE TO PLAYER 1 ERROR");
        emit gameOver(-1);
        if(player1)
            player1->write("0\n");
        if(player2)
            player2->write("0\n");
    }
    else{
        writeLineToTerminal("PLAYER 2 WINS");
        emit gameOver(2);
        player1->write("-2\n");
        player2->write("-2\n");
    }
    endGame();
}
void Moderator::tieGame(){
    if(commandLine)
        qSleep(1000);
    emit gameOver(0);
    console("TIE GAME");
    writeLineToTerminal("TIE");

    player1->write("-3\n");
    player2->write("-3\n");
    endGame();
}

void Moderator::endGame(){
    if(player1&&!player1->isManual){
        player1->disconnect();
        player1->close();
    }
    if(player2&&!player2->isManual){
        player2->disconnect();
        player2->close();
    }
    QString moveString = currentGame.getMoveString();
    if(moveString!=""){
        moveString +="\n";
        QString filePath = logFilePath;
        if(!commandLine){
            filePath+= "/log.txt";
            QFile file(filePath);
            file.open(QIODevice::WriteOnly | QIODevice::Append);
            file.write(QByteArray(moveString.toStdString().c_str()));
            file.close();
        }
        else{
            exit(1);
        }
    }
    if(player1)
        player1->deleteLater();
    player1 = NULL;
    if(player2)
        player2->deleteLater();
    player2 = NULL;
    gamestate = GAME_STOPPED;
    emit gamestring(currentGame.getMoveString());
    emit gameHasEnded();
}
bool Moderator::startGame(QStringList player1FileName, QStringList player2FileName, QString logFolder, bool swapTurns, bool cmd, int timeToRespond){
    commandLine = cmd;
    logFilePath = logFolder;
    if(startProgram(player1FileName,true, timeToRespond)&&startProgram(player2FileName,false,timeToRespond)){
        if(!player1||!player2)
            return false;
        if (player1GoesFirst || (!swapTurns)) {
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
                player2Wins(true);
                emit loadFailed(player1FileName.join(" "));
                return false;
            }
        }
        if(gamestate==PLAYER_2_QUESTION_MARK){

            if(player2->getQuestionMark()){
                gamestate = PLAYER_1_TO_MOVE;
            }
            else{
                console("Player 2 failed to output a '?', so player 1 wins.");
                player1Wins(true);
                emit loadFailed(player2FileName.join(" "));
                return false;
            }

        }

        player1MadeAMove = false;
        player2MadeAMove = false;
        if(!player1||!player2)
            return false;
        if(!player1->isManual){
            //connect(player1,SIGNAL(readyReadStandardError()),this,SLOT(player1Debug()));
        }
        else{
            emit acceptManualInput();
        }
        if(!player2->isManual){
            //connect(player2,SIGNAL(readyReadStandardError()),this,SLOT(player2Debug()));
        }
        else{
            emit acceptManualInput();
        }
        timeRemaining = timePerMove;
        timeUntilMove = delayBeforeMove;
        emit timeUntilMoveChanged(timePerMove,true);
        emit timeUntilMoveChanged(timePerMove,false);
        timer->start(10);
        timePerTurnTimer->start(10);
        currentGame = Game();
        return true;
    }
    return false;

}

void Moderator::pauseGame(){
    if(gamestate==PLAYER_1_TO_MOVE){
        gamestate = GAME_PAUSED_PLAYER_1_TO_MOVE;
    }
    else if(gamestate==PLAYER_2_TO_MOVE){
        gamestate = GAME_PAUSED_PLAYER_2_TO_MOVE;
    }
}
void Moderator::resumeGame(){
    if(gamestate==GAME_PAUSED_PLAYER_1_TO_MOVE){
        gamestate = PLAYER_1_TO_MOVE;
    }
    else if(gamestate==GAME_PAUSED_PLAYER_2_TO_MOVE){
        gamestate = PLAYER_2_TO_MOVE;
    }
}
bool Moderator::initializeProgram(Player* &player, QString progName, QStringList args, int timeToRespond){
    QStringList fullProgram = args;
    fullProgram.prepend(progName);
    try{
        player = new Player(true,progName,args, timeToRespond);
    }
    catch(bool){
        delete player;
        if(progName.endsWith(".exe")){
            try{
                player = new Player(true,"wine",QStringList(fullProgram),timeToRespond);
            }
            catch(bool){
                delete player;
                return false;
            }
        }
        else if(progName.endsWith(".py")){
            try{
                player = new Player(true,"python",QStringList(fullProgram),timeToRespond);
            }
            catch(bool){
                delete player;
                return false;
            }
        }
        else if(progName.endsWith(".jar")){
            try{
                QStringList fullProgramWithJarFlag = fullProgram;
                fullProgramWithJarFlag.prepend("-jar");
                player = new Player(true,"java",QStringList(fullProgramWithJarFlag),500);
            }
            catch(bool){
                delete player;
                return false;
            }
        }
        else{
            delete player;
            return false;
        }
    }
    return true;
}

bool Moderator::testProgram(QString progName, QStringList args, int timeToRespond){
    Player* player = NULL;
    // attempt to initialize the program.  If it does not init, return false
    if(!initializeProgram(player, progName, args, timeToRespond)){
        return false;
    }
    player->write("2\n");
    if(player->getQuestionMark()){
        delete player;
        return true;
    }
    else{
        delete player;
        return false;
    }
}

bool Moderator::startProgram(QStringList programName, bool isPlayer1, int timeToRespond){
    QString progName = QString("");
    if(!programName.isEmpty())
     progName= programName.first();
    QStringList args = programName;
    if(!args.isEmpty())
        args.removeFirst();
    console(progName);
    Player* player;
    if(isPlayer1){
        player = player1;
        if(player1!=NULL){
            player1->disconnect();
            player1->close();
            player1->deleteLater();
            player1 = NULL;
        }
    }
    else{
        player = player2;
        if(player2!=NULL){
            player2->disconnect();
            player2->close();
            player2->deleteLater();
            player2 = NULL;
        }
    }
    // attempt to initialize the program.  If it does not init, return false
    if(!initializeProgram(player, progName, args, timeToRespond)){
        return false;
    }
    if(isPlayer1){
        player1 = player;
        connect(player1,SIGNAL(readyReadStandardError()),SLOT(player1Debug()));
    }
    else{
        player2 = player;
        connect(player2,SIGNAL(readyReadStandardError()),SLOT(player2Debug()));

    }
    emit loadSuccess(programName);
    return true;
}
void Moderator::player1Debug(){
    QString input = player1->readAllStandardError();
    if(input!=""){
        input = "Player 1 says: " + input;
        console(input);
    }
}
void Moderator::player2Debug(){
    QString input = player2->readAllStandardError();
    if(input!=""){
        input = "Player 2 says: " + input;
        console(input);
    }
}
void Moderator::alert(QString message){
    QMessageBox myBox;
    myBox.setText(message);
    myBox.exec();
}



void Moderator::console(QString message){
    emit consoleOutput(message);

}
void Moderator::writeLineToTerminal(QString message){
    if(commandLine){
        std::cout << message.toStdString().c_str() << std::endl;
    }
}

void Moderator::decrementTimePerTurnTimer(){
    timeUntilMove-=10;
    if(!player1||!player2)
        return;
    if(((timeUntilMove<=0))||((gamestate==PLAYER_1_TO_MOVE)&&(player1->isManual)
            )||((gamestate==PLAYER_2_TO_MOVE)&&(player2->isManual))){
        lookForMove();
    }
}

void Moderator::updateTimer()
{
    if (gamestate==PLAYER_1_TO_MOVE) {
        this->timeRemaining-=10;
        if(timeRemaining<=0) player2Wins();
    } else if (gamestate==PLAYER_2_TO_MOVE) {
        this->timeRemaining-=10;
        if(timeRemaining<=0) player1Wins();
    }
}
int Moderator::getCurrentPlayer(){
    if(gamestate==PLAYER_1_TO_MOVE)
        return 1;
    if(gamestate==PLAYER_2_TO_MOVE)
        return 2;
    return 0;
}
void Moderator::setTimeUntilMove(int deciseconds){
    delayBeforeMove = deciseconds*100;
}
