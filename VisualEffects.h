#ifndef CRAZYTETRIS_VISUALEFFECTS_H
#define CRAZYTETRIS_VISUALEFFECTS_H

#include <vector>
#include "Declarations.h"

//================================== Effects ===================================

// Base effect type, do not use directly
class BaseEffectType {
protected:
  bool active_;
  Time startTime_;
};

// An effect that repeats periodically (starting from zero moment)
template <Time period>
class PeriodicalEffectType : public BaseEffectType {
public:
  void enable(Time currentTime) {
    active_ = true;
    startTime_ = currentTime;
    endTime_ = NEVER;
  }
  void disable(Time currentTime) {
    endTime_ = period - (currentTime - startTime_);
  }
  float progress(Time currentTime) {
    while (startTime_ > currentTime + period)
      startTime_ -= period;
    return double(currentTime - startTime_) / period; // --> double (?)
  }

protected:
  Time endTime_;
};

// An effect that can fade in and (or) out. If (active == true) the effect is turned on
// and thus is either functioning normally [if (current_time > endTime] or is being
// activated now [if (current time <= endTime)]. If (active == false) everything is vice versa.
template <Time duration>
class FadingEffectType : public BaseEffectType {
public:
  void enable(Time currentTime) {
    active_ = true;
    startTime_ = currentTime;
  }
  void disable(Time currentTime) {
    active_ = false;
    startTime_ = currentTime;
  }
  float progress(Time currentTime) {
    if (currentTime > startTime_ + duration)
      return 1.0f;
    return double(currentTime - startTime_) / duration; // --> double (?)
  }
};

typedef FadingEffectType<800> FlippedFieldEffect; // TODO: Use a constant declared somewhere else
typedef PeriodicalEffectType<3000> RotatingFieldEffect; // TODO: Use a constant declared somewhere else
typedef FadingEffectType<400> SemicubesEffect; // TODO: Use a constant declared somewhere else
typedef PeriodicalEffectType<2000> WaveEffect; // TODO: Use a constant declared somewhere else

class VisualEffects {
  FlippedFieldEffect flippedField;
  RotatingFieldEffect rotatingField;
  SemicubesEffect semicubes;
  WaveEffect wave;
};



// =================================== Blocks ==================================

class VisualObject { };

class Block : public VisualObject {
public:
  Color color; // RGB or index?
  FieldCoords movingFrom;
  FieldCoords movingTo;
  Time movingStartTime;
  Time movingEndTime;
  bool motionBlur;
};

class DisappearingLine : public VisualObject {
  std::vector<Color> blockColor;
  int row;
  Time startTime;
  Time endTime;
};

#endif
