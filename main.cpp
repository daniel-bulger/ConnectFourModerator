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
            printf("Usage: ./Moderator first/player/program/path second/player/program/path [time for programs to load]");
            exit(0);
        }
        QStringList player1FileNameOrig = QStringList(QString(a.arguments().at(1)));
        QStringList player2FileNameOrig = QStringList(QString(a.arguments().at(2)));
        QStringList player1FileName = player1FileNameOrig;
        QStringList player2FileName = player2FileNameOrig;
        int timeToRespond = 100;
        if(a.arguments().size()>3)
            timeToRespond = a.arguments().at(3).toInt();
        QString player1FileBegin;
        if(player1FileName.length()>0){
        player1FileBegin= player1FileName.first();
        player1FileName.removeFirst();
        }
        QString player2FileBegin;
        if(player2FileName.length()>0){
        player2FileBegin= player2FileName.first();
        player2FileName.removeFirst();
        }
        if(!moderator.testProgram(player1FileBegin,player1FileName,timeToRespond)){
            printf("FAILURE1");
            exit(0);
        }
        if(!moderator.testProgram(player2FileBegin,player2FileName,timeToRespond)){
            printf("FAILURE2");
            exit(0);
        }
        if(!moderator.startGame(player1FileNameOrig, player2FileNameOrig,"",false,true,timeToRespond)){
            printf("FAILURE2");
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
