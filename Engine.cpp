#include "IOFunctions.h"
#include "Engine.h"

//==================================== Game ====================================

Game::Game()
{
  for (int i = 0; i < MAX_PLAYERS; ++i)
    player[i].init(this, i);
}

void Game::newRound()
{
  for (int i = 0; i < MAX_PLAYERS; ++i)
    if (player[i].active)
      player[i].prepareForNewRound();
}

void Game::endRound()
{

}

void Game::onKeyPress(RealKey key)
{
  ControlKey controlKey;
  int iPlayer;
  if (playerKey(controlKey))
  {
//     if (player[iPlayer].active)
    player[iPlayer].onKeyPress(controlKey);
  }
  else
  {
    switch (key)
    {
    }
  }
}

void Game::onTimer()
{

}

void Game::buildKeyMap()
{
  for (RealKey realKey = BEGIN_REAL_KEY; realKey < END_REAL_KEY; ++realKey)
  {
    keyMap[realKey].controlKey = keyUnassigned;
    keyMap[realKey].iPlayer = -1;
  }
  
  for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
    if (player[iPlayer].active)
      for (int controlKey = 0; controlKey < N_CONTROL_KEYS; ++controlKey)
      {
        RealKey realKey = player[iPlayer].controls.keyArray[controlKey];
        keyMap[realKey].controlKey = ControlKey(controlKey);
        keyMap[realKey].iPlayer = iPlayer;
      }
}




//=================================== Player ===================================

void Player::init(Game* game__, int number__)
{
  game = game__;
  number = number__;
}

void Player::loadPlayerInfo(PlayerInfo* newInfo)
{
  info = newInfo;
}

void Player::prepareForNewRound()
{

}

Time Player::pieceLoweringInterval()
{

}

void Player::takesBonus(Bonus bonus)
{
  if (isKind(bonus))
    applyBonus(bonus);
  else
    victim->applyBonus(bonus);
}

void Player::applyBonus(Bonus bonus)
{
  if (isBuff(bonus))
    buffs.add(bonus);
  else if (isDebuff(bonus))
    debuffs.add(bonus);
  enableBonusEffect(bonus);
}

void Player::heal()
{
  // TODO: disable effects
  debuffs.clear();
}

void Player::kill()
{
  active = false;
}

void Player::onKeyPress(ControlKey key)
{
  switch (key) {
    case keyLeft:
      movePiece(-1);
      break;
    case keyRight:
      movePiece(1);
      break;
    case keyRotateCW:
      rotatePiece(-1);
      break;
    case keyRotateCCW:
      rotatePiece(1);
      break;
    case keyDown:
      lowerPiece();
      break;
    case keyDrop:
      dropPiece();
      break;
    case keyChangeVictim:
      cycleVictim();
      break;
  }
}

void Player::onTimer()
{

}



void Player::generateNewPiece()
{

}

void Player::lowerPiece()
{

}

void Player::checkFullLines()
{

}

void Player::movePiece(int direction)
{

}

void Player::dropPiece()
{

}

void Player::rotatePiece(int direction)
{

}

void Player::cycleVictim()
{

}

void Player::setNextBonusAppearTime()
{

}

void Player::setNextBonusDisappearTime()
{

}

void Player::refreshBonus()
{

}

void Player::enableBonusEffect(Bonus bonus)
{
  switch (bonus)
  {
    case bnHeal:
      // no effect
      break;
    case bnFlippedScreen:
      visualEffects.flippedScreen.enable(BONUS_FLIPPING_SCREEN_DURATION);
      break;
    case bnInverseControls:
      // no effect
      break;
    case bnCutBlocks:
      visualEffects.semicubes.enable(BONUS_CUTTING_BLOCKS_DURATION);
      break;
    case bnNoHint:
      visualEffects.noHint.enable(BONUS_REMOVING_HINT_DURATION);
      break;
    case bnSpeedUp:
      // no effect
      break;
    case bnSlowDown:
      // no effect
      break;
    case bnClearField:
      visualEffects.clearGlass.enable(BONUS_CLEAR_SCREEN_DURATION / 2);
      break;
    case bnFlipField:
      // no effect (?)
      break;
  }
}

void Player::disableBonusEffect(Bonus bonus)
{

}

void Player::flipBlocks()
{

}
