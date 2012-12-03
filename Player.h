#ifndef PLAYER_H
#define PLAYER_H

#include <QProcess>
#include <QtCore>

class Player : public QProcess
{
public:
    bool isManual;
    QString fileName;
    QFile* file;
    QString progName;
    QStringList args;
    int timeToRespond;
    Player();
    Player(const Player&);
    Player(bool isPlayer1,QString progName, QStringList args = QStringList(), int timeToRespond = 100);
    ~Player();
    void startManual();
    bool getQuestionMark();
    void parsePath();
    QString oldOutput;
    void sleep(int mSecs);
    QString readNewInput();
    static int nextId;
    static int getNextId(){
        nextId++;
        return nextId;
    }
};
#endif // PLAYER_H
