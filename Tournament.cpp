#include "Tournament.h"
Tournament::Tournament(players_results_t players_results)
{
    results = new players_results_t(players_results);

    std::random_shuffle(results->begin(), results->end());

    this->tournamentTree = new t_tree_t();
    tournamentTree->resize(ceil(log2(results->size())));
    for (int i=0; i<tournamentTree->size(); i++)
    {
        tournamentTree->data()[i].resize(ceil(results->size()/pow(2, i)));
    }

    for (int i=0; i<tournamentTree->at(0).size(); i++)
    {
        tournamentTree->data()[0][i] = results->data()[i].first;
    }

    for (int i=1; i<tournamentTree->size(); i++)
    {
        for (int j=0; j<tournamentTree->at(i).size(); j++)
        {
            if (tournamentTree->data()[i-1].size() % 2 != 0 && j == tournamentTree->data()[i].size())
            {
                tournamentTree->data()[i][j] = tournamentTree->data()[i-1][j*2];
            }
            else
            {
                tournamentTree->data()[i][j] = winner(tournamentTree->data()[i-1][j*2], tournamentTree->data()[i-1][j*2+1]);
            }
        }
    }

}

QString Tournament::winner(QString & p1, QString & p2)
{

    win_state p1vp2, p2vp1;

    for (int i=0; i<results->size(); i++)
    {
        if (results->at(i).first == p1)
        {
            game_results_t game1 = locate(results->at(i).second, p2);
            p1vp2 = game1.first.second;
        }
        if (results->at(i).first == p2)
        {
            game_results_t game2 = locate(results->at(i).second, p1);
            p2vp1 = game2.first.second;
        }
    }

    if (p1vp2 + p2vp1 == 0)
    {
        return (qrand() % 2 ? p1 : p2);
    }

    if (p1vp2 == 1)
        return p1;
    else
        return p2;

}

Tournament::game_results_t Tournament::locate(games_results_t haystack, QString needle)
{
    for (int i=0; i<haystack.size(); i++)
    {
        if (haystack[i].first.first == needle)
        {
            return haystack[i];
        }
    }
}

void Tournament::printTree()
{
    for (int i=0; i<tournamentTree->size(); i++)
    {
        qDebug() << "----------------------------------------";
        qDebug() << " Round " << i << ":";
        qDebug() << "----------------------------------------";
        for (int j=0; j<tournamentTree->data()[i].size(); j+=2)
        {
            if (j+1 < tournamentTree->data()[i].size())
                qDebug() << tournamentTree->data()[i][j] << " vs. " << tournamentTree->data()[i][j+1];
            else
                qDebug() << tournamentTree->data()[i][j] << " gets to pass this round";
        }
        qDebug() << endl;
    }
}
