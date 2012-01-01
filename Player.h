#ifndef PLAYER_H
#define PLAYER_H

#include <QProcess>
#include <QtCore>

class Player : public QProcess
{
public:
    bool isManual;
    QString progName;
    QStringList args;
    Player(QString progName, QStringList args = QStringList());
    void startManual();
    bool getQuestionMark();
    void parsePath();
};

#endif // PLAYER_H
