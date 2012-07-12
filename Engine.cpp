// TODO: apply effects in the end or middle of animation, not in the beggining

#include <cstdio>
#include "IOFunctions.h"
#include "Engine.h"

//=================================== Field ====================================

Field::Field()
{
  // ``Floor''
  for (int row = -WALL_WIDTH; row < 0; ++row)
    for (int col = -WALL_WIDTH; col < FIELD_WIDTH + WALL_WIDTH; ++col)
      operator()(row, col).setBlock(COLORLESS);
  // ``Walls''
  for (int row = -WALL_WIDTH; row < FIELD_HEIGHT + SKY_HEIGHT; ++row)
  {
    for (int col = -WALL_WIDTH; col < 0; ++col)
      operator()(row, col).setBlock(COLORLESS);
    for (int col = FIELD_WIDTH; col < FIELD_WIDTH + WALL_WIDTH; ++col)
      operator()(row, col).setBlock(COLORLESS);
  }
  // ``Sky''
  for (int row = FIELD_HEIGHT; row < FIELD_HEIGHT + SKY_HEIGHT; ++row)
    for (int col = -WALL_WIDTH; col < FIELD_WIDTH + WALL_WIDTH; ++col)
      operator()(row, col).clear();
}

void Field::clear()
{
  for (int row = 0; row < FIELD_WIDTH; ++row)
    for (int col = 0; col < FIELD_HEIGHT; ++col)
      operator()(row, col).clear();
}



//==================================== Game ====================================

void Game::init()
{
//   for (int key = FIRST_REAL_KEY; key <= LAST_REAL_KEY; ++key)
//     lastKeyTrigger[key] = NEVER;
  loadPieces();
  for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
    player[iPlayer].init(this, iPlayer);
}

void Game::newMatch()
{
  // ...
}

void Game::newRound(Time currentTime__)
{
  currentTime = currentTime__;
//   lastSpeedUp = currentTime;
  for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
  {
    player[iPlayer].active = player[iPlayer].participates;
    if (player[iPlayer].active)
      ++nActivePlayers;
  }
  for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
    if (player[iPlayer].participates)
      player[iPlayer].prepareForNewRound();
}

void Game::endRound()
{

}

void Game::onGlobalKeyPress(GlobalKey key)
{
  switch (key)
  {
  }
}

// void Game::onKeyPress(RealKey key)
// {
//   ControlKey controlKey = keyMap[key].controlKey;
//   if (controlKey == keyUnassigned)
//     return;
//   if (playerKey(controlKey))
//   {
//     int iPlayer = keyMap[key].iPlayer;
//     if (player[iPlayer].active)
//       player[iPlayer].onKeyPress(controlKey);
//   }
//   else
//   {
//     switch (key)
//     {
//     }
//   }
// }

void Game::onTimer(Time currentTime__)
{
  currentTime = currentTime__;
  
  // TODO: place reactivation time check somewhere
  for (int key = 0; key < N_GLOBAL_KEYS; ++key)
    onGlobalKeyPress(GlobalKey(key));
  for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
    if (player[iPlayer].active)
      for (int key = 0; key < N_PLAYER_KEYS; ++key)
        player[iPlayer].onKeyPress(PlayerKey(key));
  
  for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
    player[iPlayer].onTimer();
}



void Game::loadPieces()   // TODO: rewrite it cleaner
{
  SmartFileHandler piecesFile(PIECE_TEMPLATES_FILE.c_str(), "r");
  if (piecesFile.get() == NULL)
    throw ERR_FILE_NOT_FOUND;   // TODO: format
  int nPieceTemplates;
  char blockRow[MAX_PIECE_SIZE + 1];
  fscanf(piecesFile.get(), "%d", &nPieceTemplates);
  pieceTemplate.resize(nPieceTemplates);
  for (int iPiece = 0; iPiece < nPieceTemplates; ++iPiece)
  {
    fscanf(piecesFile.get(), "%d", &pieceTemplate[iPiece].chance);
    skipWhitespace(piecesFile.get());
    fscanf(piecesFile.get(), "(%f,%f,%f)", &pieceTemplate[iPiece].color.r,
          &pieceTemplate[iPiece].color.g, &pieceTemplate[iPiece].color.b);
    pieceTemplate[iPiece].color.a = 1.0;
    for (int rotationState = 0; rotationState < N_PIECE_ROTATION_STATES; ++rotationState)
    {
      for (int row = 0; row < MAX_PIECE_SIZE; ++row)
      {
        skipWhitespace(piecesFile.get());
        fgets(blockRow, MAX_PIECE_SIZE + 1, piecesFile.get());
        for (int col = 0; col < MAX_PIECE_SIZE; ++col)
          switch (blockRow[col])
          {
            case PIECE_TEMPLATE_BLOCK_CHAR:
              pieceTemplate[iPiece].structure[rotationState].block.push_back(
                  FieldCoords(row - CENTRAL_PIECE_ROW, col - CENTRAL_PIECE_COL));
              break;
            case PIECE_TEMPLATE_FREE_CHAR:
              break;
            default:
              throw ERR_FILE_CORRUPTED;    // TODO: format
          }
      }
      pieceTemplate[iPiece].structure[rotationState].afterRead();
    }
  } 
}

