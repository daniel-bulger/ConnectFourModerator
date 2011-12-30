#ifndef Game_H
#define Game_H
#include <QtCore>
class Game
{

  public:
  Game();
  Game(QList<int> moves);
  Game(const Game& oldGame);
  Game& operator= (const Game& old);
  QPair<int,QPair<int,int> > playMove(int playerInput);
  int getLastColumnPlayed();
  int getLastRowPlayed();
  int getLastPiecePlayed();
  int getNumMoves();
  bool getWinner(int col, int row,bool careAboutColor);
  void generateThreats(int col, int row);
  bool canPlacePiece(int position);
  int getNextPieceToPlay();
  int getObviousMove();
  int getNthMove(int n);
  void placePiece(int position);
  void removePiece(int position);
  void switchTurn(); //WARNING: WILL INVALIDATE BOARD
  void print();
  void printThreats();
  QString getMoveString();
  QVector<QVector<int> > threats;
private:
  QVector<QVector<int> > pieces;
  QList<int> moves;
  QVector<int> rowFill;
  int colorToPlace;
};

#endif // Game_H
