// TODO: apply effects in the end or middle of animation, not in the beggining

#include <windows.h>
#include <cstdio>
#include "IOFunctions.h"
#include "Engine.h"

//=================================== Field ====================================

Field::Field()
{
  // ``Floor''
  for (int row = BORDERED_FIELD_ROW_BEGIN; row < 0; ++row)
    for (int col = BORDERED_FIELD_COL_BEGIN; col < BORDERED_FIELD_COL_END; ++col)
      operator()(row, col).setBlock(COLORLESS);
  // ``Walls''
  for (int row = BORDERED_FIELD_ROW_BEGIN; row < BORDERED_FIELD_ROW_END; ++row)
  {
    for (int col = BORDERED_FIELD_COL_BEGIN; col < 0; ++col)
      operator()(row, col).setBlock(COLORLESS);
    for (int col = FIELD_WIDTH; col < BORDERED_FIELD_COL_END; ++col)
      operator()(row, col).setBlock(COLORLESS);
  }
  // ``Sky''
  for (int row = FIELD_HEIGHT; row < BORDERED_FIELD_ROW_END; ++row)
    for (int col = 0; col < FIELD_WIDTH; ++col)
      operator()(row, col).clear();
}

void Field::clear()
{
  for (int row = 0; row < FIELD_HEIGHT; ++row)
    for (int col = 0; col < FIELD_WIDTH; ++col)
      operator()(row, col).clear();
}

void Field::clearImageIndices()
{
  for (int row = BORDERED_FIELD_ROW_BEGIN; row < BORDERED_FIELD_ROW_END; ++row)
    for (int col = BORDERED_FIELD_COL_BEGIN; col < BORDERED_FIELD_COL_END; ++col)
    {
      operator()(row, col).iBlockImage = NO_BLOCK_IMAGE;
      operator()(row, col).iNewBlockImage = NO_CHANGE;
    }
}



//==================================== Game ====================================

void Game::init()
{
  loadPieces();
  loadAccounts();
  loadSettings();
  for (int key = 0; key < N_GLOBAL_KEYS; ++key)
    nextGlobalKeyActivationTable[key] = 0.0;
  for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
    players[iPlayer].init(this, iPlayer);
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
  accounts.resize(MAX_PLAYERS);
  for (size_t iAccount = 0; iAccount < accounts.size(); ++iAccount)
    accounts[iAccount].name = "Player " + char(iAccount + '1');  // TODO: rewrite
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
    fprintf(settingsFile.get(), "%d\n", players[iPlayer].accountNumber);
    fprintf(settingsFile.get(), "%d\n", players[iPlayer].participates ? 1 : 0);
    for (int key = 0; key < N_PLAYER_KEYS; ++key)
      fprintf(settingsFile.get(), "%d\n", players[iPlayer].controls.keyArray[key]);
  }
}

void Game::loadDefaultSettings()
{
  /*players[0].participates = true;
  players[0].accountNumber = 0;
  players[0].controls.keyByName.keyLeft = 'A';
  players[0].controls.keyByName.keyRight = 'D';
  players[0].controls.keyByName.keyRotateCCW = 'W';
  players[0].controls.keyByName.keyRotateCW = 'E';
  players[0].controls.keyByName.keyDown = 'S';
  players[0].controls.keyByName.keyDrop = VK_TAB;
  players[0].controls.keyByName.keyChangeVictim = 'Q';

  players[1].participates = false;
  players[1].accountNumber = 1;

  players[2].participates = true;
  players[2].accountNumber = 2;
  players[2].controls.keyByName.keyLeft = VK_LEFT;
  players[2].controls.keyByName.keyRight = VK_RIGHT;
  players[2].controls.keyByName.keyRotateCCW = VK_UP;
  players[2].controls.keyByName.keyRotateCW = VK_DELETE;
  players[2].controls.keyByName.keyDown = VK_DOWN;
  players[2].controls.keyByName.keyDrop = VK_RSHIFT;
  players[2].controls.keyByName.keyChangeVictim = VK_RCONTROL;

  players[0].participates = false;
  players[0].accountNumber = 3;*/

  players[0].participates = true;
  players[0].accountNumber = 2;
  players[0].controls.keyByName.keyLeft = VK_LEFT;
  players[0].controls.keyByName.keyRight = VK_RIGHT;
  players[0].controls.keyByName.keyRotateCCW = VK_UP;
  players[0].controls.keyByName.keyRotateCW = VK_DELETE;
  players[0].controls.keyByName.keyDown = VK_DOWN;
  players[0].controls.keyByName.keyDrop = VK_RSHIFT;
  players[0].controls.keyByName.keyChangeVictim = VK_RCONTROL;
}

