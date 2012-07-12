// TODO: Add range checking everywhere

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

const float  STARTING_SPEED = 1.0f;
const float  SPEED_UP_MULTIPLIER = 1.05f;
const Time   SPEED_UP_INTERVAL = 1.0f;
// Speed limit can be excedeed via bonus
const float  SPEED_LIMIT = 5.0f;

const Time   NORMAL_LOWERING_TIME = 0.8f;
// Time necessary for a dropping piece to move one line down
const Time   DROPPING_PIECE_LOWERING_TIME = 0.01f;
const Time   LINE_DISAPPEAR_TIME = 0.1f;
const Time   LINE_COLLAPSE_TIME = 0.1f;

const Time   PIECE_LOWERING_ANIMATION_TIME = 0.05f;
const Time   LINE_COLLAPSE_ANIMATION_TIME = 0.05f;
const Time   PIECE_MOVING_ANIMATION_TIME = 0.05f;
const Time   PIECE_ROTATING_ANIMATION_TIME = 0.05f;

const Time   MIN_BONUS_APPEAR_INTERVAL = 4.0f;
const Time   MAX_BONUS_APPEAR_INTERVAL = 6.0f;
const Time   MIN_BONUS_DISAPPEAR_INTERVAL = 15.0f;
const Time   MAX_BONUS_DISAPPEAR_INTERVAL = 20.0f;

const int    MAX_PLAYERS = 4;
const int    MAX_PLAYER_NAME_LENGTH = 16;



//================================== Keyboard ==================================

const int N_PLAYER_KEYS = 7;

enum PlayerKey { keyLeft, keyRight, keyRotateCCW, keyRotateCW, keyDown, keyDrop, keyChangeVictim };

const Time   MOVE_KEY_REACTIVATION_TIME = 0.12f;
const Time   ROTATE_KEY_REACTIVATION_TIME = 0.18f;
const Time   DOWN_KEY_REACTIVATION_TIME = 0.08f;
const Time   DROP_KEY_REACTIVATION_TIME = 0.3f;
const Time   CHANGE_VICTIM_KEY_REACTIVATION_TIME = 0.2f;

