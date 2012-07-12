// TODO: Wash out all std::vectors (since we decided to use traditional arrays)  (?)

#ifndef CRAZYTETRIS_ENGINE_H
#define CRAZYTETRIS_ENGINE_H

#include <climits>
#include <string>
#include <vector>
#include <bitset>
#include <set>
#include "Strings.h"
#include "Declarations.h"
#include "IOFunctions.h"
#include "VisualEffects.h"

using std::string;
using std::vector;
using std::bitset;
using std::set;



//================================== General ===================================

const MyReal STARTING_SPEED = 1.0f;
// const MyReal SPEED_UP_MULTIPLIER = 1.004f;
// const Time   SPEED_UP_INTERVAL = 0.5f;
const MyReal SPEED_UP_MULTIPLIER = 1.0f;
const Time   SPEED_UP_INTERVAL = 50.0f;
// Speed limit can be excedeed via bonus
const MyReal SPEED_LIMIT = 5.0f;

const Time   NORMAL_LOWERING_TIME = 1.0f;
const Time   LOWERING_ANIMATION_TIME = 0.05f;
// Time necessary for a dropping piece to move one line down
const Time   DROPPING_PIECE_LOWERING_TIME = 0.01f;
const Time   LINE_DISAPPEAR_TIME = 0.1f;
const Time   LINE_COLLAPSE_TIME = 0.1f;

const Time   MIN_BONUS_APPEAR_INTERVAL = 4.0f;
const Time   MAX_BONUS_APPEAR_INTERVAL = 6.0f;
const Time   MIN_BONUS_DISAPPEAR_INTERVAL = 15.0f;
const Time   MAX_BONUS_DISAPPEAR_INTERVAL = 20.0f;

const int    MAX_PLAYERS = 4;
const int    MAX_PLAYER_NAME_LENGTH = 16;



//================================== Keyboard ==================================

const int N_PLAYER_KEYS = 7;

enum PlayerKey { keyLeft, keyRight, keyRotateCW, keyRotateCCW, keyDown, keyDrop, keyChangeVictim };

const Time   MOVE_KEY_REACTIVATION_TIME = 0.12f;
const Time   ROTATE_KEY_REACTIVATION_TIME = 0.18f;
const Time   DOWN_KEY_REACTIVATION_TIME = 0.1f;
const Time   DROP_KEY_REACTIVATION_TIME = 0.3f;
const Time   CHANGE_VICTIM_KEY_REACTIVATION_TIME = 0.2f;

const string PLAYER_KEY_NAME[N_PLAYER_KEYS] =
{
  "Влево: ",
  "Вправо: ",
  "Вращать по ч.: ",
  "Вращать пр. ч.: ",
  "Вниз: ",
  "Бросить: ",
  "Менять цель: "
};

const Time   PLAYER_KEY_REACTIVATION_TIME[N_PLAYER_KEYS] =
{
  MOVE_KEY_REACTIVATION_TIME,
  MOVE_KEY_REACTIVATION_TIME,
  ROTATE_KEY_REACTIVATION_TIME,
  ROTATE_KEY_REACTIVATION_TIME,
  DOWN_KEY_REACTIVATION_TIME,
  DROP_KEY_REACTIVATION_TIME,
  CHANGE_VICTIM_KEY_REACTIVATION_TIME
};



const int N_GLOBAL_KEYS = 0;

enum GlobalKey { };

// const string GLOBAL_KEY_NAME[N_GLOBAL_KEYS] = { };

// const Time   GLOBAL_KEY_REACTIVATION_TIME[N_GLOBAL_KEYS] = { };

const string GLOBAL_KEY_NAME[1] = { "qwerty" };

const Time   GLOBAL_KEY_REACTIVATION_TIME[1] = { 123.0 };



//================================== Bonuses ===================================

enum Bonus
{
  // buffs
  
  // kind soceries
  bnHeal,
  bnSlowDown,
  bnClearField,
  
  // debuffs
  bnFlippedScreen,
  bnInverseControls,
  bnCrazyPieces,
  bnCutBlocks, // name --> (?)
  bnNoHint,
  
  // evil soceries
  bnSpeedUp,
  bnFlipField,
  
  //
  bnNoBonus
};

#define SKIP_BUFFS

#define SKIP_KIND_SOCERIES  case bnHeal:  case bnSlowDown:  case bnClearField:

#define SKIP_DEBFFS         case bnFlippedScreen:  case bnInverseControls:  case bnCrazyPieces: \
                            case bnCutBlocks:  case bnNoHint:

#define SKIP_EVIL_SOCERIES  case bnSpeedUp:  case bnFlipField:

#define SKIP_ALL_BUT_SOCERIES  SKIP_BUFFS  SKIP_DEBFFS  case bnNoBonus:


inline Bonus& operator++(Bonus& bonus) // (?) Isn't there really a better way?
{
  bonus = Bonus(int(bonus) + 1);
  return bonus;
}

