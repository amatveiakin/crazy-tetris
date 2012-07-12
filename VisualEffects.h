#ifndef CRAZYTETRIS_VISUALEFFECTS_H
#define CRAZYTETRIS_VISUALEFFECTS_H

#include <map>
#include "Declarations.h"

//================================== Effects ===================================

// Base effect type, do not use directly
class BaseEffectType
{
public:
  BaseEffectType() : MIN_PROGRESS(0.0f), MAX_PROGRESS(1.0f), PROGRESS_RANGE(MAX_PROGRESS - MIN_PROGRESS),
                     active_(false), progress_(MIN_PROGRESS), lastTime_(0.0f) { }
protected:
  const float MIN_PROGRESS;
  const float MAX_PROGRESS;
  const float PROGRESS_RANGE;

  bool active_;
  float progress_;
  Time lastTime_;
};



// An effect that repeats periodically (starting from zero moment)
class PeriodicalEffectType : public BaseEffectType 
{
public:
  float period;

  PeriodicalEffectType() : stopping_(false), period(1.0f) { }

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
class FadingEffectType : public BaseEffectType
{
public:
  float duration;

  FadingEffectType() : duration(1.0f) { }

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
      progress_ = min(progress_ + progressChange, MAX_PROGRESS);
    else
      progress_ = max(progress_ - progressChange, MIN_PROGRESS);
    lastTime_ = currentTime;
    return progress_;
  }
};



// Effect that acts once     // words (?)
class SingleEffectType : public BaseEffectType   // Name (?)
{
public:
  float duration;

  SingleEffectType() : duration(1.0f) { }

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



// Effect that fades in for some time and than immediately fades out.    <-- spelling (?)
class FlashEffectType : public BaseEffectType
{
public:
  float halfDuration;

  FlashEffectType() : halfDuration(1.0f) { }

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
      progress_ = max(progress_ - progressChange, MIN_PROGRESS);
    lastTime_ = currentTime;
    return progress_;
  }
};



typedef SingleEffectType ClearGlassEffect; // --> FlashEffectType (?)

// if can't actually fade out :-)
typedef FadingEffectType PlayerDyingEffect; // spelling -- (?)

typedef FadingEffectType NoHintEffect;

typedef FadingEffectType FlippedScreenEffect;

typedef PeriodicalEffectType RotatingFieldEffect;

typedef FadingEffectType SemicubesEffect;

typedef PeriodicalEffectType WaveEffect;



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
};




// =================================== Blocks ==================================

class VisualObject { };

class Block : public VisualObject {
public:
  Color color;
  FloatFieldCoords movingFrom;
  FloatFieldCoords movingTo;
  Time movingStartTime;
  Time movingDuration;
  bool motionBlur;
  
  void setMotion(Color color__, FloatFieldCoords movingFrom__, FloatFieldCoords movingTo__,
                 Time movingStartTime__, Time movingDuration__, bool motionBlur__ = false)
  {
    color = color__;
    movingFrom = movingFrom__;
    movingTo = movingTo__;
    movingStartTime = movingStartTime__;
    movingDuration = movingDuration__;
    motionBlur = motionBlur__;
  }
  
  void setStanding(Color color__, FloatFieldCoords position)
  {
    setMotion(color__, position, position, 0.0f, 1.0f, false);
  }
  
  float row(Time currentTime)
  {
    if (currentTime > movingStartTime + movingDuration) // (?)
    {
      return (movingFrom.row * (movingStartTime + movingDuration - currentTime) +
              movingTo.row   * (currentTime - movingStartTime)) /
             movingDuration;
    }
    else
      return movingTo.row;
  }
  
  float col(Time currentTime)
  {
    if (currentTime > movingStartTime + movingDuration) // (?)
    {
      return (movingFrom.col * (movingStartTime + movingDuration - currentTime) +
              movingTo.col   * (currentTime - movingStartTime)) /
             movingDuration;
    }
    else
      return movingTo.col;
  }
};

class DisappearingLine : public VisualObject {
public:
  Color blockColor[FIELD_WIDTH];
  int row;
  Time startTime;
  Time duration;
};

typedef std::map<FieldCoords, VisualObject> VisualObjects;

#endif
