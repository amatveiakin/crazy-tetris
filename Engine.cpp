#include "IOFunctions.h"
#include "Engine.h"

//==================================== Game ====================================

void Game::Game()
{
  player.resize(MAX_PLAYERS);
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

void Game::onKeyPress(ControlKey key, int iPlayer = -1)
{
  if (playerKey(key))
  {
    for (int i = 0; i < MAX_PLAYERS; ++i)
      if (player[i].active)
        player[i].onKeyPress(key);
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



//=================================== Player ===================================

void Player::init(Game* game, int number)
{

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

}

void Player::applyBonus(Bonus bonus)
{

}

void Player::heal()
{
  // TODO: disable effects
  diseases.clear();
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




// ...






void Player::enableEffectByBonus(Bonus bonus)
{
  switch (bonus)
  {
    case bnHeal:
      // no effect
      break;
    case bnFlipScreen:
      effects.flippedView.enable(X);
      break;
    case bnInverseControls:
      // no effect
      break;
    case bnTransparentBlocks:
      effects.semicubes.enable(X);
      break;
    case bnNoHint:
      effects.noHint.enable(X);
      break;
    case bnSpeedUp:
      // no effect
      break;
    case bnSlowDown:
      // no effect
      break;
    case bnClearGlass:
      effects.clearGlass.enable(X);
      break;
    case bnFlipField:
      // no effect (?)
      break;
  }
}

void Player::disableEffectByBonus(Bonus bonus)
{

}

