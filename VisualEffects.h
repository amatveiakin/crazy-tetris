// TODO: make more class members protected and private

// TODO: stop ignoring such OOP features as virtual functions and abandon these template tricks (?)
//       (if it will get us close to KISS principle)

#ifndef CRAZYTETRIS_VISUALEFFECTS_H
#define CRAZYTETRIS_VISUALEFFECTS_H

#include <vector>
#include <list>
#include "Declarations.h"

//============================= Auxiliary classes ==============================

class LinearMotion
{
public:
  FloatFieldCoords aimingShiftVector;
  Time movingStartTime;
  Time movingDuration;

  LinearMotion(FloatFieldCoords aimingShiftVector__, Time movingStartTime__, Time movingDuration__) :
          aimingShiftVector(aimingShiftVector__), movingStartTime(movingStartTime__),
          movingDuration(movingDuration__) { }

  bool begun(Time currentTime) const
  {
    return currentTime >= movingStartTime;
  }

  bool finished(Time currentTime) const
  {
    return currentTime >= movingStartTime + movingDuration;
  }

  FloatFieldCoords shiftVector(Time currentTime) const
  {
    float multiplier = boundValue((currentTime - movingStartTime) / movingDuration, 0.0f, 1.0f);
    return aimingShiftVector * multiplier;
  }
};



//================================== Objects ===================================

class VisualObject
{
public:
  VisualObject* parent;

  virtual FloatFieldCoords relativePosition(Time currentTime) = 0;

  // TODO: Test whether VS' optimizator is smart enough to remove unnecessary calculations in the following functions

  float relativePositionX(Time currentTime)
  {
    return relativePosition(currentTime).col;
  }

  float relativePositionY(Time currentTime)
  {
    return relativePosition(currentTime).row;
  }

  FloatFieldCoords position(Time currentTime)  // TODO: rename (?)
  {
    if (parent)
      return parent->position(currentTime) + relativePosition(currentTime);
    else
      return relativePosition(currentTime);
  }

  float positionX(Time currentTime)
  {
    return position(currentTime).col;
  }

  float positionY(Time currentTime)
  {
    return position(currentTime).row;
  }
};



class AffixmentPointObject : public VisualObject  // name -> (?)
{
public:
  virtual void placeAt(FloatFieldCoords newPosition)
  {
    position_ = newPosition;
  }

  virtual FloatFieldCoords relativePosition(Time currentTime)
  {
    return position_;
  }

protected:
  FloatFieldCoords position_;
};



typedef AffixmentPointObject ReferenceFrame; // (?) Is it necessary?



class MovingObject : public AffixmentPointObject
{
public:
  std::vector<LinearMotion> motions;

  void addMotion(FloatFieldCoords aimingShiftVector, Time movingStartTime, Time movingDuration)
  {
    motions.push_back(LinearMotion(aimingShiftVector, movingStartTime, movingDuration));
  }

  virtual void placeAt(FloatFieldCoords newPosition)
  {
    AffixmentPointObject::placeAt(newPosition);
    motions.clear();
  }

  virtual FloatFieldCoords relativePosition(Time currentTime)
  {
    FloatFieldCoords currentPosition = position_;
    for (std::vector<LinearMotion>::iterator motion = motions.begin(); motion != motions.end(); )
    {
      currentPosition += motion->shiftVector(currentTime);
      if (motion->finished(currentTime))
      {
        position_ += motion->shiftVector(currentTime);
        motion = motions.erase(motion);
      }
      else
        ++motion;
    }
    return currentPosition;
  }
};



class MagnetObject : public AffixmentPointObject
{
public:
  VisualObject* binding;
  float maxSpeed;

  MagnetObject() : binding(NULL) { }

  void bindTo(VisualObject* newBinding)
  {
    binding = newBinding;
  }

  void resetBinding()
  {
    binding = NULL;
  }

  void clear()
  {
    resetBinding();
  }

  virtual FloatFieldCoords relativePosition(Time currentTime)
  {
    float deltaTime = currentTime - lastUpdated_;
    lastUpdated_ = currentTime;
    if (binding == NULL)
      return position_;
    FloatFieldCoords shiftVector = binding->position(currentTime) - position_;
    float deltaDistance = shiftVector.vectorLength();
    float maxDistance = maxSpeed * deltaTime;
    FloatFieldCoords newPosition;
    if (deltaDistance <= maxDistance)
      newPosition = binding->position(currentTime);
    else
      newPosition = position_ + shiftVector * maxDistance / deltaDistance;  // (?)
    position_ = newPosition;
    return newPosition;
  }

protected:
  Time lastUpdated_;
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
                       progress_(MIN_PROGRESS), lastUpdated_(INITIAL_TIME) { }