const Bonus  FIRST_BONUS = bnHeal;
const Bonus  LAST_BONUS = bnFlipField;
const int    N_BONUSES = LAST_BONUS - FIRST_BONUS + 1;

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

const MyReal BONUS_SPEED_UP_MULTIPLIER = 1.4f;
const MyReal BONUS_SLOW_DOWN_MULTIPLIER = 0.7f;

const Time   BONUS_FLIPPING_SCREEN_DURATION = 0.8f;
const Time   BONUS_CLEAR_SCREEN_DURATION = 0.5f;
const Time   BONUS_CUTTING_BLOCKS_DURATION = 0.5f;
const Time   BONUS_REMOVING_HINT_DURATION = 1.0f;



const Bonus  FIRST_KIND_BONUS   = bnFlippedScreen;
const Bonus  LAST_KIND_BONUS    = bnClearField;

const Bonus  FIRST_EVIL_BONUS   = bnFlippedScreen;
const Bonus  LAST_EVIl_BONUS    = bnFlipField;

const Bonus  FIRST_BUFF         = Bonus(bnHeal + 1);
const Bonus  LAST_BUFF          = bnHeal;
const int    N_BUFFS            = LAST_BUFF - FIRST_BUFF + 1;

const Bonus  FIRST_DEBUFF       = bnFlippedScreen;
const Bonus  LAST_DEBUFF        = bnFlipField;
const int    N_DEBUFFS          = LAST_DEBUFF - FIRST_DEBUFF + 1;

inline bool isKind(Bonus bonus)
{
  return (FIRST_KIND_BONUS <= bonus) && (bonus <= LAST_KIND_BONUS);
}

inline bool isEvil(Bonus bonus)
{
  return (FIRST_EVIL_BONUS <= bonus) && (bonus <= LAST_EVIl_BONUS);
}

inline bool isBuff(Bonus bonus)
{
  return (FIRST_BUFF <= bonus) && (bonus <= LAST_BUFF);
}

inline bool isDebuff(Bonus bonus)
{
  return (FIRST_DEBUFF <= bonus) && (bonus <= LAST_DEBUFF);
}

typedef ShiftedBitSet<N_BUFFS, FIRST_BUFF> Buffs;

typedef ShiftedBitSet<N_DEBUFFS, FIRST_DEBUFF> Debuffs;



//=============================== Pieces & Field ===============================

const int    MAX_PIECE_SIZE = 4;
const int    CENTRAL_PIECE_ROW = MAX_PIECE_SIZE / 2; // (?)
const int    CENTRAL_PIECE_COL = MAX_PIECE_SIZE / 2; // (?)
const int    N_PIECE_ROTATION_STATES = 4;

const int    SKY_HEIGHT = MAX_PIECE_SIZE;
// MAX_PIECE_SIZE / 2  is enough  WALL_WIDTH  in most cases, but that's safe
const int    WALL_WIDTH = MAX_PIECE_SIZE - 1;

struct BlockStructure
{
  vector<FieldCoords> block;
  int lowestBlockRow;
  void afterRead()  // (!) change name
  {
    if (block.empty())
      throw ERR_EMPTY_BLOCK;
    lowestBlockRow = block[0].row;
    for (size_t i = 1; i < block.size(); ++i)
      if (block[i].row < lowestBlockRow)
        lowestBlockRow = block[i].row;
  }
};

struct PieceTemplate
{
  Color color;
  int chance;
  BlockStructure structure[N_PIECE_ROTATION_STATES];
};

struct FieldCell
{
  bool blocked;
  Color color;
  Bonus bonus;
  
  void clear()
  {
    blocked = false;
  }
  
  void setBlock(Color color__, Bonus bonus__ = bnNoBonus)
  {
    blocked = true;
    color = color__;
    bonus = bonus__;
  }
  
  bool isFree() const
  {
    return !blocked;
  }
  
  bool isBlocked() const
  {
    return blocked;
  }
};

struct Field : public Fixed2DArray<FieldCell, -WALL_WIDTH, -WALL_WIDTH,
                                   FIELD_HEIGHT + SKY_HEIGHT, FIELD_WIDTH + WALL_WIDTH>
{
  Field();
  void clear();
};

enum FallingPieceState { psAbsent, psNormal, psDropping };

const char   PIECE_TEMPLATE_BLOCK_CHAR = 'X';
const char   PIECE_TEMPLATE_FREE_CHAR  = '.';



//=================================== Events ===================================

enum EventType
{
  etPieceLowering,
  etLineCollapse,
  etSpeedUp,
  etBonusAppearance,
  etBonusDisappearance
};



struct LineCollapseParameters
{
  int row;
};

union EventParameters
{
  LineCollapseParameters lineCollapse;
};



struct Event
{
  EventType type;
  Time activationTime;
  EventParameters parameters;
  
  Event() { }
  Event(const Event& otherEvent) : type(otherEvent.type), activationTime(otherEvent.activationTime),
                                   parameters(otherEvent.parameters) { }
  Event(EventType type__, Time activationTime__) : type(type__), activationTime(activationTime__) { }
  
