#include <QtGui>
#include "Moderator.h"
#include "ControlPanel.h"
#include "Board.h"
int main(int argc, char *argv[])
{
    QApplication connectFour(argc, argv);
    ControlPanel controlPanel;
    controlPanel.parent->controlPanel=&controlPanel;
    connectFour.installEventFilter(&controlPanel);
    return connectFour.exec();

}
