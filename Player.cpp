#include "Player.h"
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
            file = new QFile("./player1.txt");
            this->setStandardOutputFile("./player1.txt",QIODevice::Truncate);



        }
        else{
            file = new QFile("./player2.txt");
            this->setStandardOutputFile("./player2.txt",QIODevice::Truncate);
        }
        oldOutput = "";
        if(args==QStringList()) start(progName,QStringList());
        else start(progName,args);

            if (!waitForStarted()) {
                qDebug("HI");

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
    qDebug() << "SLEEP";
    QTime dieTime = QTime::currentTime().addMSecs(mSecs);
    while( QTime::currentTime() < dieTime )
            QCoreApplication::processEvents(QEventLoop::AllEvents, mSecs);
    qDebug()<< "DONE";
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
    qDebug() << newInput;
    int oldLen = oldOutput.length();
    oldOutput = newInput;
    if(newInput.endsWith("\n")){
        newInput.chop(2);
    }
    QString ret = newInput.remove(0,oldLen);
    qDebug() << (ret=="p");
    qDebug() << ret;
    file->close();
    return ret;

}
