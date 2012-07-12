#ifndef CRAZYTETRIS_ENGINE_H
#define CRAZYTETRIS_ENGINE_H

#include <climits>
#include <string>
#include <vector>
#include <bitset>
#include "Declarations.h"
#include "Strings.h"

using std::string;
using std::vector;
using std::bitset;



//================================== General ===================================

const int    FIELD_WIDTH = 10;
const int    FIELD_HEIGHT = 20;

const double STARTING_SPEED = 1.0;
const double SPEED_UP_MULTIPLIER = 1.004;
const Time   SPEED_UP_INTERVAL = 500;
// Speed limit can be excedeed via bonus
const double SPEED_LIMIT = 5.0;

const Time   NORMAL_LOWERING_TIME = 500;
// Time necessary for a dropping piece to move one line down
const Time   DROPPING_PIECE_SPEED = 7;
const Time   LINE_DISAPPEAR_TIME = 10;

const Time   MIN_BONUS_APPEAR_INTERVAL = 4000;
const Time   MAX_BONUS_APPEAR_INTERVAL = 6000;
const Time   MIN_BONUS_DISAPPEAR_INTERVAL = 15000;
const Time   MAX_BONUS_DISAPPEAR_INTERVAL = 20000;

const int    MAX_PLAYERS = 4;
const int    MAX_PLAYER_NAME_LENGTH = 16;



//================================== Keyboard ==================================

const int    N_CONTROL_KEYS = 8;

const Time   MOVE_KEY_REACTIVATE_TIME = 120;
const Time   ROTATE_KEY_REACTIVATE_TIME = 180;
const Time   DOWN_KEY_REACTIVATE_TIME = 100;
const Time   DROP_KEY_REACTIVATE_TIME = 300;
const Time   HEAL_KEY_REACTIVATE_TIME = 200;
const Time   CHANGE_VICTIM_KEY_REACTIVATE_TIME = 200;

const string CONTROL_KEY_NAME[N_CONTROL_KEYS] = {
  "Влево: ",
  "Вправо: ",
  "Вращать по ч.: ",
  "Вращать пр. ч.: ",
  "Вниз: ",
  "Бросить: ",
  "Лечиться: ",
  "Менять цель: "
};

const Time   KEY_REACTIVATE_TIME[N_CONTROL_KEYS] = {
  MOVE_KEY_REACTIVATE_TIME,
  MOVE_KEY_REACTIVATE_TIME,
  ROTATE_KEY_REACTIVATE_TIME,
  ROTATE_KEY_REACTIVATE_TIME,
  DOWN_KEY_REACTIVATE_TIME,
  DROP_KEY_REACTIVATE_TIME,
  HEAL_KEY_REACTIVATE_TIME,
  CHANGE_VICTIM_KEY_REACTIVATE_TIME
};



//================================== Bonuses ===================================

const int    N_BONUSES = 10;

enum Bonus {
  bnHeal,
  bnFlipScreen,
  bnInverseControls,
//   bnCrazyPieces,
  bnTransparentBlocks,
  bnForeground,
  bnNoHint,
  bnSpeedUp,
  bnSlowDown,
//   bnChangeBackground,
  bnClearGlass,
  bnFlipField,
};
   
const string BONUS_NAME[N_BONUSES] = {
  "Heal",
  "FlipScreen",
  "InverseControls",
//   "CrazyPieces",
  "TransparentBlocks",
  "Foreground",
  "NoHint",
  "SpeedUp",
  "SlowDown",
//   "ChangeBackground",
  "ClearField",
  "FlipField"
};

const int    BONUS_CHANCE[N_BONUSES] = {
  5,
  2,
  2,
//   2,
  2,
  2,
  2,
  2,
  2,
//   2,
  1,
  2
};

const double BONUS_SPEED_UP_MULTIPLIER = 1.4;
const double BONUS_SLOW_DOWN_MULTIPLIER = 0.7;