const string PLAYER_KEY_NAME[N_PLAYER_KEYS] =
{
  "Влево: ",
  "Вправо: ",
  "Вращать пр. ч.: ",
  "Вращать по ч.: ",
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



struct PlayerKeyList
{
  KeyboardKey keyLeft;
  KeyboardKey keyRight;
  KeyboardKey keyRotateCCW;
  KeyboardKey keyRotateCW;
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

const float  BONUS_SPEED_UP_MULTIPLIER = 1.4f;
const float  BONUS_SLOW_DOWN_MULTIPLIER = 0.7f;

const Time   BONUS_FLIPPING_SCREEN_DURATION = 0.8f;
const Time   BONUS_CLEAR_SCREEN_DURATION = 0.5f;
const Time   BONUS_CUTTING_BLOCKS_DURATION = 0.5f;
const Time   BONUS_REMOVING_HINT_DURATION = 1.0f;
const Time   BONUS_LANTERN_ANIMATION_TIME = PIECE_LOWERING_ANIMATION_TIME;



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
const int    CENTRAL_PIECE_ROW = (MAX_PIECE_SIZE - 1) / 2; // is it necessary (?)
const int    CENTRAL_PIECE_COL = (MAX_PIECE_SIZE - 1) / 2; // is it necessary (?)
const int    N_PIECE_ROTATION_STATES = 4;

const int    SKY_HEIGHT = MAX_PIECE_SIZE;
// MAX_PIECE_SIZE / 2  is enough for  WALL_WIDTH  in most cases, but that's safe
const int    WALL_WIDTH = MAX_PIECE_SIZE - 1;

// TODO: use in Field::Field()
const int    BORDERED_FIELD_ROW_BEGIN = -WALL_WIDTH;
const int    BORDERED_FIELD_ROW_END   = FIELD_HEIGHT + SKY_HEIGHT;
const int    BORDERED_FIELD_COL_BEGIN = -WALL_WIDTH;
const int    BORDERED_FIELD_COL_END   = FIELD_WIDTH + WALL_WIDTH;

const int    NO_BLOCK_IMAGE = -1;
const int    NO_CHANGE = -2;

struct BlockStructure
{
  vector<FieldCoords> blocks;
  int lowestBlockRow;
  void afterRead()  // (!) change name
  {
    if (blocks.empty())
      throw ERR_EMPTY_BLOCK;
    lowestBlockRow = blocks[0].row;
    for (size_t i = 1; i < blocks.size(); ++i)
      if (blocks[i].row < lowestBlockRow)
        lowestBlockRow = blocks[i].row;
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
  int iBlockImage;
  int iNewBlockImage;
  
  void assign(const FieldCell& a)
  {
    blocked = a.blocked;
    color = a.color;
    bonus = a.bonus;
  }

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

private:
  // prevent full copy
  FieldCell& operator=(const FieldCell& a);
};

struct Field : public Fixed2DArray<FieldCell, -WALL_WIDTH, -WALL_WIDTH,
                                   FIELD_HEIGHT + SKY_HEIGHT, FIELD_WIDTH + WALL_WIDTH>
{
  Field();

  FieldCell& operator()(int row, int col)
  {
    return Fixed2DArray<FieldCell, -WALL_WIDTH, -WALL_WIDTH,
                        FIELD_HEIGHT + SKY_HEIGHT, FIELD_WIDTH + WALL_WIDTH>::
                        operator()(row, col);
  }
  
  const FieldCell& operator()(int row, int col) const
  {
    return Fixed2DArray<FieldCell, -WALL_WIDTH, -WALL_WIDTH,
                        FIELD_HEIGHT + SKY_HEIGHT, FIELD_WIDTH + WALL_WIDTH>::
                        operator()(row, col);
  }

  FieldCell& operator()(FieldCoords position)
  {
    return operator()(position.row, position.col);
  }

  const FieldCell& operator()(FieldCoords position) const
  {
    return operator()(position.row, position.col);
  }

  void clear();

  void clearImageIndices();
};

enum FallingPieceState { psAbsent, psNormal, psDropping };

//const char   PIECE_TEMPLATE_BLOCK_CHAR = 'X';
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

// Initialize at:  [N]ever, on [C]reation, new [M]atch, new [R]ound, game [S]ettings change
class Player
{
public:
  int           number;         // C
  int           accountNumber;  // S  // TODO: changeSystem, use some ID's instead of numbers
  Game*         game;           // C
  
  bool          participates;   // S
  bool          active;         // R  (in Game::newRound)
  int           score;          // M
  Controls      controls;       // S
  
  float         speed;          // R
  Field         field;          // C/R
  Time          latestLineCollapse; // R
  
  const PieceTemplate*  nextPiece;                // R
  int                   nextPieceRotationState;   // R
  
  FallingPieceState     fallingPieceState;        // R
  const PieceTemplate*  fallingPiece;             // R
  int                   fallingPieceRotationState; // R
  FieldCoords           fallingPiecePosition;     // R    [``center'' coordinates]
  
  Buffs         buffs;          // R
  Debuffs       debuffs;        // R
  int           victimNumber;   // R
  
  EventSet      events;         // R
  
  vector<BlockImage>        blockImages;          // N
  vector<DisappearingLine>  disappearingLines;    // R
  VisualEffects             visualEffects;        // R
  
  Time          nextKeyActivationTable[N_PLAYER_KEYS]; // C

  void          init(Game* game__, int number__);
  void          loadAccountInfo(int newAccount);
  AccountInfo*  account();
  void          prepareForNewMatch();
  void          prepareForNewRound();
  
  Time          currentTime();
  Time          pieceLoweringInterval();
  
  Player*       victim();
  
  void          takesBonus(Bonus bonus);
  void          applyBonus(Bonus bonus);
  void          heal();
  void          kill();

  void          onKeyPress(PlayerKey key);
  void          onTimer();
  void          redraw();
  
private:
  const BlockStructure& fallingBlockStructure();
  bool          canDisposePiece(FieldCoords position, const BlockStructure& piece) const;
  void          setUpPiece();
  void          sendNewPiece();
  void          lowerPiece();
  void          removeFullLines();
  void          collapseLine(int row);
  void          movePiece(int direction);
  void          dropPiece();
  void          rotatePiece(int direction);
  
  void          applyBlockImagesMovements();
  void          addStandingBlockImage(Color color, FieldCoords position);
  void          addMovingBlockImage(Color color, FieldCoords movingFrom, FieldCoords movingTo,
                                    Time movingStartTime, Time movingDuration);
  void          setUpBlockImage(Color color, FieldCoords position);
  void          moveBlockImage(FieldCoords movingFrom, FieldCoords movingTo,
                               Time movingStartTime, Time movingDuration);
  void          removeBlockImage(FieldCoords position);

  void          cycleVictim();
  
  void          enableBonusEffect(Bonus bonus);
  void          disableBonusEffect(Bonus bonus);
  void          flipBlocks();
};



//==================================== Game ====================================

class Game
{
public:
  vector<AccountInfo> accounts;

  Player        players[MAX_PLAYERS];
  vector<Player*> activePlayers;
  //int nActivePlayers;
  
  vector<PieceTemplate> pieceTemplates;
  vector<int>   randomPieceTable;
  
  Time          currentTime;
  
//  Time          nextGlobalKeyActivationTable[N_GLOBAL_KEYS];
  Time          nextGlobalKeyActivationTable[1];
  GlobalControls globalControls;
  
  void          init();
  
  void          saveAccounts();
  void          saveSettings();

  void          newMatch();
  void          newRound(Time currentTime__);
  void          endRound();
  
  void          onGlobalKeyPress(GlobalKey key);
  void          onTimer(Time currentTime);
  
private:
  void          loadPieces();
  void          loadAccounts();
  void          loadDefaultAccounts();
  void          loadSettings();
  void          loadDefaultSettings();
};

#endif