  bool fullyInactive() const
  {
    return (progress_ == MIN_PROGRESS);
  }

  bool fullyActive() const
  {
    return (progress_ == MAX_PROGRESS);
  }

  bool somehowActive() const
  {
    return (progress_ > MIN_PROGRESS);
  }

  void clear()
  {
    active_ = false;
    progress_ = MIN_PROGRESS;
    lastUpdated_ = INITIAL_TIME;
  }

protected:
  // Remove these constants (?)
  const Time  INITIAL_TIME;
  const float MIN_PROGRESS;
  const float MAX_PROGRESS;
  const float PROGRESS_RANGE;

  float progress_;
  Time lastUpdated_;
};



// An effect that repeats periodically (starting from zero moment)
class PeriodicalEffectType : public SmoothEffectType
{
public:
  Time period;

  PeriodicalEffectType() : STOPPING_ACCELERATION_COEFF(0.05f), period(1.0), isStopping_(false) { }

  void enable(float newPeriod)
  {
    active_ = true;
    isStopping_ = false;
    period = newPeriod;
  }

  void disable()
  {
    isStopping_ = true;
    stoppingAcceleration_ = 0.0;
  }

  float progress(Time currentTime)
  {
    if (!active_)
    {
      lastUpdated_ = currentTime;
      return MIN_PROGRESS;
    }

    float progress_increment = float(currentTime - lastUpdated_) / period;
    progress_ += progress_increment;
    if (isStopping_)
    {
      stoppingAcceleration_ += progress_increment * STOPPING_ACCELERATION_COEFF;
      progress_ += stoppingAcceleration_;
    }

    while (progress_ > MAX_PROGRESS)
    {
      if (isStopping_)
      {
        progress_ = MIN_PROGRESS;
        active_ = false;
      }
      else
        progress_ -= PROGRESS_RANGE;
    }
    lastUpdated_ = currentTime;
    return progress_;
  }

protected:
  const float STOPPING_ACCELERATION_COEFF;

  bool isStopping_;
  Time stoppingAcceleration_;
};



// An effect that can fade in and (or) out. If (active == true) the effect is turned on
// and thus is either functioning normally [if (current_time > endTime] or is being
// activated now [if (current time <= endTime)]. If (active == false) everything is vice versa.
class FadingEffectType : public SmoothEffectType
{
public:
  Time duration;

  FadingEffectType() : duration(1.0) { }

  void enable(Time newDuration)
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
    float progressChange = float(currentTime - lastUpdated_) / duration;
    if (active_)
      progress_ = myMin(progress_ + progressChange, MAX_PROGRESS);
    else
      progress_ = myMax(progress_ - progressChange, MIN_PROGRESS);
    lastUpdated_ = currentTime;
    return progress_;
  }
};



// Effect that acts once
class SingleEffectType : public SmoothEffectType   // Name (?)
{
public:
  Time duration;

  SingleEffectType() : duration(1.0) { }

  void enable(Time newDuration)
  {
    active_ = true;
    duration = newDuration;
  }

  // TODO: Should SingleEffectType::disable be simply ignored?
  void disable() { }

  float progress(Time currentTime)
  {
    float progressChange = float(currentTime - lastUpdated_) / duration;
    if (active_) {
      progress_ += progressChange;
      if (progress_ > MAX_PROGRESS)
      {
        progress_ = MIN_PROGRESS;
        active_ = false;
      }
    }
    lastUpdated_ = currentTime;
    return progress_;
  }
};



// Effect that fades in and than immediately fades out.
class FlashEffectType : public SmoothEffectType
{
public:
  Time halfDuration;

  FlashEffectType() : halfDuration(1.0) { }

  void enable(Time newDuration)
  {
    active_ = true;
    halfDuration = newDuration / 2.0f;
  }

  void disable()
  {
    active_ = false;
  }

  float progress(Time currentTime)
  {
    float progressChange = float(currentTime - lastUpdated_) / halfDuration;
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
    lastUpdated_ = currentTime;
    return progress_;
  }
};



// TODO: Join with FadingEffectType (?)

// Effect fades in and remains fully active until it is resetted
class PermanentEffectType : public SmoothEffectType   // Name (?)
{
public:
  Time duration;

  PermanentEffectType() : duration(1.0) { }

  void enable(Time newDuration)
  {
    active_ = true;
    duration = newDuration;
  }

  void disable()
  {
    active_ = false;
    progress_ = MIN_PROGRESS;
  }