const Time   BONUS_FLIP_SCREEN_TIME = 800;
const Time   BONUS_CLEAR_SCREEN_TIME = 500;

const Bonus  FIRST_DISEASE = bnFlipScreen;
const Bonus  LAST_DISEASE  = bnNoHint;
const int    N_DISEASES    = LAST_DISEASE - FIRST_DISEASE + 1;

class Diseases : private bitset<N_DISEASES>
{
public:
  bool operator[](size_t pos) const {
    return bitset<N_DISEASES>::operator[](pos);
  }
  reference operator[](size_t pos) {
    return bitset<N_DISEASES>::operator[](pos);
  }
  void clear() {
    reset();
  }
};



//==================================== Game ====================================

int MAX_PIECE_SIZE = 4;
int N_ROTATION_STATES = 4;

typedef Table<bool, MAX_PIECE_SIZE, MAX_PIECE_SIZE> BlockStructre; // (?) is it slower with bool?

class PieceTemplate {
public:
  Color color;
  Piece() {
    structure_.resize(N_ROTATION_STATES);
  }
  BlockStructre& operator[](int index) {
    return structure_[index];
  }
  const BlockStructre& operator[](int index) const {
    return structure_[index];
  }
protected:
  vector<BlockStructre> structure_;
};

enum FieldCellType { ctEmpty, ctSky, ctBlock, ctWall };

struct FieldCell {
  FieldCellType cellType;
  Color color;
  Bonus bonus;
  
  bool free() {
    return (cellType == ctEmpty) || (cellType == ctSky);
  }
  bool blocked() {
    return !free();
  }
};

typedef Table<FieldCell, FIELD_WIDTH, FIELD_HEIGHT> Field;

const char   PIECE_TEMPLATE_BLOCK_CHAR = '*';
const char   PIECE_TEMPLATE_FREE_CHAR  = '-';



class PlayerGameSpace {
public:
  double speed;
  Field field;
  Player* player();
  
//   int nextPieceNumber;
  PieceTemplate* nextPiece;
  int nextPieceRotationState;
  
  Time pieceLoweringInterval();
//   int fallingPieceNumber;
  PieceTemplate* fallingPiece;
  int fallingPieceRotationState;
  // Left top corner coordinates
  FieldCoords fallingPiecePos;
  
  Time nextBonusTime;
  
  void movePieceLeft();
  void movePieceRight();
  void movePieceDown();
  void dropPiece();
  void rotatePieceCW();
  void rotatePieceCCW();
  
  void onKeyPress();
  void onTimer();
  
private:
  void generateNewPiece();
  void lowerPiece();
  void checkFullLines();
  void setNextBonusAppearTime();
  void setNextBonusDisappearTime();
  void refreshBonus();
  void flipBlocks();
};



struct ControlKeyList {
  ControlKey keyLeft, keyRight;
  ControlKey keyRotateCW, keyRotateCCW;
  ControlKey keyDown;
  ControlKey keyDrop;
  ControlKey keyHeal;
  ControlKey keyChangeTarget;
};

union Controls {
  ControlKeyList keyList;
  ControlKey keyArray[N_CONTROL_KEY];
};



class PlayerInfo {
public:
  int score;
  string name;
  Controls controls;
};



class Player {
public:
  Player(Game* game, int number);
  
  Game* game;
  PlayerGameSpace gameSpace;
  PlayerInfo info;

  int number;
  bool participates;
  bool active;
  
  Diseases diseases;
  
  Player* victim;
  void cycleVictim();
  
  void takesBonus(Bonus bonus);
  void applyBonus(Bonus bonus);
  void kill();
  
  void onKeyPress();
  void onTimer();
};



class Game {
public:
  int lastWinner;
  Time lastSpeedUp;
  Time startTime;
  Player player[MAX_PLAYERS];
  
  vector<PieceTemplate> pieceTemplate;
  int nPieceTemplates;
  int nNormalPieceTemplates;
  
  void Game();
  void newRound();
  void endRound();

  void onKeyPress();
  void onTimer();
};

#endif
