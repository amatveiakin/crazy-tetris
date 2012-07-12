// TODO: apply effects in the end or middle of animation, not in the beggining

#include <windows.h>
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
  for (int row = 0; row < FIELD_HEIGHT; ++row)
    for (int col = 0; col < FIELD_WIDTH; ++col)
      operator()(row, col).clear();
}



//==================================== Game ====================================

void Game::init()
{
  loadPieces();
  loadAccounts();
  loadSettings();
  for (int key = 0; key < N_GLOBAL_KEYS; ++key)
    nextGlobalKeyActivation[key] = 0.0;
  for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
    player[iPlayer].init(this, iPlayer);
}

void Game::loadAccounts()
{
  // ...
  loadDefaultSettings();
}

void Game::saveAccounts()
{
  // ...
}

void Game::loadDefaultAccounts()
{
  account.resize(MAX_PLAYERS);
  for (size_t iAccount = 0; iAccount < account.size(); ++iAccount)
    account[iAccount].name = "Player " + char(iAccount + '1');  // TODO: rewrite
}

void Game::loadSettings()
{
  // ...
  loadDefaultSettings();
}

void Game::saveSettings()
{
  SmartFileHandler settingsFile(SETTINGS_FILE.c_str(), "w");
  for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
  {
    fprintf(settingsFile.get(), "%d\n", player[iPlayer].accountNumber);
    fprintf(settingsFile.get(), "%d\n", player[iPlayer].participates ? 1 : 0);
    for (int key = 0; key < N_PLAYER_KEYS; ++key)
      fprintf(settingsFile.get(), "%d\n", player[iPlayer].controls.keyArray[key]);
  }
}

void Game::loadDefaultSettings()
{
  player[0].participates = true;
  player[0].accountNumber = 0;
  player[0].controls.keyByName.keyLeft = VK_LEFT;
  player[0].controls.keyByName.keyRight = VK_RIGHT;
  player[0].controls.keyByName.keyRotateCCW = VK_UP;
  player[0].controls.keyByName.keyRotateCW = VK_DELETE;
  player[0].controls.keyByName.keyDown = VK_DOWN;
  player[0].controls.keyByName.keyDrop = VK_RSHIFT;
  player[0].controls.keyByName.keyChangeVictim = VK_RCONTROL;
  player[1].participates = false;
  player[2].participates = false;
  player[3].participates = false;
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

void Game::onTimer(Time currentTime__)
{
  currentTime = currentTime__;
  
  for (int key = 0; key < N_GLOBAL_KEYS; ++key)
  {
    if (keyPressed(globalControls.keyArray[key] &&
        (currentTime >= nextGlobalKeyActivation[key])))
    {
      onGlobalKeyPress(GlobalKey(key));
      nextGlobalKeyActivation[key] = currentTime + GLOBAL_KEY_REACTIVATION_TIME[key];
    }
  }
  for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
  {
    if (player[iPlayer].active)
    {
      for (int key = 0; key < N_PLAYER_KEYS; ++key)
      {
        if (keyPressed(player[iPlayer].controls.keyArray[key]) &&
            (currentTime >= player[iPlayer].nextKeyActivation[key]))
        {
          player[iPlayer].onKeyPress(PlayerKey(key));
          player[iPlayer].nextKeyActivation[key] = currentTime + PLAYER_KEY_REACTIVATION_TIME[key];
        }
      }
    }
  }
  
  for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
    if (player[iPlayer].active)
      player[iPlayer].onTimer();
}



void Game::loadPieces()   // TODO: rewrite it cleaner
{
  SmartFileHandler piecesFile(PIECE_TEMPLATES_FILE.c_str(), "r");
  if (piecesFile.get() == NULL)
    throw ERR_FILE_NOT_FOUND;   // TODO: format
  pieceTemplate.clear();
  char blockRow[MAX_PIECE_SIZE + 1];
  int nPieceTemplates;
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

  randomPieceTable.clear();
  for (int iPiece = 0; iPiece < pieceTemplate.size(); ++iPiece)
    for (int i = 0; i < pieceTemplate[iPiece].chance; ++i)
      randomPieceTable.push_back(iPiece);
}



//=================================== Player ===================================

void Player::init(Game* game__, int number__)
{
  game = game__;
  number = number__;
  for (int key = 0; key < N_PLAYER_KEYS; ++key)
    nextKeyActivation[key] = 0.0;
}

void Player::loadAccountInfo(int newAccount)
{
  accountNumber = newAccount;
}

AccountInfo* Player::account()
{
  return &game->account[accountNumber];
}

void Player::prepareForNewMatch()
{
  score = 0;
  // ...
}

void Player::prepareForNewRound()
{
  events.clear();
  events.push(etSpeedUp, currentTime() + SPEED_UP_INTERVAL);
  buffs.clear();
  debuffs.clear();
  field.clear();
  speed = STARTING_SPEED;
  nextPiece = NULL;
  sendNewPiece();
  sendNewPiece();
  nDisappearingLines = 0;
  latestLineCollapse = NEVER;
  victimNumber = number;
  cycleVictim();
  visualEffects.lantern.lanternPosition = FloatFieldCoords((FIELD_HEIGHT - 1.0f) / 2.0f,
                                                           (FIELD_WIDTH  - 1.0f) / 2.0f);
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
      events.eraseEventType(etPieceLowering);
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
  while ((!events.empty()) && (currentTime() >= events.top().activationTime))
  {
    EventSet::iterator curentEvent = events.topIterator();
    switch (events.top().type)
    {
      case etPieceLowering:
        lowerPiece();
        break;
      case etLineCollapse:
        collapseLine(events.top().parameters.lineCollapse.row);
        break;
      case etSpeedUp:   // TODO: why is it called in the very beginning?
        // ...
        break;
      case etBonusAppearance:
        // ...
        break;
      case etBonusDisappearance:
        // ...
        break;
    }
    events.erase(curentEvent);
  }
  redraw();
}

void Player::redraw()
{
  nBlockImages = 0;
  for (int row = 0; row < FIELD_HEIGHT; ++row)
    for (int col = 0; col < FIELD_WIDTH; ++col)
      if (field(row, col).isBlocked())
        blockImage[nBlockImages++].setStanding(field(row, col).color, FloatFieldCoords(row, col));
  
  if (fallingPieceState != psAbsent)
  {
    for (int i = 0; i < fallingBlockStructure().block.size(); ++i)
    {
      blockImage[nBlockImages++].setStanding(fallingPiece->color,
          FloatFieldCoords(fallingBlockStructure().block[i] + fallingPiecePosition));
    }
  }

  if (fallingPieceState != psAbsent)
  {
    visualEffects.lantern.lanternPosition = fallingPiecePosition;
    visualEffects.lantern.powerOn();
  }
  else
    visualEffects.lantern.powerOff();
}



const BlockStructure& Player::fallingBlockStructure()
{
  return fallingPiece->structure[fallingPieceRotationState];
}

void Player::setUpPiece()
{
  for (size_t i = 0; i < fallingBlockStructure().block.size(); ++i)
  {
    FieldCoords cell = fallingPiecePosition + fallingBlockStructure().block[i];
    field(cell.row, cell.col).setBlock(fallingPiece->color);
  }
  removeFullLines();
  sendNewPiece();
}

bool Player::canDisposePiece(FieldCoords position, const BlockStructure& piece) const
{
  for (size_t i = 0; i < piece.block.size(); ++i)
    if (field(position.row + piece.block[i].row, position.col + piece.block[i].col).isBlocked())
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
    fallingPiecePosition.row = FIELD_HEIGHT + fallingBlockStructure().lowestBlockRow;
    fallingPiecePosition.col = MAX_PIECE_SIZE + rand() % (FIELD_WIDTH - 2 * MAX_PIECE_SIZE); // (?)
    events.push(etPieceLowering, currentTime() + pieceLoweringInterval());
  }
  else
    fallingPieceState = psAbsent;
//  nextPiece = &game->pieceTemplate[rand() % game->pieceTemplate.size()];
  nextPiece = &game->pieceTemplate[game->randomPieceTable[rand() % game->randomPieceTable.size()]];
  nextPieceRotationState = rand() % N_PIECE_ROTATION_STATES;
}

