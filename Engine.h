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

const double STARTING_SPEED = 1.0;
const double SPEED_UP_MULTIPLIER = 1.004;
const Time   SPEED_UP_INTERVAL = 0.5f;
// Speed limit can be excedeed via bonus
const double SPEED_LIMIT = 5.0;

const Time   NORMAL_LOWERING_TIME = 0.5f;
// Time necessary for a dropping piece to move one line down
const Time   DROPPING_PIECE_SPEED = 0.01f;
const Time   LINE_DISAPPEAR_TIME = 0.05f;

const Time   MIN_BONUS_APPEAR_INTERVAL = 4.0f;
const Time   MAX_BONUS_APPEAR_INTERVAL = 6.0f;
const Time   MIN_BONUS_DISAPPEAR_INTERVAL = 15.0f;
const Time   MAX_BONUS_DISAPPEAR_INTERVAL = 20.0f;

const int    MAX_PLAYERS = 4;
const int    MAX_PLAYER_NAME_LENGTH = 16;



//================================== Keyboard ==================================

const Time   MOVE_KEY_REACTIVATE_TIME = 0.12f;
const Time   ROTATE_KEY_REACTIVATE_TIME = 0.18f;
const Time   DOWN_KEY_REACTIVATE_TIME = 0.1f;
const Time   DROP_KEY_REACTIVATE_TIME = 0.3f;
// const Time   HEAL_KEY_REACTIVATE_TIME = 0.2f;
const Time   CHANGE_VICTIM_KEY_REACTIVATE_TIME = 0.2f;

const string CONTROL_KEY_NAME[N_CONTROL_KEYS] =
{
  "Влево: ",
  "Вправо: ",
  "Вращать по ч.: ",
  "Вращать пр. ч.: ",
  "Вниз: ",
  "Бросить: ",
//   "Лечиться: ",
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
//   HEAL_KEY_REACTIVATE_TIME,
  CHANGE_VICTIM_KEY_REACTIVATE_TIME
};



//================================== Bonuses ===================================

const int    N_BONUSES = 10;

enum Bonus
{
  // Kind buffs
  
  // Kind soceries
  bnHeal,
  bnSlowDown,
  bnClearField,
  
  // Evil buffs
  bnFlippedScreen,
  bnInverseControls,
  bnCrazyPieces,
  bnCutBlocks, // name --> (?)
  bnNoHint,
  
  // Evil soceries
  bnSpeedUp,
  bnFlipField
};
   
const string BONUS_NAME[N_BONUSES] =
{
  "Heal",
  "SlowDown",
  "ClearField",
  "FlippedScreen",
  "InverseControls",
  "CrazyPieces",
  "CutBlocks",
  "NoHint",
  "SpeedUp",
  "FlipField"
};

const int    BONUS_CHANCE[N_BONUSES] =
{
  5, // bnHeal
  2, // bnSlowDown
  1, // bnClearField
  2, // bnFlippedScreen
  2, // bnInverseControls
  2, // bnCrazyPieces
  2, // bnCutBlocks
  2, // bnNoHint
  2, // bnSpeedUp
  2  // bnFlipField
};

const double BONUS_SPEED_UP_MULTIPLIER = 1.4;
const double BONUS_SLOW_DOWN_MULTIPLIER = 0.7;

const Time   BONUS_FLIPPING_SCREEN_DURATION = 0.8f;
const Time   BONUS_CLEAR_SCREEN_DURATION = 0.5f;
const Time   BONUS_CUTTING_BLOCKS_DURATION = 0.5f;
const Time   BONUS_REMOVING_HINT_DURATION = 1.0f;



const Bonus  BEGIN_KIND         = bnFlippedScreen;
const Bonus  END_KIND           = Bonus(bnClearField + 1);

const Bonus  BEGIN_EVIL         = bnFlippedScreen;
const Bonus  END_EVIl           = Bonus(bnFlipField + 1);

