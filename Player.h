#ifndef PLAYER_H
#define PLAYER_H

#include <QProcess>

class Player : public QProcess
{
public:
    Player(bool manual, QString progName, QStringList args = QStringList());
    void startManual();
    bool getQuestionMark();
};

#endif // PLAYER_H
