#include "Player.h"
int Player::nextId = 0;

Player::Player() {}
Player::Player(const Player&) {}

Player::Player(bool isPlayer1,QString progName, QStringList args,int timeToRespond):progName(progName),args(args),timeToRespond(timeToRespond)
{

    if((progName=="MANUAL_MODE") || ((progName == "" )&& args.length() == 1 && (args[0]=="MANUAL_MODE"))){
        qDebug() << "Manual mode engaged";
        isManual = true;
        startManual();
    }
    else{
        isManual = false;
        oldOutput = "";
        if(args==QStringList()) start(progName,QStringList());
        else start(progName,args);

            if (!waitForStarted()) {
                //qDebug("Program did not start");
                //qDebug() << progName;
                if(args!=QStringList()){
                    //qDebug() << args;
                }
                throw false;
            }
            sleep(timeToRespond);
            waitForBytesWritten();
            QString newInput = this->readNewInput();
            qDebug() << newInput;
            if (newInput=="p") {
            }
            else {
                //qDebug("Program did not output a 'p'");
                throw false;
            }
        }
}
Player::~Player(){
    this->kill();
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
    if(isManual)
        return true;
    sleep(timeToRespond);
    waitForReadyRead(timeToRespond);
    if(readNewInput() == "?"){
        return true;
    }
    else{
        return false;
    }
}
QString Player::readNewInput(){
    if(isManual){
        return "";
    }
    QString newInput = QString(readAllStandardOutput());
    newInput.remove('\n');
    newInput.remove('\r');
    QString ret = newInput;
    return ret;

}