const Bonus  BEGIN_BUFFS        = bnHeal;
const Bonus  END_BUFFS          = bnHeal;
const int    N_BUFFS            = END_BUFFS - BEGIN_BUFFS;

const Bonus  BEGIN_DEBUFFS      = bnFlippedScreen;
const Bonus  END_DEBUFFS        = Bonus(bnFlipField + 1);
const int    N_DEBUFFS          = END_DEBUFFS - BEGIN_DEBUFFS;

inline bool isKind(Bonus bonus)
{
  return (BEGIN_KIND <= bonus) && (bonus < END_KIND);
}

inline bool isEvil(Bonus bonus)
{
  return (BEGIN_EVIL <= bonus) && (bonus < END_EVIl);
}

inline bool isBuff(Bonus bonus)
{
  return (BEGIN_BUFFS <= bonus) && (bonus < END_BUFFS);
}

inline bool isDebuff(Bonus bonus)
{
  return (BEGIN_DEBUFFS <= bonus) && (bonus < END_DEBUFFS);
}

typedef ShiftedBitSet<N_BUFFS, BEGIN_BUFFS> Buffs;

typedef ShiftedBitSet<N_DEBUFFS, BEGIN_DEBUFFS> Debuffs;



//==================================== Game ====================================

const int    MAX_PIECE_SIZE = 4;
const int    N_ROTATION_STATES = 4;

typedef FixedSizeTable<bool, MAX_PIECE_SIZE, MAX_PIECE_SIZE> BlockStructre;

class PieceTemplate
{
public:
  Color color;
  BlockStructre& operator[](int index)
  {
    return structure_[index];
  }
  const BlockStructre& operator[](int index) const {
    return structure_[index];
  }
protected:
  BlockStructre structure_[N_ROTATION_STATES];
};

enum FieldCellType { ctEmpty, ctSky, ctBlock, ctWall };

struct FieldCell
{
  FieldCellType cellType;
  Color color;
  Bonus bonus;
  
  bool isFree()
  {
    return (cellType == ctEmpty) || (cellType == ctSky);
  }
  bool isBlocked()
  {
    return !isFree();
  }
};

typedef FixedSizeTable<FieldCell, FIELD_WIDTH, FIELD_HEIGHT> Field;

const char   PIECE_TEMPLATE_BLOCK_CHAR = '*';
const char   PIECE_TEMPLATE_FREE_CHAR  = '-';



struct ControlKeyList
{
  RealKey keyLeft;
  RealKey keyRight;
  RealKey keyRotateCW;
  RealKey keyRotateCCW;
  RealKey keyDown;
  RealKey keyDrop;
//   RealKey keyHeal;
  RealKey keyChangeVictim;
};

union Controls    // (?) Is it necessary?
{
  ControlKeyList keyList;
  RealKey keyArray[N_CONTROL_KEYS];
};

struct KeyMapping
{
  ControlKey controlKey;
  int iPlayer;
};



class Game;

class PlayerInfo
{
public:
  string name;
//   int totalWins;
//   int totalLosts;
  // TODO: other stats
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
  FieldCoords fallingPiecePos;
  
  Time nextBonusTime;
  
  Buffs buffs;
  Debuffs debuffs;
  Player* victim;
  
  VisualObjects visualObjects;
  VisualEffects visualEffects;
  
  void init(Game* game__, int number__);
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
  void generateNewPiece();
  void lowerPiece();
  void checkFullLines();
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
  int lastWinner;
  Time lastSpeedUp;
  Time startTime;
  Player player[MAX_PLAYERS];
  
  std::vector<PieceTemplate> pieceTemplate;
  int nPieceTemplates;
  int nNormalPieceTemplates;
  
  KeyMapping keyMap[N_REAL_KEYS];
  
  Game();
  
  void newRound();
  void endRound();
  
  void onKeyPress(RealKey key);
  void onTimer();
  
private:
  void buildKeyMap();
};

#endif
