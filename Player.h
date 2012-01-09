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
    Player(bool isPlayer1,QString progName, QStringList args = QStringList());
    void startManual();
    bool getQuestionMark();
    void parsePath();
    QString oldOutput;
    void sleep(int mSecs);
    QString readNewInput();
};

#endif // PLAYER_H
