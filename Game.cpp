#include "Game.h"
#include <QDebug>

Game::Game()
{
  pieces = QVector<QVector<int> >(7,QVector<int>(6,0));
  threats = QVector<QVector<int> >(7,QVector<int>(6,0));
  rowFill = QVector<int>(7,0);
  colorToPlace = 1;
}
Game::Game(QList<int> initialMoves){
  moves = initialMoves;
  pieces = QVector<QVector<int> >(7,QVector<int>(6,0));
  threats = QVector<QVector<int> >(7,QVector<int>(6,0));
  rowFill = QVector<int>(7,0);
  colorToPlace = 1;
  for(QList<int>::iterator itr = moves.begin();itr!=moves.end();itr++){
    pieces[*itr][rowFill[*itr]] = colorToPlace;
    colorToPlace = 3-colorToPlace;
    rowFill[*itr]++;
  }
}
Game::Game(const Game& oldGame){
moves = oldGame.moves;
pieces = oldGame.pieces;
threats = oldGame.threats;
rowFill = oldGame.rowFill;
colorToPlace = oldGame.colorToPlace;
}
Game& Game::operator= (const Game& oldGame){
moves = oldGame.moves;
pieces = oldGame.pieces;
threats = oldGame.threats;
rowFill = oldGame.rowFill;
colorToPlace = oldGame.colorToPlace;
return *this;
}
QPair<int,QPair<int,int> > Game::playMove(int playerInput){
    QPair<int,QPair<int,int> > retVal;
    int move = playerInput - 1; // we subtract 1 because the game uses 0-6 instead of 1-7 in its internal structure.
    if(move>-1&&move<7 && canPlacePiece(move)){
        placePiece(move);
        if(getNumMoves()==42){
            retVal.first = -2;
            retVal.second = qMakePair(playerInput,getLastRowPlayed()+1);
            return retVal;
        }
    }
    else{
        retVal.first = -1;
        retVal.second = qMakePair(playerInput,7);
        return retVal;
    }
    if(getWinner(getLastColumnPlayed(),getLastRowPlayed(),true)){
        retVal.first = 0;
        retVal.second = qMakePair(getLastColumnPlayed()+1,getLastRowPlayed()+1);
        return retVal;
    }
    retVal.first = 1;
    retVal.second = qMakePair(getLastColumnPlayed()+1,getLastRowPlayed()+1);
    return retVal;

}

int Game::getLastColumnPlayed(){
  return moves.back();
}
int Game::getLastRowPlayed(){
  return rowFill[moves.back()]-1;
}