void Game::newMatch()
{
  // ...
}

void Game::newRound(Time currentTime__)
{
  currentTime = currentTime__;
//   lastSpeedUp = currentTime;
  activePlayers.clear();
  for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
  {
    players[iPlayer].active = players[iPlayer].participates;
    if (players[iPlayer].active)
      activePlayers.push_back(&players[iPlayer]);
  }
  for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
    if (players[iPlayer].participates)
      players[iPlayer].prepareForNewRound();
}

void Game::endRound()
{

}

void Game::onGlobalKeyPress(GlobalKey key)
{
  /*switch (key)
  {
  }*/
}

void Game::onTimer(Time currentTime__)
{
  currentTime = currentTime__;
  
  for (int key = 0; key < N_GLOBAL_KEYS; ++key)
  {
    if (keyPressed(globalControls.keyArray[key] &&
        (currentTime >= nextGlobalKeyActivationTable[key])))
    {
      onGlobalKeyPress(GlobalKey(key));
      nextGlobalKeyActivationTable[key] = currentTime + GLOBAL_KEY_REACTIVATION_TIME[key];
    }
  }
  for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
  {
    if (players[iPlayer].active)
    {
      for (int key = 0; key < N_PLAYER_KEYS; ++key)
      {
        if (keyPressed(players[iPlayer].controls.keyArray[key]) &&
            (currentTime >= players[iPlayer].nextKeyActivationTable[key]))
        {
          players[iPlayer].onKeyPress(PlayerKey(key));
          players[iPlayer].nextKeyActivationTable[key] = currentTime + PLAYER_KEY_REACTIVATION_TIME[key];
        }
      }
    }
  }
  
  for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
    if (players[iPlayer].active)
      players[iPlayer].onTimer();
}



void Game::loadPieces()   // TODO: rewrite it cleaner
{
  SmartFileHandler piecesFile(PIECE_TEMPLATES_FILE.c_str(), "r");
  if (piecesFile.get() == NULL)
    throw ERR_FILE_NOT_FOUND;   // TODO: format

  pieceTemplates.clear();
  char pieceBlock[MAX_PIECE_SIZE][MAX_PIECE_SIZE + 1];
  int nPieceTemplates;
  fscanf_s(piecesFile.get(), "%d", &nPieceTemplates);
  pieceTemplates.resize(nPieceTemplates);

  for (int iPiece = 0; iPiece < nPieceTemplates; ++iPiece)
  {
    fscanf_s(piecesFile.get(), "%d", &pieceTemplates[iPiece].chance);
    skipWhitespace(piecesFile.get());
    fscanf_s(piecesFile.get(), "(%f,%f,%f)", &pieceTemplates[iPiece].color.r,
          &pieceTemplates[iPiece].color.g, &pieceTemplates[iPiece].color.b);
    pieceTemplates[iPiece].color.a = 1.0;

    for (int rotationState = 0; rotationState < N_PIECE_ROTATION_STATES; ++rotationState)
    {
      for (int row = MAX_PIECE_SIZE - 1; row >= 0; --row)
      {
        skipWhitespace(piecesFile.get());
        fgets(&pieceBlock[row][0], MAX_PIECE_SIZE + 1, piecesFile.get());
      }

      int nBlockInCurrentPiece = 0;
      int maxBlockNumber = -1;
      for (int row = 0; row < MAX_PIECE_SIZE; ++row)
      {
        for (int col = 0; col < MAX_PIECE_SIZE; ++col)
        {
          if (pieceBlock[row][col] != PIECE_TEMPLATE_FREE_CHAR)
          {
            if ((pieceBlock[row][col] < '0') || (pieceBlock[row][col] > '9'))
              throw ERR_FILE_CORRUPTED;    // TODO: format
            maxBlockNumber = myMax(maxBlockNumber, pieceBlock[row][col] - '0');
            ++nBlockInCurrentPiece;
          }
        }
      }
      if (maxBlockNumber + 1 != nBlockInCurrentPiece)
        throw ERR_FILE_CORRUPTED;    // TODO: format

      pieceTemplates[iPiece].structure[rotationState].blocks.resize(nBlockInCurrentPiece);
      for (int row = 0; row < MAX_PIECE_SIZE; ++row)
        for (int col = 0; col < MAX_PIECE_SIZE; ++col)
          if (pieceBlock[row][col] != PIECE_TEMPLATE_FREE_CHAR)
            pieceTemplates[iPiece].structure[rotationState].blocks[pieceBlock[row][col] - '0'] =
                    FieldCoords(row - CENTRAL_PIECE_ROW, col - CENTRAL_PIECE_COL);
      pieceTemplates[iPiece].structure[rotationState].afterRead();
    }
  }

  randomPieceTable.clear();
  for (size_t iPiece = 0; iPiece < pieceTemplates.size(); ++iPiece)
    for (int i = 0; i < pieceTemplates[iPiece].chance; ++i)
      randomPieceTable.push_back(iPiece);
}



