#include <QObject>
#include <QTimer>

#include <QtGui>
#include <QtCore/QCoreApplication>
#include <stdio.h>
#include "Moderator.h"
#include "ControlPanel.h"
#include "Board.h"
int main(int argc, char *argv[])
{
    if(argc>1){
        QCoreApplication a(argc, argv);
        Moderator moderator;
        if(a.arguments().size()<3){
            printf("Usage: ./Moderator first/player/program/path second/player/program/path");
            exit(0);
        }
        QStringList player1FileName = QStringList(QString(a.arguments().at(1)));
        QStringList player2FileName = QStringList(QString(a.arguments().at(2)));
        if(!moderator.startGame(player1FileName, player2FileName,"",false,true)){
            printf("Failure");
            exit(0);
        }
        return a.exec();
    }
    else{
        QApplication connectFour(argc, argv);
        ControlPanel controlPanel;
        controlPanel.parent->controlPanel=&controlPanel;
        connectFour.installEventFilter(&controlPanel);
        return connectFour.exec();

    }

}
