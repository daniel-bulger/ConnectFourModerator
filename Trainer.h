#ifndef TRAINER_H
#define TRAINER_H

#include "Moderator.h"
#include "ControlPanel.h"
#include <QtGui>
#include <QtCore>
#include <QMainWindow>
class ControlPanel;
class Trainer : public QWidget
{
    Q_OBJECT

public:
    Trainer(ControlPanel* parent = 0, bool isSimple = true);
    ~Trainer();


private:
    ControlPanel* parent;
    Moderator* trainerModerator;
    QVBoxLayout* simpleLayout;
    QVBoxLayout* advancedLayout;
    QPushButton* goButton;
    QLabel* numTrialsLabel;
    QLineEdit* numTrialsInput;
    void closeEvent(QCloseEvent *event);

public slots:
    void pause();
    void resume();
    void stop();
    void start();
};

#endif // TRAINER_H