  /*void restart(Time newDuration)
  {
    disable();
    enable(newDuration);
  }*/

  float progress(Time currentTime)
  {
    float progressChange = float(currentTime - lastUpdated_) / duration;
    if (active_) {
      progress_ += progressChange;
      if (progress_ > MAX_PROGRESS)
        progress_ = MAX_PROGRESS;
    }
    lastUpdated_ = currentTime;
    return progress_;
  }
};



template<class EffectT__>
class AutoStartingEffectType : public EffectT__
{
public:
  AutoStartingEffectType() : startTime_(WILL_NEVER_HAPPEN) { }

  void startAt(Time startTime)
  {
    startTime_ = startTime;
  }

  void enable(Time newDuration)
  {
    startTime_ = WILL_NEVER_HAPPEN;
    EffectT__::enable();
  }

  float progress(Time currentTime)
  {
    if (currentTime > startTime_)
      enable();
    return EffectT__::progress(currentTime);
  }

protected:
  Time startTime_;
};



template<class EffectT__>
class AutoStoppingEffectType : public EffectT__
{
public:
  AutoStoppingEffectType() : stopTime_(WILL_NEVER_HAPPEN) { }

  // TODO: find out why operator= isn't declared automatically.
  AutoStoppingEffectType& operator=(const AutoStoppingEffectType& source)
  {
    memcpy(this, &source, sizeof(AutoStoppingEffectType));  // (?)
    return *this;
  }

  void stopAt(Time stopTime)
  {
    stopTime_ = stopTime;
  }

  void disable()
  {
    stopTime_ = WILL_NEVER_HAPPEN;
    EffectT__::disable();
  }

  float progress(Time currentTime)
  {
    if (currentTime > stopTime_)
      disable();
    return EffectT__::progress(currentTime);
  }

protected:
  Time stopTime_;
};



class DirectedEffectExtraType
{
public:
  int sender;
  int target;
};



class BlockImage;

typedef PermanentEffectType HintEffect;

typedef PermanentEffectType HintMaterializationEffect;

typedef SingleEffectType FieldCleaningEffect; // --> FlashEffectType (?)

// if can't actually fade out :-)
typedef FadingEffectType PlayerDyingEffect;

typedef FadingEffectType NoHintEffect;

typedef FadingEffectType FlippedScreenEffect;

typedef PeriodicalEffectType RotatingFieldEffect;

typedef FadingEffectType SemicubesEffect;

typedef PeriodicalEffectType WaveEffect; // += FadingEffectType (?)

class LanternEffect : public FadingEffectType, public MagnetObject
{
public:
  void clear()
  {
    FadingEffectType::clear();
    MagnetObject::clear();
  }
};

class PieceTheftEffect : public SingleEffectType, public DirectedEffectExtraType { };



class PlayerVisualEffects
{
public:
  HintEffect hint;
  HintMaterializationEffect hintMaterialization;
  FieldCleaningEffect fieldCleaning;
  PlayerDyingEffect playerDying;
  NoHintEffect noHint;
  FlippedScreenEffect flippedScreen;
  RotatingFieldEffect rotatingField;
  SemicubesEffect semicubes;
  WaveEffect wave;
  LanternEffect lantern;
  PieceTheftEffect* pieceTheftPtr;

  void clear()
  {
    hint.clear();
    hintMaterialization.clear();
    fieldCleaning.clear();
    playerDying.clear();
    noHint.clear();
    flippedScreen.clear();
    rotatingField.clear();
    semicubes.clear();
    wave.clear();
    lantern.clear();  // (based on fallingPieceFrame)
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

class BlockImage : public MovingObject {
public:
  Color color;
  Bonus bonus;
  AutoStoppingEffectType<FadingEffectType> bonusImage;
//  bool motionBlur;

  BlockImage(VisualObject* parent__, Color color__, FieldCoords position) :
          color(color__), bonus(bnNoBonus)
  {
    parent = parent__;
    bonusImage.clear();
    placeAt(position);
  }

  /*void placeNewImageAt(VisualObject* parent__, Color color__, FieldCoords position)
  {
    parent = parent__;
    color = color__;
    bonus = bnNoBonus;
    bonusImage.clear();
    placeAt(position);
  }*/
};



class DisappearingLine {
public:
  Color blockColor[FIELD_WIDTH];
  int row;
  Time startTime;
  Time duration;

  // TODO: declare constants (?)
  float progress(Time currentTime)
  {
    return boundValue((currentTime - startTime) / duration, 0.0f, 1.0f);
  }
};

#endif