//=================================== Player ===================================

void Player::init(Game* game__, int number__)
{
  game = game__;
  number = number__;
  for (int key = 0; key < N_PLAYER_KEYS; ++key)
    nextKeyActivationTable[key] = 0.0;
}

void Player::loadAccountInfo(int newAccount)
{
  accountNumber = newAccount;
}

AccountInfo* Player::account()
{
  return &game->accounts[accountNumber];
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
  field.clearImageIndices();
  blockImages.clear();
  speed = STARTING_SPEED;
  nextPiece = NULL;
  sendNewPiece();
  sendNewPiece();
  disappearingLines.clear();
  latestLineCollapse = NEVER;
  victimNumber = number;
  cycleVictim();
  visualEffects.clear();
  visualEffects.lantern.setStanding(FloatFieldCoords((FIELD_HEIGHT - 1.0f) / 2.0f,
                                                     (FIELD_WIDTH  - 1.0f) / 2.0f));
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
  return (victimNumber != number) ? &game->players[victimNumber] : NULL;
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
      case etSpeedUp:   // TODO: [FIX BUG] why is it called in the very beginning?
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
  /*nBlockImages = 0;
  for (int row = 0; row < FIELD_HEIGHT; ++row)
    for (int col = 0; col < FIELD_WIDTH; ++col)
      if (field(row, col).isBlocked())
        blockImages[nBlockImages++].setStanding(field(row, col).color, FieldCoords(row, col));
  
  if (fallingPieceState != psAbsent)
  {
    for (size_t i = 0; i < fallingBlockStructure().blocks.size(); ++i)
    {
      blockImages[nBlockImages++].setStanding(fallingPiece->color,
          FieldCoords(fallingBlockStructure().blocks[i] + fallingPiecePosition));
    }
  }*/

  if (fallingPieceState != psAbsent)
  {
    // TODO: selfmodifiable  movingFrom  (?)
    visualEffects.lantern.setMotion(FloatFieldCoords(visualEffects.lantern.positionY(currentTime()),
                                                     visualEffects.lantern.positionX(currentTime())),
                                    fallingPiecePosition,
                                    currentTime(), BONUS_LANTERN_ANIMATION_TIME);
  }
}



const BlockStructure& Player::fallingBlockStructure()
{
  return fallingPiece->structure[fallingPieceRotationState];
}

void Player::setUpPiece()
{
  for (size_t i = 0; i < fallingBlockStructure().blocks.size(); ++i)
  {
    FieldCoords cell = fallingPiecePosition + fallingBlockStructure().blocks[i];
    if (cell.row >= FIELD_HEIGHT)
    {
      MessageBox(0, L"Good game :-)", L"The end", 0);
      exit(0);
    }
    /*{
      field.clear();
      field.clearImageIndices();
      return;
    }*/
    // TODO: do something else when player loses (but don't let the field borders to get spoilt!!!)
    field(cell).setBlock(fallingPiece->color);
    setUpBlockImage(fallingPiece->color, cell);
  }
  removeFullLines();
  sendNewPiece();
}

