#ifndef CRAZYTETRIS_ENGINE_H
#define CRAZYTETRIS_ENGINE_H

#include <climits>
#include <string>
#include <vector>
#include <bitset>
#include "Strings.h"
#include "Declarations.h"
#include "IOFunctions.h"
#include "VisualEffects.h"

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
const Time   DROPPING_PIECE_LOWERING_TIME = 7;
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

const string CONTROL_KEY_NAME[N_CONTROL_KEYS] =
{
  "Влево: ",
  "Вправо: ",
  "Вращать по ч.: ",
  "Вращать пр. ч.: ",
  "Вниз: ",
  "Бросить: ",
  "Лечиться: ",
  "Менять цель: "
};

const Time   KEY_REACTIVATE_TIME[N_CONTROL_KEYS] =
{
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

enum Bonus
{
  bnFlippedScreen,
  bnInverseControls,
  bnCrazyPieces,
  bnCutBlocks, // name --> (?)
  bnNoHint,
  bnHeal,
  bnSpeedUp,
  bnSlowDown,
  bnClearField,
  bnFlipField,
};
   
const string BONUS_NAME[N_BONUSES] =
{
  "FlippedScreen",
  "InverseControls",
  "CrazyPieces",
  "CutBlocks",
  "NoHint",
  "Heal",
  "SpeedUp",
  "SlowDown",
  "ClearField",
  "FlipField"
};

const int    BONUS_CHANCE[N_BONUSES] =
{
  2, // bnFlippedScreen
  2, // bnInverseControls
  2, // bnCrazyPieces
  2, // bnCutBlocks
  2, // bnNoHint
  5, // bnHeal
  2, // bnSpeedUp
  2, // bnSlowDown
  1, // bnClearField
  2  // bnFlipField
};

const double BONUS_SPEED_UP_MULTIPLIER = 1.4;
const double BONUS_SLOW_DOWN_MULTIPLIER = 0.7;

const Time   BONUS_FLIP_SCREEN_TIME = 800;
const Time   BONUS_CLEAR_SCREEN_TIME = 500;

const Bonus  BEGIN_ENCHANTMENT = bnFlippedScreen;
const Bonus  END_ENCHANTMENT   = bnNoHint + 1;
const int    N_ENCHANTMENTS    = END_ENCHANTMENT - BEGIN_ENCHANTMENT;

const Bonus  BEGIN_BUFF        = BEGIN_ENCHANTMENT;
const Bonus  END_BUFF          = bnFlippedScreen;

const Bonus  BEGIN_DEBUFF      = END_BUFF;
const Bonus  END_DEBUFF        = END_ENCHANTMENT;

bool isEnchantment(Bonus bonus)
{
  return (BEGIN_ENCHANTMENT <= bonus) && (bonus < END_ENCHANTMENT);
}

bool isBuff(Bonus bonus) // spelling (?)
{
  return (BEGIN_BUFF <= bonus) && (bonus < END_BUFF);
}

bool isDebuff(Bonus bonus)
{
  return (BEGIN_DEBUFF <= bonus) && (bonus < END_DEBUFF);
}

class Enchantment : private bitset<N_ENCHANTMENTS>
{
public:
  bool operator[](size_t pos) const
  {
    return bitset<N_ENCHANTMENTS>::operator[](pos);
  }
  reference operator[](size_t pos)
  {
    return bitset<N_ENCHANTMENTS>::operator[](pos);
  }
  void clear()
  {
    reset();
  }
};



//==================================== Game ====================================

const int    MAX_PIECE_SIZE = 4;
const int    N_PIECE_ROTATION_STATES = 4;

typedef Table<bool, MAX_PIECE_SIZE, MAX_PIECE_SIZE> BlockStructre; // (?) is it slower with bool?

class PieceTemplate
{
public:
  Color color;
  Piece()
  {
    structure_.resize(N_ROTATION_STATES);
  }
  BlockStructre& operator[](int index)
  {
    return structure_[index];
  }
  const BlockStructre& operator[](int index) const {
    return structure_[index];
  }
protected:
  vector<BlockStructre> structure_;
};

enum FieldCellType { ctEmpty, ctSky, ctBlock, ctWall };

struct FieldCell
{
  FieldCellType cellType;
  Color color;
  Bonus bonus;
  
  bool free()
  {
    return (cellType == ctEmpty) || (cellType == ctSky);
  }
  bool blocked()
  {
    return !free();
  }
};

typedef Table<FieldCell, FIELD_WIDTH, FIELD_HEIGHT> Field;

const char   PIECE_TEMPLATE_BLOCK_CHAR = '*';
const char   PIECE_TEMPLATE_FREE_CHAR  = '-';



struct ControlKeyList
{
  ControlKey keyLeft, keyRight;
  ControlKey keyRotateCW, keyRotateCCW;
  ControlKey keyDown;
  ControlKey keyDrop;
//   ControlKey keyHeal;
  ControlKey keyChangeVictim;
};

union Controls
{
  ControlKeyList keyList;
  ControlKey keyArray[N_CONTROL_KEY];
};



class PlayerInfo
{
public:
  string name;
  int totalWins; // (?)
  int totalLosts; // (?)
};

class Player
{
public:
  int number;
  PlayerInfo* info;
  Game* game;
  
  bool participates;
  bool active;
  int score;
  
  double speed;
  Field field;
  Controls controls;
  
//   int nextPieceNumber;
  PieceTemplate* nextPiece;
  int nextPieceRotationState;
  
//   int fallingPieceNumber;
  PieceTemplate* fallingPiece;
  int fallingPieceRotationState;
  // Left top corner coordinates
  FieldCoords fallingPiecePosition;
  
  Time nextBonusTime;
  
  Diseases diseases;
  Player* victim;
  
  VisualEffects effects;
  
  void init(Game* game, int number);
  void loadPlayerInfo(PlayerInfo* playerInfo);
  void prepareForNewRound();
  
  Time pieceLoweringInterval();
  void takesBonus(Bonus bonus);
  void applyBonus(Bonus bonus);
  void heal();
  void kill();

  void onKeyPress(ControlKey key);
  void onTimer();
  
private:
  void sendNewPiece();
  void lowerPiece();
  void removeFullLines();
  // 1 -- right, -1 -- left
  void movePiece(int direction);
  void dropPiece();
  // 1 -- CCW, -1 -- CW
  void rotatePiece(int direction);
  void cycleVictim();
  
  void setNextBonusAppearTime();
  void setNextBonusDisappearTime();
  void refreshBonus();
  
  void enableBonusEffect(Bonus bonus);
  void disableBonusEffect(Bonus bonus);
  void flipBlocks();
};



class Game
{
public:
  void Game();
  
  int lastWinner;
  Time lastSpeedUp;
  Time startTime;
  vector<Player> player;
  
  vector<PieceTemplate> pieceTemplate;
  int nPieceTemplates;
  int nNormalPieceTemplates;
  
  void newRound();
  void endRound();
  
  void onKeyPress();
  void onTimer();
};

#endif
