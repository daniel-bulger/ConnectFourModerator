#include <QtGui>
#include "Moderator.h"
#include "ControlPanel.h"
#include "Board.h"
int main(int argc, char *argv[])
{
    QApplication connectFour(argc, argv);
    Moderator moderator;
    connectFour.installEventFilter(&moderator);
    return connectFour.exec();
}
