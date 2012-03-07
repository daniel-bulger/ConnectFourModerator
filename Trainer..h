#ifndef TRAINERDIALOG_H
#define TRAINERDIALOG_H

#include <QtGui>
#include <QtCore>
#include <QMainWindow>

class TrainerDialog : public QDialog
{
    Q_OBJECT

public:
    TrainerDialog(Moderator *parent = 0);
    ~TrainerDialog();
private:
    QLabel* gamesToRun;
    QLineEdit* numGames;
    QPushButton* goButton;
public slots:
    void pause();
    void resume();

};

#endif // TRAINERDIALOG_H
