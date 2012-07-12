#ifndef CRAZYTETRIS_VISUALEFFECTS_H
#define CRAZYTETRIS_VISUALEFFECTS_H

#include <vector>
#include "Declarations.h"

//================================== Effects ===================================

// Base effect type, do not use directly
class BaseEffectType
{
public:
  BaseEffectType() : active_(false), progress_(MIN_PROGRESS), MIN_PROGRESS(0.0f), MAX_PROGRESS(1.0f),
                     PROGRESS_RANGE(MAX_PROGRESS - MIN_PROGRESS) { }
protected:
  const float MIN_PROGRESS;
  const float MAX_PROGRESS;
  const float PROGRESS_RANGE;
  const int TIME_UNITS_PER_SECONDS;

  bool active_;
  float progress_;
  Time lastTime_;
};



// An effect that repeats periodically (starting from zero moment)
class PeriodicalEffectType : public BaseEffectType
{
public:
  float period;
  
  void enable(Time currentTime, float newPeriod)
  {
    active_ = true;
    period = newPeriod;
    lastTime_ = currentTime;
  }
  
  void disable(Time currentTime)
  {
    stopping_ = true;
    lastTime_ = currentTime;
  }
  
  float progress(Time currentTime)
  {
    if (!active_)
      return MIN_PROGRESS;
    progress_ += (currentTime - lastTime_) / period;
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
  float animationDuration;
  
  void enable(Time currentTime, float newAnimationDuration)
  {
    active_ = true;
    animationDuration = newAnimationDuration;
    lastTime_ = currentTime;
  }
  
  void disable(Time currentTime)
  {
    active_ = false;
    lastTime_ = currentTime;
  }
  
  float progress(Time currentTime)
  {
    float progressChange = float(currentTime - lastTime_) / animationDuration;
    if (active_)
      progress_ = min(progress_ + progressChange, MAX_PROGRESS);
    else
      progress_ = max(progress_ - progressChange, MIN_PROGRESS);
    return progress_;
  }
};



// Effect that fades in for some time and than immediately fades out.    <-- spelling (?)
class BlinkEffectType : public BaseEffectType
{
public:
  float halfDuration;
  
  void enable(Time currentTime, float newHalfDuration)
  {
    active_ = true;
    halfDuration = newHalfDuration;
    lastTime_ = currentTime;
  }
  
  void disable(Time currentTime)
  {
    active_ = false;
    lastTime_ = currentTime;
  }
  
  float progress(Time currentTime)
  {
    float progressChange = float(currentTime - lastTime_) / halfDuration;
    if (active_) {
      progress_ += progressChange;
      if (progress > MAX_PROGRESS)
      {
        progress = MAX_PROGRESS;
        active_ = false;
      }
    }
    else
      progress_ = max(progress_ - progressChange, MIN_PROGRESS);
    return progress_;
  }
};



typedef BlinkEffectType ClearGlassEffect;

// if actually can't fade out :-)
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

class Block : public VisualObject
{
public:
  Color color;
  FieldCoords movingFrom;
  FieldCoords movingTo;
  Time movingStartTime;
  Time movingEndTime;
  bool motionBlur;
};

class DisappearingLine : public VisualObject
{
  std::vector<Color> blockColor;
  int row;
  Time startTime;
  Time endTime;
};

#endif
