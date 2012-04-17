#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include <QObject>
#include <QtGui>
#include <algorithm>
#include <cmath>

class Tournament : public QObject
{
    Q_OBJECT
public:
    enum win_state { LOSS = -1, TIE = 0, WIN = 1 };
                         //oppn  //win?  //moves
    typedef QPair< QPair<QString, win_state>, QString > game_results_t;
    typedef QVector< game_results_t > games_results_t;
    typedef QPair< QString, games_results_t > player_results_t;
    typedef QVector< player_results_t > players_results_t;

    //typedef QVector< QPair< QString, QVector< QPair< QPair<QString, bool>, QString > > > > results_t;

    typedef QVector< QVector< QString > > t_tree_t;

private:
    t_tree_t* tournamentTree;
    players_results_t* results;
public:
    Tournament(players_results_t players_results);
    QString winner(QString & p1, QString & p2);
    game_results_t locate(games_results_t haystack, QString needle);
    void printTree();

signals:

public slots:

};

#endif // TOURNAMENT_H
