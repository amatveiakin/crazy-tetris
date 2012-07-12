#ifndef CRAZYTETRIS_VISUALEFFECTS_H
#define CRAZYTETRIS_VISUALEFFECTS_H

#include <vector>
#include <list>
#include "Declarations.h"

//================================== General ===================================

class MovingObject {
public:
  FloatFieldCoords movingFrom;
  FloatFieldCoords movingTo;
  Time movingStartTime;
  Time movingDuration;
  
  void setMotion(FloatFieldCoords movingFrom__, FloatFieldCoords movingTo__,
                 Time movingStartTime__, Time movingDuration__)
  {
    movingFrom = movingFrom__;
    movingTo = movingTo__;
    movingStartTime = movingStartTime__;
    movingDuration = movingDuration__;
  }
  
  void moveTo(FloatFieldCoords movingTo__, Time currentTime, Time movingDuration__)
  {
    movingFrom = position(currentTime);
    movingTo = movingTo__;
    movingStartTime = currentTime;
    movingDuration = movingDuration__;
  }
  
  void placeAt(FloatFieldCoords position)
  {
    setMotion(position, position, 0.0, 1.0);
  }
  
  FloatFieldCoords position(Time currentTime)
  {
    if (currentTime < movingStartTime)
    {
      return movingFrom;
    }
    else if (currentTime < movingStartTime + movingDuration)
    {
      return (movingFrom * (movingStartTime + movingDuration - currentTime) +
              movingTo   * (currentTime - movingStartTime)) /
             movingDuration;
    }
    else
    {
      return movingTo;
    }
  }

  float positionX(Time currentTime)
  {
    return position(currentTime).col;
  }

  float positionY(Time currentTime)
  {
    return position(currentTime).row;
  }

  // TODO: Test whether VS' optimizator is smart enough to remove unnecessary calculations

  /*float positionX(Time currentTime)
  {
    if (currentTime < movingStartTime)
    {
      return movingFrom.col;
    }
    else if (currentTime < movingStartTime + movingDuration)
    {
      return (movingFrom.col * (movingStartTime + movingDuration - currentTime) +
              movingTo.col   * (currentTime - movingStartTime)) /
             movingDuration;
    }
    else
    {
      return movingTo.col;
    }
  }

  float positionY(Time currentTime)
  {
    if (currentTime < movingStartTime)
    {
      return movingFrom.row;
    }
    else if (currentTime < movingStartTime + movingDuration)
    {
      return (movingFrom.row * (movingStartTime + movingDuration - currentTime) +
              movingTo.row   * (currentTime - movingStartTime)) /
             movingDuration;
    }
    else
    {
      return movingTo.row;
    }
  }*/
};



//================================== Effects ===================================

// Base effect type, do not use directly
class BaseEffectType
{
public:
  BaseEffectType() : active_(false) { }
protected:
  bool active_;
};



class SmoothEffectType : public BaseEffectType
{
public:
  SmoothEffectType() : INITIAL_TIME(0.0), MIN_PROGRESS(0.0), MAX_PROGRESS(1.0),
                       PROGRESS_RANGE(MAX_PROGRESS - MIN_PROGRESS),
                       progress_(MIN_PROGRESS), lastTime_(INITIAL_TIME) { }

  void clear()
  {
    active_ = false;
    progress_ = MIN_PROGRESS;
    lastTime_ = INITIAL_TIME;
  }
protected:
  const float INITIAL_TIME;
  const float MIN_PROGRESS;
  const float MAX_PROGRESS;
  const float PROGRESS_RANGE;

  float progress_;
  Time lastTime_;
};



// An effect that repeats periodically (starting from zero moment)
class PeriodicalEffectType : public SmoothEffectType 
{
public:
  float period;

  PeriodicalEffectType() : period(1.0), stopping_(false) { }

  void enable(float newPeriod) 
  {
    active_ = true;
    stopping_ = false;
    period = newPeriod;
  }

  void disable() 
  {
    stopping_ = true;
  }

  float progress(Time currentTime) 
  {
    if (!active_)
    {
      lastTime_ = currentTime;
      return MIN_PROGRESS;
    }
    progress_ += float(currentTime - lastTime_) / period;
    while (progress_ > MAX_PROGRESS)
    {
      if (stopping_)
      {
        progress_ = MIN_PROGRESS;
        active_ = false;
      }
      else
        progress_ -= PROGRESS_RANGE;
    }
    lastTime_ = currentTime;
    return progress_;
  }

protected:
  bool stopping_;
};



// An effect that can fade in and (or) out. If (active == true) the effect is turned on
// and thus is either functioning normally [if (current_time > endTime] or is being
// activated now [if (current time <= endTime)]. If (active == false) everything is vice versa.
class FadingEffectType : public SmoothEffectType
{
public:
  float duration;

  FadingEffectType() : duration(1.0) { }

  void enable(float newDuration)
  {
    active_ = true;
    duration = newDuration;
  }

  void disable()
  {
    active_ = false;
  }

