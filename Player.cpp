#include "Player.h"
Player::Player(QString progName, QStringList args):progName(progName),args(args)
{

    if(progName=="MANUAL_MODE"){
        isManual = true;
        startManual();
    }
    else{
        //parsePath();
        isManual = false;
        if(args==QStringList()) start(progName,QStringList());
        else start(progName,args);

            if (!waitForStarted()) {
                throw false;
            }
            if (waitForReadyRead(2000)) {
                setReadChannel(QProcess::StandardOutput);
                char input;
                getChar(&input);
                if (QString(input) != "p")
                {
                    throw false;
                }
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

bool Player::getQuestionMark(){
    waitForReadyRead(100);
    char input;
    getChar(&input);
    qDebug() << QString(input);
    if(QString(input) == "?"){
        return true;
    }
    else{
        return false;
    }
}
