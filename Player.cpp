#include "Player.h"
Player::Player(bool manual, QString progName, QStringList args)
{
    if(manual){ startManual();}
    else{
        if(args==QStringList()) start(progName);
        else start(progName,args);

            if (!waitForStarted()) {
                throw false;
            }
            if (waitForReadyRead(100)) {
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
bool Player::getQuestionMark(){
    waitForReadyRead(100);
    char input;
    getChar(&input);
    if(input == '?'){
        return true;
    }
    else{
        return false;
    }
}
