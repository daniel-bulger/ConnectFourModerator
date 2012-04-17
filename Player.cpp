#include "Player.h"
int Player::nextId = 0;

Player::Player() {}
Player::Player(const Player&) {}

Player::Player(bool isPlayer1,QString progName, QStringList args):progName(progName),args(args)
{

    if(progName=="MANUAL_MODE"){
        isManual = true;
        startManual();
    }
    else{
        //parsePath();
        isManual = false;
        if(isPlayer1){
            // TODO figure out where to put these
            QString fileName = "./player1";
            fileName+=QString().setNum(this->getNextId()+QDateTime::currentMSecsSinceEpoch());
            fileName+=".txt";
            file = new QFile(fileName);
            this->setStandardOutputFile(fileName,QIODevice::Truncate);
        }
        else{
            QString fileName = "./player2";
            fileName+=QString().setNum(this->getNextId()+QDateTime::currentMSecsSinceEpoch());
            fileName+=".txt";
            file = new QFile(fileName);
            this->setStandardOutputFile(fileName,QIODevice::Truncate);
        }
        oldOutput = "";
        if(args==QStringList()) start(progName,QStringList());
        else start(progName,args);

            if (!waitForStarted()) {
                throw false;
            }
            sleep(100);
            if (readNewInput()=="p") {
            }
            else {
                throw false;
            }
        }
}
void Player::startManual(){

}
void Player::parsePath(){
#ifdef Q_OS_LINUX
    qDebug("LINUX");
    progName.replace(" ","\\ ");
    qDebug() << progName;
#endif
#ifdef Q_OS_MSDOS
#endif
}
void Player::sleep(int mSecs){
    QTime dieTime = QTime::currentTime().addMSecs(mSecs);
    while( QTime::currentTime() < dieTime )
            QCoreApplication::processEvents(QEventLoop::AllEvents, mSecs);
}

bool Player::getQuestionMark(){
    sleep(100);
    if(readNewInput() == "?"){
        return true;
    }
    else{
        return false;
    }
}
QString Player::readNewInput(){
    if(isManual) return "";
    if(!file->open(QIODevice::ReadOnly)){
        qDebug("Failed to open file");
    }
    QString newInput = QString(file->readAll());
    int oldLen = oldOutput.length();
    oldOutput = newInput;
    if(newInput.endsWith("\n")){
        newInput.chop(2);
    }
    QString ret = newInput.remove(0,oldLen);
    file->close();
    return ret;

}

