#ifndef CRAZYTETRIS_VISUALEFFECTS_H
#define CRAZYTETRIS_VISUALEFFECTS_H

#include <map>
#include "Declarations.h"

//================================= General ===================================

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
  
  void startMovingTo(FloatFieldCoords movingTo__,
                     Time movingStartTime__, Time movingDuration__)
  {
    movingTo = movingTo__;
    movingStartTime = movingStartTime__;
    movingDuration = movingDuration__;
  }
  
  void setStanding(FloatFieldCoords position)
  {
    setMotion(position, position, 0.0, 1.0);
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
  }
  
  float positionX(Time currentTime)
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

  // TODO: SingleEffectType::disable should it be simply ignored?
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



typedef SingleEffectType ClearGlassEffect; // --> FlashEffectType (?)

// if can't actually fade out :-)
typedef FadingEffectType PlayerDyingEffect;

typedef FadingEffectType NoHintEffect;

typedef FadingEffectType FlippedScreenEffect;

typedef PeriodicalEffectType RotatingFieldEffect;

typedef FadingEffectType SemicubesEffect;

typedef PeriodicalEffectType WaveEffect;

class LanternEffect : public FadingEffectType, public MovingObject { };



class VisualEffects
{
public:
  ClearGlassEffect clearGlass;
  PlayerDyingEffect playerDying;
  NoHintEffect noHint;
  FlippedScreenEffect flippedScreen;
  RotatingFieldEffect rotatingField;
  SemicubesEffect semicubes;
  WaveEffect wave;
  LanternEffect lantern;

  void clear()
  {
    clearGlass.clear();
    playerDying.clear();
    noHint.clear();
    flippedScreen.clear();
    rotatingField.clear();
    semicubes.clear();
    wave.clear();
    lantern.clear();
  }
};




// =================================== Blocks ==================================

class VisualObject { };



//class BlockImage : public VisualObject, private MovingObject {   // TODO: return
class BlockImage : public VisualObject, public MovingObject {
public:
//  BlockID id;
  Color color;
//  bool motionBlur;
  
  /*BlockImage(Color color__, FloatFieldCoords movingFrom__, FloatFieldCoords movingTo__,
             Time movingStartTime__, Time movingDuration__)
  {
    setMotion(color__, movingFrom__, movingTo__, movingStartTime__, movingDuration__, motionBlur__);
  }

  BlockImage(BlockID id__, Color color__, FloatFieldCoords position)
  {
    setStanding(color__, position);
  }*/

  void setMotion(Color color__, FloatFieldCoords movingFrom__, FloatFieldCoords movingTo__,
                 Time movingStartTime__, Time movingDuration__)
  {
    color = color__;
    MovingObject::setMotion(movingFrom__, movingTo__, movingStartTime__, movingDuration__);
  }
  
  void setStanding(Color color__, FloatFieldCoords position)
  {
    setMotion(color__, position, position, 0.0, 1.0);
  }
  
  float positionY(Time currentTime)
  {
    return MovingObject::positionY(currentTime);
  }
  
  float positionX(Time currentTime)
  {
    return MovingObject::positionX(currentTime);
  }
};



class DisappearingLine : public VisualObject {
public:
  Color blockColor[FIELD_WIDTH];
  int row;
  Time startTime;
  Time duration;
};

#endif