  bool operator<(const Event& otherEvent) const
  {
    return activationTime < otherEvent.activationTime;
  }
};



class EventSet
{
public:
  typedef set<Event>::iterator iterator;

  iterator begin()
  {
    return events_.begin();
  }

  iterator end()
  {
    return events_.end();
  }

  void push(const Event& event)
  {
    events_.insert(event);
  }
  
  void push(const EventType eventType, Time activationTime)
  {
    events_.insert(Event(eventType, activationTime));
  }
  
  const Event& top()
  {
    return *events_.begin();
  }
  
  iterator topIterator()
  {
    return events_.begin();
  }
  
  void pop()
  {
    events_.erase(events_.begin());
  }
  
  void erase(iterator it)
  {
    events_.erase(it);
  }
  
  void eraseEventType(EventType eventType)
  {
    for (set<Event>::iterator i = events_.begin(); i != events_.end(); )
    {
      if (i->type == eventType)
        events_.erase(i++);
      else
        ++i;
    }
  }
  
  void clear()
  {
    events_.clear();
  }
  
  bool empty()
  {
    return events_.empty();
  }
  
private:
  set<Event> events_;
};



//================================== Controls ==================================

struct PlayerKeyList
{
  KeyboardKey keyLeft;
  KeyboardKey keyRight;
  KeyboardKey keyRotateCW;
  KeyboardKey keyRotateCCW;
  KeyboardKey keyDown;
  KeyboardKey keyDrop;
  KeyboardKey keyChangeVictim;
};

union Controls
{
  PlayerKeyList keyByName;
  KeyboardKey keyArray[N_PLAYER_KEYS];
};



struct GlobalKeyList { };

union GlobalControls
{
  GlobalKeyList keyByName;
//  KeyboardKey keyArray[N_GLOBAL_KEYS];
  KeyboardKey keyArray[1];
};



//=================================== Player ===================================

class Game;

class AccountInfo
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
  int accountNumber;  // TODO: changeSystem, use some ID's insted of numbers
  Game* game;
  
  bool participates;
  bool active;
  int score;
  Controls controls;
  
  MyReal speed;
  Field field;
  Time latestLineCollapse;
  
//   int nextPieceNumber;
  const PieceTemplate* nextPiece;
  int nextPieceRotationState;
  
  FallingPieceState fallingPieceState;
//   int fallingPieceNumber;
  const PieceTemplate* fallingPiece;
  int fallingPieceRotationState;
  // ``center'' coordinates
  FieldCoords fallingPiecePosition;
  
//   Time nextBonusTime;
  
  Buffs buffs;
  Debuffs debuffs;
  int victimNumber;
  
  EventSet events;
  
  int nBlockImages;
  BlockImage blockImage[MAX_BLOCKS];
  int nDisappearingLines;
  DisappearingLine disappearingLine[FIELD_HEIGHT];
  VisualEffects visualEffects;
  
  Time nextKeyActivation[N_PLAYER_KEYS];

  void init(Game* game__, int number__);
  void loadAccountInfo(int newAccount);
  AccountInfo* account();
  void prepareForNewMatch();
  void prepareForNewRound();
  
  Time currentTime();
  Time pieceLoweringInterval();
  
  Player* victim();
  
  void takesBonus(Bonus bonus);
  void applyBonus(Bonus bonus);
  void heal();
  void kill();

  void onKeyPress(PlayerKey key);  // (!) private
  void onTimer();
  void redraw();  // (!) private
  
private:
  const BlockStructure& fallingBlockStructure();
  bool canDisposePiece(FieldCoords position, const BlockStructure& piece) const;
//   void setUpPiece(FieldCoords position, const BlockStructure& piece, const Color& color)
  void setUpPiece();
  void sendNewPiece();
  void lowerPiece();
  void removeFullLines();
  void collapseLine(int row);
  void movePiece(int direction);
  void dropPiece();
  void rotatePiece(int direction);
  void cycleVictim();
  
  void enableBonusEffect(Bonus bonus);
  void disableBonusEffect(Bonus bonus);
  void flipBlocks();
};



//==================================== Game ====================================

class Game
{
public:
  vector<AccountInfo> account;

  Player player[MAX_PLAYERS];
  int nActivePlayers;
  
  vector<PieceTemplate> pieceTemplate;
  vector<int> randomPieceTable;
  
  Time currentTime;
//   Time lastSpeedUp;
  
//   Time nextGlobalKeyActivation[N_GLOBAL_KEYS];
  Time nextGlobalKeyActivation[1];
  GlobalControls globalControls;
  
  void init();
  
  void saveAccounts();
  void saveSettings();

  void newMatch();
  void newRound(Time currentTime__);
  void endRound();
  
  void onGlobalKeyPress(GlobalKey key);  // (!) private
  void onTimer(Time currentTime);
  
private:
  void loadPieces();
  void loadAccounts();
  void loadDefaultAccounts();
  void loadSettings();
  void loadDefaultSettings();
};

#endif