  float progress(Time currentTime)
  {
    float progressChange = float(currentTime - lastTime_) / duration;
    if (active_)
      progress_ = myMin(progress_ + progressChange, MAX_PROGRESS);
    else
      progress_ = myMax(progress_ - progressChange, MIN_PROGRESS);
    lastTime_ = currentTime;
    return progress_;
  }
};



// Effect that acts once
class SingleEffectType : public SmoothEffectType   // Name (?)
{
public:
  float duration;

  SingleEffectType() : duration(1.0) { }

  void enable(float newDuration)
  {
    active_ = true;
    duration = newDuration;
  }

  // TODO: Should SingleEffectType::disable be simply ignored?
  void disable() { }

  float progress(Time currentTime)
  {
    float progressChange = float(currentTime - lastTime_) / duration;
    if (active_) {
      progress_ += progressChange;
      if (progress_ > MAX_PROGRESS)
      {
        progress_ = MIN_PROGRESS;
        active_ = false;
      }
    }
    lastTime_ = currentTime;
    return progress_;
  }
};



// Effect that fades in and than immediately fades out.
class FlashEffectType : public SmoothEffectType
{
public:
  float halfDuration;

  FlashEffectType() : halfDuration(1.0) { }

  void enable(float newHalfDuration)
  {
    active_ = true;
    halfDuration = newHalfDuration;
  }

  void disable()
  {
    active_ = false;
  }

  float progress(Time currentTime)
  {
    float progressChange = float(currentTime - lastTime_) / halfDuration;
    if (active_) {
      progress_ += progressChange;
      if (progress_ > MAX_PROGRESS)
      {
        progress_ = MAX_PROGRESS;
        active_ = false;
      }
    }
    else
      progress_ = myMax(progress_ - progressChange, MIN_PROGRESS);
    lastTime_ = currentTime;
    return progress_;
  }
};



class DirectedEffectExtraType
{
public:
  int sender;
  int target;
};



typedef SingleEffectType FieldCleaningEffect; // --> FlashEffectType (?)

// if can't actually fade out :-)
typedef FadingEffectType PlayerDyingEffect;

typedef FadingEffectType NoHintEffect;

typedef FadingEffectType FlippedScreenEffect;

typedef PeriodicalEffectType RotatingFieldEffect;

typedef FadingEffectType SemicubesEffect;

typedef PeriodicalEffectType WaveEffect; // += FadingEffectType (?)

class LanternEffect : public FadingEffectType, public MovingObject { };

// TODO: try to rewrite to   operator=() = default  when C++0x is out
class PieceTheftEffect : public SingleEffectType, public DirectedEffectExtraType
{
public:
  /*PieceTheftEffect& operator=(const PieceTheftEffect& a)
  {
    progress_ = a.progress_;
    lastTime_ = a.lastTime_;
    duration = a.duration;
    sender = a.sender;
    target = a.target;
    return *this;
  }*/
};



class PlayerVisualEffects
{
public:
  FieldCleaningEffect fieldCleaning;
  PlayerDyingEffect playerDying;
  NoHintEffect noHint;
  FlippedScreenEffect flippedScreen;
  RotatingFieldEffect rotatingField;
  SemicubesEffect semicubes;
  WaveEffect wave;
  LanternEffect lantern;
//  std::list<PieceTheftEffect>::iterator pieceTheftPtr;
  PieceTheftEffect* pieceTheftPtr; 

  void clear()
  {
    fieldCleaning.clear();
    playerDying.clear();
    noHint.clear();
    flippedScreen.clear();
    rotatingField.clear();
    semicubes.clear();
    wave.clear();
    lantern.clear();
    pieceTheftPtr = NULL;
  }
};



class GlobalVisualEffects
{
public:
  std::list<PieceTheftEffect> pieceThefts;

  void clear()
  {
    pieceThefts.clear();
  }
};



//================================== Objects ===================================

class VisualObject { };



class BlockImage : public VisualObject, private MovingObject {
public:
  Color color;
  Bonus bonus;
  FieldCoords binding;
//  bool motionBlur;
  
  void placeAt(FieldCoords position)
  {
    binding = position;
    MovingObject::placeAt(FloatFieldCoords(position));
  }
  
  void moveTo(FieldCoords movingTo__, Time currentTime, Time movingDuration__)
  {
    binding = movingTo__;
    MovingObject::moveTo(FloatFieldCoords(movingTo__), currentTime, movingDuration__);
  }
  
  void placeNewImageAt(Color color__, FieldCoords position)
  {
    color = color__;
    bonus = bnNoBonus;
    placeAt(position);
  }
  
  using MovingObject::position;
  using MovingObject::positionX;
  using MovingObject::positionY;

  /*float positionY(Time currentTime)
  {
    return MovingObject::positionY(currentTime);
  }
  
  float positionX(Time currentTime)
  {
    return MovingObject::positionX(currentTime);
  }*/
};



class DisappearingLine : public VisualObject {
public:
  Color blockColor[FIELD_WIDTH];
  int row;
  Time startTime;
  Time duration;

  // TODO: declare constants
  float progress(Time currentTime)
  {
    if (currentTime <= startTime)
      return 0.0f;
    else if (currentTime < startTime + duration)
      return (currentTime - startTime) / duration;
    else
      return 1.0f;
  }
};

#endif