// void Game::buildKeyMap()
// {
//   for (RealKey realKey = FIRST_REAL_KEY; realKey <= LAST_REAL_KEY; ++realKey)
//   {
//     keyMap[realKey].controlKey = keyUnassigned;
//     keyMap[realKey].iPlayer = -1;
//   }
//   
//   for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
//     if (player[iPlayer].participates)
//       for (int controlKey = 0; controlKey < N_CONTROL_KEYS; ++controlKey)
//       {
//         RealKey realKey = player[iPlayer].controls.keyArray[controlKey];
//         keyMap[realKey].controlKey = ControlKey(controlKey);
//         keyMap[realKey].iPlayer = iPlayer;
//       }
// }




//=================================== Player ===================================

void Player::init(Game* game__, int number__)
{
  game = game__;
  number = number__;
  score = 0;
}

void Player::loadPlayerInfo(PlayerInfo* newInfo)
{
  info = newInfo;
}

void Player::prepareForNewMatch()
{
  score = 0;
  // ...
}

void Player::prepareForNewRound()
{
  buffs.clear();
  debuffs.clear();
  field.clear();
  nextPiece = NULL;
  sendNewPiece();
  sendNewPiece();
  nDisappearingLines = 0;
  events.clear();
  events.push(etSpeedUp, currentTime() + SPEED_UP_INTERVAL);
  latestLineCollapse = NEVER;
  victimNumber = number;
  cycleVictim();
  // ...
}

Time Player::currentTime()
{
  return game->currentTime;
}

Time Player::pieceLoweringInterval()
{
  return NORMAL_LOWERING_TIME / speed;
}

Player* Player::victim()
{
  return (victimNumber != number) ? &game->player[victimNumber] : NULL;
}

void Player::takesBonus(Bonus bonus)
{
  if (isKind(bonus))
    applyBonus(bonus);
  else
  {
    if (victim() != NULL)
      victim()->applyBonus(bonus);
  }
}

void Player::applyBonus(Bonus bonus)
{
  if (isBuff(bonus))
    buffs.add(bonus);
  else if (isDebuff(bonus))
    debuffs.add(bonus);
  else
  {
    switch (bonus)
    {
      case bnHeal:
        heal();
        break;
      case bnSlowDown:
        // ...
        break;
      case bnClearField:
        // ...
        break;
      case bnSpeedUp:
        // ...
        break;
      case bnFlipField:
        // ...
        break;
      SKIP_ALL_BUT_SOCERIES;
    }
  }
  enableBonusEffect(bonus);
}

void Player::heal()
{
  // optimize disabling all effects (?)
  for (Bonus i = FIRST_DEBUFF; i <= LAST_DEBUFF; ++i)
    if (debuffs.check(i))
      disableBonusEffect(i);
  debuffs.clear();
}

void Player::kill()
{
  active = false;
}

void Player::onKeyPress(PlayerKey key)
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
  while (currentTime() >= events.top().activationTime)
  {
    switch (events.top().type)
    {
      case etPieceLowering:
        lowerPiece();
        break;
      case etLineCollapse:
        // ...
        break;
      case etSpeedUp:
        // ...
        break;
      case etBonusAppearance:
        // ...
        break;
      case etBonusDisappearance:
        // ...
        break;
    }
    events.pop();
  }
}



const BlockStructre& Player::fallingBlockStructure()
{
  return fallingPiece->structure[fallingPieceRotationState];
}

void Player::setUpPiece()
{
  for (size_t i = 0; i < fallingBlockStructure().block.size(); ++i)
  {
    FieldCoords cell = fallingPiecePosition + fallingBlockStructure().block[i];
    field(cell.col, cell.row).setBlock(fallingPiece->color);
  }
  removeFullLines();
}

bool Player::canDisposePiece(FieldCoords position, const BlockStructre& piece) const
{
  for (size_t i = 0; i < piece.block.size(); ++i)
    if (field(position.col + piece.block[i].col, position.row + piece.block[i].row).isBlocked())
      return false;
  return true;
}

void Player::sendNewPiece()
{
  fallingPiece = nextPiece;
  if (fallingPiece != NULL)
  {
    fallingPieceState = psNormal;
    fallingPieceRotationState = nextPieceRotationState;
    fallingPiecePosition.row = FIELD_HEIGHT - fallingBlockStructure().lowestBlockRow;
    fallingPiecePosition.col = MAX_PIECE_SIZE + rand() % (FIELD_WIDTH - 2 * MAX_PIECE_SIZE); // (?)
    events.push(etPieceLowering, currentTime() + NORMAL_LOWERING_TIME);
  }
  else
    fallingPieceState = psAbsent;
  nextPiece = &game->pieceTemplate[rand() % game->pieceTemplate.size()];
  nextPieceRotationState = rand() % N_PIECE_ROTATION_STATES;
}