bool Game::getWinner(int col, int row, bool careAboutColor){

  int yourColor = 3 - colorToPlace; // color of last played piece.
  if((careAboutColor)&&(pieces[col][row]!=yourColor)) return false;
  bool aWinnerIsYou = false;
  int count = 1;
  for(int x = col-1;(x>=0)&&(pieces[x][row]==yourColor);x--){
    count++;
  }
  for(int x = col+1;(x<=6)&&(pieces[x][row]==yourColor);x++){
    count++;
  }
  aWinnerIsYou = (aWinnerIsYou||count>=4);
  count = 1;
  if(aWinnerIsYou)
    return true;
  for(int y = row-1;(y>=0)&&(pieces[col][y]==yourColor);y--){
    count++;
  }
  aWinnerIsYou = (aWinnerIsYou||count>=4);
  if(aWinnerIsYou)
    return true;
  count = 1;
  for(int x = col-1, y = row - 1;(x>=0)&&(y>=0)&&(pieces[x][y]==yourColor);x--,y--){
    count++;
  }
  for(int x = col+1, y = row + 1;(x<=6)&&(y<=5)&&(pieces[x][y]==yourColor);x++,y++){
    count++;
  }
  aWinnerIsYou = (aWinnerIsYou||count>=4);
  if(aWinnerIsYou)
    return true;
  count = 1;
  for(int x = col-1, y = row + 1;(x>=0)&&(y<=5)&&(pieces[x][y]==yourColor);x--,y++){
    count++;
  }
  for(int x = col+1, y = row - 1;(x<=6)&&(y>=0)&&(pieces[x][y]==yourColor);x++,y--){
    count++;
  }
  aWinnerIsYou = (aWinnerIsYou||count>=4);
  return aWinnerIsYou;
}
void Game::generateThreats(int col, int row){  // 1 for first, 2 for second, 3 for both
  int yourColor = 3 - colorToPlace; // color of last played piece.
  int otherColor = colorToPlace;
  int i = 0;

  while(i<2){
    i++;
    bool lastEmpty = false;
    for(int x = col-1;(x>=0)&&(pieces[x][row]!=otherColor&&(!(lastEmpty&&pieces[x][row]!=yourColor)));x--){
      if(lastEmpty==true) lastEmpty=false;
      if(pieces[x][row]!=yourColor){
              lastEmpty = true;
              if(getWinner(x,row,false)){
                if(threats[x][row]==0) threats[x][row]=yourColor;
                if(threats[x][row]==(3-yourColor)) threats[x][row]+=yourColor;
              }
            }
    }
    lastEmpty = false;
    for(int x = col+1;(x<=6)&&(pieces[x][row]!=otherColor&&(!(lastEmpty&&pieces[x][row]!=yourColor)));x++){
      if(lastEmpty==true) lastEmpty=false;
      if(pieces[x][row]!=yourColor){
              lastEmpty = true;
        if(getWinner(x,row,false)){
                if(threats[x][row]==0) threats[x][row]=yourColor;
                if(threats[x][row]==(3-yourColor)) threats[x][row]+=yourColor;
              }
            }
    }
    lastEmpty = false;
    for(int y = row-1;(y>=0)&&(pieces[col][y]!=otherColor&&(!(lastEmpty&&pieces[col][y]!=yourColor)));y--){
      if(lastEmpty==true) lastEmpty=false;
      if(pieces[col][y]!=yourColor){
              lastEmpty = true;
        if(getWinner(col,y,false)){
                if(threats[col][y]==0) threats[col][y]=yourColor;
                if(threats[col][y]==(3-yourColor)) threats[col][y]+=yourColor;
              }
            }
    }
    lastEmpty = false;
    for(int y = row+1;(y<=5)&&(pieces[col][y]!=otherColor&&(!(lastEmpty&&pieces[col][y]!=yourColor)));y++){
      if(lastEmpty==true) lastEmpty=false;
      if(pieces[col][y]!=yourColor){
              lastEmpty = true;
        if(getWinner(col,y,false)){
                if(threats[col][y]==0) threats[col][y]=yourColor;
                if(threats[col][y]==(3-yourColor)) threats[col][y]+=yourColor;
              }
            }
    }
    lastEmpty = false;
    for(int x = col-1, y = row - 1;(x>=0)&&(y>=0)&&(pieces[x][y]!=otherColor)&&(!(lastEmpty&&pieces[x][y]!=yourColor));x--,y--){
      if(lastEmpty==true) lastEmpty=false;
      if(pieces[x][y]!=yourColor){
              lastEmpty = true;
        if(getWinner(x,y,false)){
                if(threats[x][y]==0) threats[x][y]=yourColor;
                if(threats[x][y]==(3-yourColor)) threats[x][y]+=yourColor;
              }
            }
    }
    lastEmpty = false;
    for(int x = col+1, y = row + 1;(x<=6)&&(y<=5)&&(pieces[x][y]!=otherColor)&&(!(lastEmpty&&pieces[x][y]!=yourColor));x++,y++){
      if(lastEmpty==true) lastEmpty=false;
      if(pieces[x][y]!=yourColor){
              lastEmpty = true;
        if(getWinner(x,y,false)){
                if(threats[x][y]==0) threats[x][y]=yourColor;
                if(threats[x][y]==(3-yourColor)) threats[x][y]+=yourColor;
              }
            }
    }
    lastEmpty = false;
    for(int x = col-1, y = row + 1;(x>=0)&&(y<=5)&&(pieces[x][y]!=otherColor)&&(!(lastEmpty&&pieces[x][y]!=yourColor));x--,y++){
      if(lastEmpty==true) lastEmpty=false;
      if(pieces[x][y]!=yourColor){
              lastEmpty = true;
        if(getWinner(x,y,false)){
                if(threats[x][y]==0) threats[x][y]=yourColor;
                if(threats[x][y]==(3-yourColor)) threats[x][y]+=yourColor;
              }
            }
    }
    lastEmpty = false;
    for(int x = col+1, y = row - 1;(x<=6)&&(y>=0)&&(pieces[x][y]!=otherColor)&&(!(lastEmpty&&pieces[x][y]!=yourColor));x++,y--){
      if(lastEmpty==true) lastEmpty=false;
      if(pieces[x][y]!=yourColor){
              lastEmpty = true;
        if(getWinner(x,y,false)){
                if(threats[x][y]==0) threats[x][y]=yourColor;
                if(threats[x][y]==(3-yourColor)) threats[x][y]+=yourColor;
              }
            }
    }
    switchTurn();
    yourColor = 3-yourColor;
    otherColor=3-otherColor;
  }
}

bool Game::canPlacePiece(int position){
return (rowFill[position]<6);
}
void Game::placePiece(int position){
  pieces[position][rowFill[position]] = colorToPlace;
  threats[position][rowFill[position]] = 0;
  moves.push_back(position);
  rowFill[position]++;
  colorToPlace = 3-colorToPlace;
}
void Game::removePiece(int position){
  pieces[position][rowFill[position]-1] = 0;
  moves.pop_back();
  rowFill[position]--;
  colorToPlace = 3-colorToPlace;
}
int Game::getLastPiecePlayed(){
  return (3-colorToPlace);
}
int Game::getNextPieceToPlay(){
  return colorToPlace;
}
void Game::print(){
    qDebug() << endl;
  for(int row = 5; row>=0;row--){
    for(int col = 0; col<7;col++){
      qDebug() << pieces[col][row];
    }
    qDebug() << endl;
  }
  qDebug() << endl;
}
void Game::printThreats(){
qDebug() << endl;
  for(int row = 5; row>=0;row--){
    for(int col = 0; col<7;col++){
      qDebug() << threats[col][row];
    }
    qDebug() << endl;
  }
  qDebug() << endl;
}
int Game::getNumMoves(){
  return moves.size();
}
int Game::getObviousMove(){
  int move = -1;
  int ruleFollowed = 100;
  for(int i = 0;i<7;i++){
    if(canPlacePiece(i)){
      placePiece(i);
      if(getWinner(getLastColumnPlayed(),getLastRowPlayed(),false)){
        removePiece(i);
        return i;
      }
      switchTurn();
      if(getWinner(getLastColumnPlayed(),getLastRowPlayed(),false)){
        move = i;
        ruleFollowed=1;
      }
      switchTurn();
      removePiece(i);
    }
  }
  return move;
}
void Game::switchTurn(){  // WARNING: WILL INVALIDATE Game
colorToPlace = 3-colorToPlace;
}

int Game::getNthMove(int n){
  int i = 1;
  QList<int>::iterator itr;
  for( itr = moves.begin();i<n;itr++,i++){}
    return *itr;
}
QString Game::getMoveString(){
    QString retVal = "";
    for(QList<int>::iterator itr = moves.begin();itr != moves.end();itr++){
        retVal+=QString().setNum(*itr+1);
    }
    return retVal;
}