bool Player::canDisposePiece(FieldCoords position, const BlockStructure& piece) const
{
  for (size_t i = 0; i < piece.blocks.size(); ++i)
    if (field(position + piece.blocks[i]).isBlocked())
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
    for (size_t i = 0; i < fallingBlockStructure().blocks.size(); ++i)
    {
      addStandingBlockImage(fallingPiece->color,
                            fallingPiecePosition + fallingBlockStructure().blocks[i]);
    }
  }
  else
    fallingPieceState = psAbsent;
  nextPiece = &game->pieceTemplates[game->randomPieceTable[rand() % game->randomPieceTable.size()]];
  nextPieceRotationState = rand() % N_PIECE_ROTATION_STATES;
}

void Player::lowerPiece()
{
  FieldCoords newPosition = fallingPiecePosition + FieldCoords(-1, 0);
  // We should not forget about the case when a piece gets stuck as the result of
  // field modification. Such pieces should not continue falling.
  if (canDisposePiece(fallingPiecePosition, fallingBlockStructure()) &&
      canDisposePiece(newPosition, fallingBlockStructure())) {
    for (size_t i = 0; i < fallingBlockStructure().blocks.size(); ++i)
    {
      moveBlockImage(fallingPiecePosition + fallingBlockStructure().blocks[i],
                     newPosition + fallingBlockStructure().blocks[i],
                     currentTime(), PIECE_LOWERING_ANIMATION_TIME);
    }
    applyBlockImagesMovements();

    fallingPiecePosition = newPosition;
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
      disappearingLines.resize(disappearingLines.size() + 1);
      disappearingLines.back().startTime = currentTime();
      disappearingLines.back().duration = LINE_DISAPPEAR_TIME;
      disappearingLines.back().row = row;
      for (int col = 0; col < FIELD_WIDTH; ++col)
      {
        disappearingLines.back().blockColor[col] = field(row, col).color;
        field(row, col).clear();
        removeBlockImage(FieldCoords(row, col));
      }
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
  {
    for (int col = 0; col < FIELD_WIDTH; ++col)
    {
      field(curRow, col).assign(field(curRow + 1, col));
      if (field(curRow, col).isBlocked())
      {
        moveBlockImage(FieldCoords(curRow + 1, col), FieldCoords(curRow, col),
                       currentTime(), LINE_COLLAPSE_ANIMATION_TIME);
      }
    }
  }
  applyBlockImagesMovements();

  for (vector<DisappearingLine>::iterator i = disappearingLines.begin();
       i != disappearingLines.end(); ++i)
  {
    if (i->row == row)
    {
      disappearingLines.erase(i);
      break;
    }
  }
  for (vector<DisappearingLine>::iterator i = disappearingLines.begin();
       i != disappearingLines.end(); ++i)
    if (i->row > row)
      --(i->row);
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
  {
    for (size_t i = 0; i < fallingBlockStructure().blocks.size(); ++i)
    {
      moveBlockImage(fallingPiecePosition + fallingBlockStructure().blocks[i],
                     newPosition + fallingBlockStructure().blocks[i],
                     currentTime(), PIECE_MOVING_ANIMATION_TIME);
    }
    applyBlockImagesMovements();
    fallingPiecePosition = newPosition;
  }
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

  FieldCoords oldPosition = fallingPiecePosition;    // animation
  int oldRotationState = fallingPieceRotationState;    // animation
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

  for (size_t i = 0; i < fallingBlockStructure().blocks.size(); ++i)    // animation
  {    // animation
    moveBlockImage(oldPosition + fallingPiece->structure[oldRotationState].blocks[i],    // animation
                   fallingPiecePosition + fallingBlockStructure().blocks[i],    // animation
                   currentTime(), PIECE_ROTATING_ANIMATION_TIME);    // animation
  }    // animation
  applyBlockImagesMovements();    // animation
}

void Player::applyBlockImagesMovements()
{
  /*FILE* logFile = fopen("debug.log", "a");
  fprintf(logFile, "BEFORE (old):\n");
  for (int row = BORDERED_FIELD_ROW_BEGIN; row < BORDERED_FIELD_ROW_END; ++row)
  {
    for (int col = BORDERED_FIELD_COL_BEGIN; col < BORDERED_FIELD_COL_END; ++col)
      fprintf(logFile, "%d ", field(row, col).iBlockImage);
    fprintf(logFile, "\n");
  }
  fprintf(logFile, "BEFORE (new):\n");
  for (int row = BORDERED_FIELD_ROW_BEGIN; row < BORDERED_FIELD_ROW_END; ++row)
  {
    for (int col = BORDERED_FIELD_COL_BEGIN; col < BORDERED_FIELD_COL_END; ++col)
      fprintf(logFile, "%d ", field(row, col).iNewBlockImage);
    fprintf(logFile, "\n");
  }
  fprintf(logFile, "\n");
  fclose(logFile);*/

  for (int row = BORDERED_FIELD_ROW_BEGIN; row < BORDERED_FIELD_ROW_END; ++row)
    for (int col = BORDERED_FIELD_COL_BEGIN; col < BORDERED_FIELD_COL_END; ++col)
      if (field(row, col).iNewBlockImage != NO_CHANGE)
      {
        field(row, col).iBlockImage = field(row, col).iNewBlockImage;
        field(row, col).iNewBlockImage = NO_CHANGE;
      }

  /*logFile = fopen("debug.log", "a");
  fprintf(logFile, "AFTER:\n");
  for (int row = BORDERED_FIELD_ROW_BEGIN; row < BORDERED_FIELD_ROW_END; ++row)
  {
    for (int col = BORDERED_FIELD_COL_BEGIN; col < BORDERED_FIELD_COL_END; ++col)
      fprintf(logFile, "%d ", field(row, col).iBlockImage);
    fprintf(logFile, "\n");
  }
  fprintf(logFile, "\n\n\n");
  fclose(logFile);*/
}

void Player::addStandingBlockImage(Color color, FieldCoords position)
{
  blockImages.resize(blockImages.size() + 1);
  blockImages.back().setStanding(color, position);
  field(position).iBlockImage = blockImages.size() - 1;
}

void Player::addMovingBlockImage(Color color, FieldCoords movingFrom, FieldCoords movingTo,
                                 Time movingStartTime, Time movingDuration)
{
  blockImages.resize(blockImages.size() + 1);
  blockImages.back().setMotion(color, movingFrom, movingTo, movingStartTime, movingDuration);
  field(movingTo).iBlockImage = blockImages.size() - 1;
}

void Player::setUpBlockImage(Color color, FieldCoords position)
{
  blockImages[field(position).iBlockImage].setStanding(color, position);
}

void Player::moveBlockImage(FieldCoords movingFrom, FieldCoords movingTo,
                            Time movingStartTime, Time movingDuration)
{
  if (movingFrom == movingTo)
    return;
  blockImages[field(movingFrom).iBlockImage].setMotion(  // add version that doesn't change color
          blockImages[field(movingFrom).iBlockImage].color, movingFrom, movingTo,
          movingStartTime, movingDuration);
  field(movingTo).iNewBlockImage = field(movingFrom).iBlockImage;
  if (field(movingFrom).iNewBlockImage == NO_CHANGE)
    field(movingFrom).iNewBlockImage = NO_BLOCK_IMAGE;
}

void Player::removeBlockImage(FieldCoords position)
{
  if (field(position).iBlockImage == blockImages.size() - 1)
  {
    field(position).iBlockImage = NO_BLOCK_IMAGE;
  }
  else
  {
    blockImages[field(position).iBlockImage] = blockImages.back();
    // TODO: change it !!!
    field((blockImages.back().movingTo.row), (blockImages.back().movingTo.col)).iBlockImage =
            field(position).iBlockImage;
    field(position).iBlockImage = NO_BLOCK_IMAGE;
  }
  blockImages.erase(blockImages.end() - 1);
}

void Player::cycleVictim()
{
  if (game->activePlayers.size() < 2)
    return;
  do
  {
    victimNumber = (victimNumber + 1) % MAX_PLAYERS;
  } while (game->players[victimNumber].active);
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