void Player::lowerPiece()
{
  FieldCoords newPosition = fallingPiecePosition + FieldCoords(-1, 0);
  // We should not forget about the case when a piece gets stuck as the result of
  // field modification. Such pieces should not continue falling.
  if (canDisposePiece(fallingPiecePosition, fallingBlockStructure()) &&
      canDisposePiece(newPosition, fallingBlockStructure())) {
    fallingPiecePosition = newPosition; // animation (!)
    events.push(etPieceLowering,
                currentTime() +
                ((fallingPieceState == psDropping) ?
                 DROPPING_PIECE_LOWERING_TIME : pieceLoweringInterval()));
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
  for (EventSet::iterator event = events.begin(); event != events.end(); ++event)
    if ((event->type == etLineCollapse) && (event->parameters.lineCollapse.row > row))
      --event->parameters.lineCollapse.row;
}

void Player::movePiece(int direction)
{
  if (fallingPieceState != psNormal)
    return;
  FieldCoords newPosition = fallingPiecePosition + FieldCoords(0, direction);
  if (canDisposePiece(newPosition, fallingBlockStructure()))
    fallingPiecePosition = newPosition;
}

void Player::dropPiece()
{
  if (fallingPieceState != psNormal)
    return;
  fallingPieceState = psDropping;
  events.eraseEventType(etPieceLowering);
  events.push(etPieceLowering, currentTime() + DROPPING_PIECE_LOWERING_TIME);
}

void Player::rotatePiece(int direction)
{
  if (fallingPieceState != psNormal)
    return;
  int newFallingPieceRotationState = (fallingPieceRotationState + N_PIECE_ROTATION_STATES + direction) %
                                     N_PIECE_ROTATION_STATES;

  if (canDisposePiece(fallingPiecePosition, fallingPiece->structure[newFallingPieceRotationState]))
  {
    fallingPieceRotationState = newFallingPieceRotationState;
  }
  else if (canDisposePiece(fallingPiecePosition + FieldCoords(0, 1), // TODO: optimize
           fallingPiece->structure[newFallingPieceRotationState]))
  {
    fallingPiecePosition += FieldCoords(0, 1);
    fallingPieceRotationState = newFallingPieceRotationState;
  }
  else if (canDisposePiece(fallingPiecePosition - FieldCoords(0, 1), // TODO: optimize
           fallingPiece->structure[newFallingPieceRotationState]))
  {
    fallingPiecePosition -= FieldCoords(0, 1);
    fallingPieceRotationState = newFallingPieceRotationState;
  }
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