void Player::lowerPiece()
{
  FieldCoords newPosition = fallingPiecePosition - FieldCoords(-1, 0);
  // We should not forget about the case when a piece gets stuck as the result of
  // field modification. Such pieces should not continue falling.
  if (canDisposePiece(fallingPiecePosition, fallingBlockStructure()) &&
      canDisposePiece(newPosition, fallingBlockStructure())) {
    fallingPiecePosition = newPosition; // animation (!)
    events.push(etPieceLowering,
                currentTime() + (fallingPieceState == psDropping) ?
                DROPPING_PIECE_LOWERING_TIME : pieceLoweringInterval());
  }
  else
    setUpPiece();
}

void Player::removeFullLines() // TODO: optimize: don't check all lines
{
  for (int row = 0; row < FIELD_HEIGHT; ++row)
  {
    bool rowIsFull = true;
    for (int col = 0; col < FIELD_WIDTH; ++col)
    {
      if (field(row, col).isFree())
      {
        rowIsFull = false;
        break;
      } 
    }
    if (rowIsFull)
    {
      disappearingLine[nDisappearingLines].startTime = currentTime();
      disappearingLine[nDisappearingLines].duration = LINE_DISAPPEAR_TIME;
      disappearingLine[nDisappearingLines].row = row;
      for (int col = 0; col < FIELD_WIDTH; ++col)
      {
        disappearingLine[nDisappearingLines].blockColor[col] = field(row, col).color;
        field(row, col).clear();
      }
      ++nDisappearingLines;
      if (latestLineCollapse < currentTime() + LINE_DISAPPEAR_TIME)
        latestLineCollapse = currentTime() + LINE_DISAPPEAR_TIME;
      latestLineCollapse += LINE_COLLAPSE_TIME;
      Event event(etLineCollapse, latestLineCollapse);
      event.parameters.lineCollapse.row = row;
      events.push(event);
    }
  }
}

void Player::collapseLine(int row) // TODO: optimize  AND  animate
{
  for (int curRow = row; curRow < FIELD_HEIGHT; ++curRow)
    for (int col = 0; col < FIELD_WIDTH; ++col)
      field(curRow, col) = field(curRow + 1, col);
  for (int i = 0; i < nDisappearingLines; ++i)
    if (disappearingLine[i].row > row)
      --disappearingLine[i].row;
}

void Player::movePiece(int direction)
{
  FieldCoords newPosition = fallingPiecePosition - FieldCoords(0, direction);
  if (canDisposePiece(newPosition, fallingBlockStructure()))
    fallingPiecePosition = newPosition;
}

void Player::dropPiece()
{
  fallingPieceState = psDropping;
  events.eraseEventType(etPieceLowering);
  events.push(etPieceLowering, currentTime() + DROPPING_PIECE_LOWERING_TIME);
}

void Player::rotatePiece(int direction)
{
  int newFallingPieceRotationState = (fallingPieceRotationState + N_PIECE_ROTATION_STATES + direction) %
                                     N_PIECE_ROTATION_STATES;
  if (canDisposePiece(fallingPiecePosition, fallingPiece->structure[newFallingPieceRotationState]))
    fallingPieceRotationState = newFallingPieceRotationState;
}

void Player::cycleVictim()
{
  if (game->nActivePlayers < 2)
    return;
  do
  {
    victimNumber = (victimNumber + 1) % MAX_PLAYERS;
  } while (game->player[victimNumber].active);
}

void Player::enableBonusEffect(Bonus bonus)
{
  switch (bonus)
  {
    case bnHeal:
      // no effect
      break;
    case bnSlowDown:
      // no effect
      break;
    case bnClearField:
      visualEffects.clearGlass.enable(BONUS_CLEAR_SCREEN_DURATION / 2);
      break;
    case bnFlippedScreen:
      visualEffects.flippedScreen.enable(BONUS_FLIPPING_SCREEN_DURATION);
      break;
    case bnInverseControls:
      // no effect
      break;
    case bnCrazyPieces:
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
    case bnFlipField:
      // no effect *at this moment*
      break;
    case bnNoBonus: ;
  }
}

void Player::disableBonusEffect(Bonus bonus)
{
  switch (bonus)
  {
    case bnHeal:
      // no effect
      break;
    case bnSlowDown:
      // no effect
      break;
    case bnClearField:
      visualEffects.clearGlass.disable();
      break;
    case bnFlippedScreen:
      visualEffects.flippedScreen.disable();
      break;
    case bnInverseControls:
      // no effect
      break;
    case bnCrazyPieces:
      // no effect
      break;
    case bnCutBlocks:
      visualEffects.semicubes.disable();
      break;
    case bnNoHint:
      visualEffects.noHint.disable();
      break;
    case bnSpeedUp:
      // no effect
      break;
    case bnFlipField:
      // no effect *at this moment*
      break;
    case bnNoBonus: ;
  }
}

void Player::flipBlocks()
{
  // TODO: implement  Player::flipBlocks()
}
