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



//==================================== Game ====================================

void Game::init()
{
  assert(PIECE_FORCED_LOWERING_ANIMATION_TIME <= DOWN_KEY_REACTIVATION_TIME);

  loadPieces();
  loadBonuses();
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
  loadDefaultAccounts();
}

void Game::saveAccounts()
{
  // ...
}

void Game::loadDefaultAccounts()
{
  accounts.resize(MAX_PLAYERS);
  for (size_t iAccount = 0; iAccount < accounts.size(); ++iAccount)
    accounts[iAccount].name = wstring(L"Player ") + wchar_t(iAccount + '1');  // TODO: rewrite
}

void Game::loadSettings()
{
  SmartFileHandler settingsFile(SETTINGS_FILE.c_str(), L"r");
  if (settingsFile.get() == NULL)
  {
      loadDefaultSettings();
      return;
  }
  for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
  {
    fscanf_s(settingsFile.get(), "%d\n", &players[iPlayer].accountNumber);
//    fscanf_s(settingsFile.get(), "%d\n", &players[iPlayer].participates);
    int tmp;
    fscanf_s(settingsFile.get(), "%d\n", &tmp);
    switch (tmp)
    {
    case 0:
      players[iPlayer].participates = false;
      break;
    case 1:
      players[iPlayer].participates = true;
      break;
    default:
      throw ERR_FILE_CORRUPTED; // TODO: format
    }
    for (int key = 0; key < N_PLAYER_KEYS; ++key)
      fscanf_s(settingsFile.get(), "%d\n", &players[iPlayer].controls.keyArray[key]);
  }
}

void Game::saveSettings()
{
  SmartFileHandler settingsFile(SETTINGS_FILE.c_str(), L"w");
  if (settingsFile.get() == NULL)
    throw ERR_FILE_WRITE_ERROR;   // TODO: format
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
  players[0].participates = true;
  players[0].accountNumber = 0;
  players[0].controls.keyByName.keyLeft = 'A';
  players[0].controls.keyByName.keyRight = 'D';
  players[0].controls.keyByName.keyRotateCCW = 'W';
  players[0].controls.keyByName.keyRotateCW = 'E';
  players[0].controls.keyByName.keyDown = 'S';
  players[0].controls.keyByName.keyDrop = VK_TAB;
  players[0].controls.keyByName.keyNextVictim = 'Q';

  players[1].participates = false;
  players[1].accountNumber = 1;
  players[1].controls.keyByName.keyLeft = 'H';
  players[1].controls.keyByName.keyRight = 'K';
  players[1].controls.keyByName.keyRotateCCW = 'U';
  players[1].controls.keyByName.keyRotateCW = 'I';
  players[1].controls.keyByName.keyDown = 'J';
  players[1].controls.keyByName.keyDrop = VK_SPACE;
  players[1].controls.keyByName.keyNextVictim = 'L';

  players[2].participates = true;
  players[2].accountNumber = 2;
  players[2].controls.keyByName.keyLeft = VK_LEFT;
  players[2].controls.keyByName.keyRight = VK_RIGHT;
  players[2].controls.keyByName.keyRotateCCW = VK_UP;
  players[2].controls.keyByName.keyRotateCW = VK_DELETE;
  players[2].controls.keyByName.keyDown = VK_DOWN;
  players[2].controls.keyByName.keyDrop = VK_RSHIFT;
  players[2].controls.keyByName.keyNextVictim = VK_RCONTROL;

  players[3].participates = false;  
  players[3].accountNumber = 3;
  players[3].controls.keyByName.keyLeft = VK_NUMPAD4;
  players[3].controls.keyByName.keyRight = VK_NUMPAD6;
  players[3].controls.keyByName.keyRotateCCW = VK_NUMPAD8;
  players[3].controls.keyByName.keyRotateCW = VK_NUMPAD9;
  players[3].controls.keyByName.keyDown = VK_NUMPAD5;
  players[3].controls.keyByName.keyDrop = VK_NUMPAD0;
  players[3].controls.keyByName.keyNextVictim = VK_ADD;
}

void Game::newMatch()
{
  participants.clear();
  for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
    if (players[iPlayer].participates)
      participants.push_back(&players[iPlayer]);
}

void Game::newRound(Time currentTime__)
{
  currentTime = currentTime__;
  globalEffects.clear();
  for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
    players[iPlayer].active = players[iPlayer].participates;
  activePlayers = participants;
  for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
    if (players[iPlayer].participates)
      players[iPlayer].prepareForNewRound();
}

void Game::endRound()
{
  MessageBox(0, L"Good game :-)", L"The end", 0);
  exit(0);
}

void Game::onGlobalKeyPress(GlobalKey key) { }

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
    else if (!keyPressed(globalControls.keyArray[key]))
      nextGlobalKeyActivationTable[key]  = currentTime;
  }
  
  for (size_t iPlayer = 0; iPlayer < activePlayers.size(); ++iPlayer)
  {
    for (int key = 0; key < N_PLAYER_KEYS; ++key)
    {
      if (keyPressed(activePlayers[iPlayer]->controls.keyArray[key]) &&
          (currentTime >= activePlayers[iPlayer]->nextKeyActivationTable[key]))
      {
        activePlayers[iPlayer]->onKeyPress(PlayerKey(key));
        activePlayers[iPlayer]->nextKeyActivationTable[key] = currentTime + PLAYER_KEY_REACTIVATION_TIME[key];
      }
      else if (!keyPressed(activePlayers[iPlayer]->controls.keyArray[key]))
        activePlayers[iPlayer]->nextKeyActivationTable[key] = currentTime;
    }
  }
  
  for (size_t iPlayer = 0; iPlayer < activePlayers.size(); ++iPlayer)
    activePlayers[iPlayer]->onTimer();
  
  /*for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
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
      players[iPlayer].onTimer();*/
}



void Game::loadPieces()   // TODO: rewrite it cleaner
{
  SmartFileHandler piecesFile(PIECE_TEMPLATES_FILE.c_str(), L"r");
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
      pieceTemplates[iPiece].structure[rotationState].setBounds();
    }
  }

  randomPieceTable.clear();
  for (size_t iPiece = 0; iPiece < pieceTemplates.size(); ++iPiece)
    for (int i = 0; i < pieceTemplates[iPiece].chance; ++i)
      randomPieceTable.push_back(iPiece);
}

void Game::loadBonuses()
{
  for (Bonus bonus = FIRST_REAL_BONUS; bonus <= LAST_REAL_BONUS; ++bonus)
    for (int i = 0; i < BONUS_CHANCES[bonus]; ++i)
      randomBonusTable.push_back(bonus);
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
  statistics.clear();
  events.clear();
  fieldLocks.clear();
  buffs.clear();
  debuffs.clear();
  field.clear();
  lyingBlockImages.clear();
  lyingBlockIndices.clear();
  fallingPieceFrame.parent = NULL;  // TODO: move to initialization?
  fallingBlockImages.clear();
  disappearingLines.clear();
  visualEffects.clear();
  visualEffects.lantern.parent = NULL;  // TODO: move to initialization?
  visualEffects.lantern.bindTo(&fallingPieceFrame);  // TODO: move to initialization?
  visualEffects.lantern.placeAt(FloatFieldCoords((FIELD_HEIGHT - 1.0f) / 2.0f, (FIELD_WIDTH  - 1.0f) / 2.0f));
  visualEffects.lantern.maxSpeed = BONUS_LANTERN_MAX_SPEED;
  latestLineCollapse = NEVER_HAPPENED;
  victimNumber = number;
  cycleVictim();
  speed = STARTING_SPEED;

  backgroundSeed = rand();
  events.push(etRoutineSpeedUp, currentTime() + ROUTINE_SPEED_UP_INTERVAL);
  initPieceQueue(NORMAL_HINT_QUEUE_SIZE);
  planBonusAppearance();
  sendNewPiece();
}

Time Player::currentTime()
{
  return game->currentTime;
}

Time Player::pieceLoweringInterval()
{
  return AUTO_LOWERING_TIME / speed;
}

Player* Player::victim() const
{
  return (victimNumber != number) ? &game->players[victimNumber] : NULL;
}

wstring Player::victimName() const
{
  return victim() ? victim()->account()->name : L"[nobody]";
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
  {
    switch (bonus)
    {
    case bnEnlargeHintQueue:
      resizePieceQueue(BONUS_ENLARGED_HINT_QUEUE_SIZE);
      break;
    case bnPieceTheft:
      break;
    SKIP_ALL_BUT_BUFFS;
    }
    buffs.add(bonus);
  }
  else if (isDebuff(bonus))
  {
    switch (bonus)
    {
    // ...
    SKIP_ALL_BUT_DEBUFFS;
    }
    debuffs.add(bonus);
  }
  else
  {
    switch (bonus)
    {
    case bnHeal:
      events.push(etHeal, currentTime());
//      heal();
      break;
    case bnSlowDown:
      bonusSlowDown();
      break;
    case bnClearField:
      events.push(etBeginClearField, currentTime());
      break;
    case bnSpeedUp:
      bonusSpeedUp();
      break;
//    case bnFlipField:
//      // ...
//      break;
    SKIP_ALL_BUT_SORCERIES;
    }
  }
  enableBonusVisualEffect(bonus);
}

void Player::disenchant(Bonus bonus)
{
  assert(isEnchantment(bonus));
  if (isBuff(bonus))
  {
    switch (bonus)
    {
    case bnEnlargeHintQueue:
      resizePieceQueue(NORMAL_HINT_QUEUE_SIZE);
      break;
    case bnPieceTheft:
      // ...
      break;
    // ...
    SKIP_ALL_BUT_BUFFS;
    }
    buffs.remove(bonus);
  }
  else if (isDebuff(bonus))
  {
    switch (bonus)
    {
    // ...
    SKIP_ALL_BUT_DEBUFFS;
    }
    debuffs.remove(bonus);
  }
  disableBonusVisualEffect(bonus);
}

void Player::heal()
{
  // optimize disabling all effects (?)
  for (Bonus i = FIRST_DEBUFF; i <= LAST_DEBUFF; ++i)
    if (debuffs.test(i))
      disenchant(i);
}

void Player::beginClearField()
{
  //  TODO: also handle a case when the bonus is taken somehow but a piece is still falling (?)
  fieldLocks.isBeingCleared = true;
  field.clear();
  visualEffects.fieldCleaning.enable(BONUS_CLEAR_FIELD_DURATION);
  events.push(etEndClearField, currentTime() + BONUS_CLEAR_FIELD_DURATION);
}

void Player::endClearField()
{
  fieldLocks.isBeingCleared = false;
  lyingBlockImages.clear();
  lyingBlockIndices.clear();
}

void Player::kill()
{
  for (vector<Player*>::iterator i = game->activePlayers.begin(); i != game->activePlayers.end(); ++i)
  {
    if ((*i)->victimNumber == number)
    {
      (*i)->cycleVictim();
      if ((game->activePlayers.size() >= 2) && ((*i)->victimNumber == (*i)->number))
        (*i)->cycleVictim();
    }
  }

  for (vector<Player*>::iterator i = game->activePlayers.begin(); i != game->activePlayers.end(); ++i)
  {
    if (*i == this)
    {
      game->activePlayers.erase(i);
      break;
    }
  }

  if (game->activePlayers.empty())
    game->endRound();

  visualEffects.playerDying.enable(PLAYER_DYING_ANIMATION_TIME);
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
    lowerPiece(true /* forced */);
    break;
  case keyDrop:
    dropPiece();
    break;
  case keyNextVictim:
    cycleVictim();
    break;
  }
}

/*void Player::onTimer()
{
  while ((!events.empty()) && (currentTime() >= events.top().activationTime))
  {
    EventSet::iterator curentEvent = events.topIterator();
    bool eventDelayed = false;
    switch (events.top().type)
    {
    case etPieceLowering:
      lowerPiece(false / auto /);
      break;
    case etLineCollapse:
      collapseLine(events.top().parameters.lineCollapse.row);
      break;
    case etNewPiece:
      if (!sendNewPiece())
        eventDelayed = true;
//      if (canSendNewPiece())
//        sendNewPiece();
//      else
//        eventDelayed = true;
      break;
    case etRoutineSpeedUp:   // TODO: [FIXED?] why is it called in the very beginning?
      routineSpeedUp();
      break;
    case etBonusAppearance:
      if (!generateBonus())
        eventDelayed = true;
      break;
    case etBonusDisappearance:
      removeBonuses();  // TODO: remove only one (?)
      break;
    }
    if (eventDelayed)
      events.delay(curentEvent);
    else
      events.erase(curentEvent);
  }
  redraw();
}*/

void Player::onTimer()
{
  while ((!events.empty()) && (currentTime() >= events.top().activationTime))
  {
    Event currentEvent = events.top();
    events.pop();
    bool eventDelayed = false;
    switch (currentEvent.type)
    {
    case etPieceLowering:
      lowerPiece(false /* auto */);
      break;
    case etLineCollapse:
      collapseLine(currentEvent.parameters.lineCollapse.row);
      break;
    case etNewPiece:
      if (!sendNewPiece())
        eventDelayed = true;
      break;
    case etRoutineSpeedUp:   // TODO: [FIXED?] why is it called in the very beginning?
      routineSpeedUp();
      break;
    case etBonusAppearance:
      if (!generateBonus())
        eventDelayed = true;
      break;
    case etBonusDisappearance:
      removeBonuses();   // TODO: remove only one (?)
      break;
    case etHeal:
      heal();
      break;
    case etBeginClearField:
      beginClearField();
      break;
    case etEndClearField:
      endClearField();
      break;
    case etKill:
      kill();
      break;
    default:
      throw ERR_INTERNAL_ERROR;  // TODO: formal  "Events queue crashed (event %d found)"
    }
    if (eventDelayed)
    {
      currentEvent.activationTime += EVENT_DELAY_TIME;
      events.push(currentEvent);
    }
  }
}



int Player::highestNonemptyRow() const
{
  for (int row = FIELD_HEIGHT - 1; row >= 0; --row)
    for (int col = 0; col < FIELD_WIDTH; ++col)
      if (field(row, col).isBlocked())
        return row;
  return -1;
}

bool Player::canDisposePiece(FieldCoords position, const BlockStructure& piece) const
{
  for (size_t i = 0; i < piece.blocks.size(); ++i)
    if (field(position + piece.blocks[i]).isBlocked())
      return false;
  return true;
}

bool Player::fallingPieceCannotReachSky() const
{
  for (int i = 0; i < N_PIECE_ROTATION_STATES; ++i)
    if (fallingPiece.pieceTemplate->structure[i].bounds.top + fallingPiece.position.row >= FIELD_HEIGHT)
      return false;
  return true;
}

bool Player::canSendNewPiece() const
{
  return disappearingLines.empty() && !fieldLocks.isBeingCleared;
}

Piece Player::randomPiece() const
{
  Piece piece;
  piece.pieceTemplate = &game->pieceTemplates[game->randomPieceTable[rand() % game->randomPieceTable.size()]];
  piece.rotationState = rand() % N_PIECE_ROTATION_STATES;
  piece.position.row = FIELD_HEIGHT - piece.currentStructure().bounds.bottom;
  piece.position.col = MAX_PIECE_SIZE + rand() % (FIELD_WIDTH - 2 * MAX_PIECE_SIZE); // TODO: modify the formula
  return piece;
}

bool Player::bonusIsUseful(Bonus bonus) const
{
  // TODO: process all debuffs uniformly  (may be, they are just always useful?)
  if (isBuff(bonus))
    return !buffs.test(bonus);

  switch (bonus)
  {
  case bnHeal:
    return debuffs.any();
    break;
  case bnSlowDown:
    return speed > STARTING_SPEED + BONUS_SLOW_DOWN_VALUE;  // (?)
    break;
  case bnClearField:
    return highestNonemptyRow() > BONUS_HIGHEST_LINE_MAKING_CLEARING_USEFUL;
    break;

  case bnFlippedScreen:
    // ...
    break;
  case bnRotatingScreen:
    // ...
    break;
  case bnWave:
    // ...
    break;
  case bnLantern:
    // ...
    break;
  case bnCrazyPieces:
    // ...
    break;
  case bnTruncatedBlocks:
    // ...
    break;
  case bnNoHint:
    // ...
    break;

  case bnSpeedUp:
    // ...
    break;

  SKIP_BUFFS;
  }

  return true;
}

Bonus Player::randomBonus() const
{
  Bonus bonus;
  for (int iAttempt = 0; iAttempt < N_BONUS_CHOOSE_ATTEMPTS; ++iAttempt) {
    bonus = game->randomBonusTable[rand() % game->randomBonusTable.size()];
    if (bonusIsUseful(bonus))
      return bonus;
  }
  return bnNoBonus;
}

void Player::setUpPiece()
{
  for (size_t i = 0; i < fallingPiece.nBlocks(); ++i)
  {
    FieldCoords cell = fallingPiece.absoluteCoords(i);
    if (cell.row >= FIELD_HEIGHT)
    {
      // Don't let the field borders to get spoilt!
      events.push(etKill, currentTime());
      return;
    }

    field(cell).setBlock(fallingPiece.color());

    // TODO: copy images from one array to another with motion (?)

    lyingBlockImages.push_back(BlockImage(NULL, fallingPiece.color(), cell));
    lyingBlockIndices.insert(make_pair(cell, lyingBlockImages.size() - 1));
  }
  fallingBlockImages.clear();
  fallingPieceState = psAbsent;
  
  bool fullLinesFound = removeFullLines();
  Time newPieceDelay = fullLinesFound ? myMax(HINT_MATERIALIZATION_TIME, LINE_DISAPPEAR_TIME) :
                                        HINT_MATERIALIZATION_TIME;
  events.pushWithUniquenessCheck(etNewPiece, currentTime() + newPieceDelay);
  visualEffects.hintMaterialization.enable(newPieceDelay);
  visualEffects.hint.enable(HINT_APPERAING_TIME);
//  visualEffects.lantern.resetBinding();
}

void Player::initPieceQueue(int size)
{
  nextPieces.clear();
  resizePieceQueue(size);
}

void Player::resizePieceQueue(int newSize)
{
  int oldSize = nextPieces.size();
  nextPieces.resize(newSize);
  if (newSize > oldSize)
    for (int i = oldSize; i < newSize; ++i)
      nextPieces[i] = randomPiece();
}

bool Player::sendNewPiece()
{
  if (!canSendNewPiece())
    return false;

  fallingPiece = nextPieces[0];
  assert(!fallingPiece.empty());

  visualEffects.hint.disable();
  visualEffects.hintMaterialization.disable();

  fallingPieceState = psNormal;
  fallingPieceFrame.placeAt(fallingPiece.position);
  for (size_t i = 0; i < fallingPiece.nBlocks(); ++i)
    fallingBlockImages.push_back(BlockImage(&fallingPieceFrame, fallingPiece.color(), fallingPiece.relativeCoords(i)));
//  visualEffects.lantern.bindTo(&fallingPieceFrame);
  events.push(etPieceLowering, currentTime() + pieceLoweringInterval());

  nextPieces.erase(nextPieces.begin());
  nextPieces.push_back(randomPiece());
  /*for (size_t i = 0; i < nextPieces.size() - 1; ++i)
    nextPieces[i] = nextPieces[i + 1];
  nextPieces.back() = randomPiece();*/
  return true;
}

void Player::lowerPiece(bool forced)
{
  if (fallingPieceState == psAbsent)
    return;

  FieldCoords newPosition = fallingPiece.position + FieldCoords(-1, 0);
  // We should not forget about the case when a piece gets stuck as the result of
  // field modification. Such pieces should not continue falling.
  if (canDisposePiece(fallingPiece.position, fallingPiece.currentStructure()) &&
      canDisposePiece(newPosition,           fallingPiece.currentStructure()))
  {
    Time loweringTime = (fallingPieceState == psDropping) ? DROPPING_PIECE_LOWERING_TIME :
                        (forced ? PIECE_FORCED_LOWERING_ANIMATION_TIME :
                                  PIECE_AUTO_LOWERING_ANIMATION_TIME);
    fallingPieceFrame.addMotion(newPosition - fallingPiece.position, currentTime(), loweringTime);

    fallingPiece.position = newPosition;

    if (fallingPieceCannotReachSky())
      visualEffects.hint.enable(HINT_APPERAING_TIME);

    events.push(etPieceLowering,
                currentTime() +
                ((fallingPieceState == psDropping) ?
                 DROPPING_PIECE_LOWERING_TIME : pieceLoweringInterval()));
  }
  else
    setUpPiece();
}

// TODO: Optimize Player::removeFullLines: don't check all lines
// TODO: Does Player::removeFullLines have to return a bool any more?
bool Player::removeFullLines()
{
  bool fullLinesExisted = false;

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
      fullLinesExisted = true;
      ++statistics.lineCleared;

      disappearingLines.resize(disappearingLines.size() + 1);
      disappearingLines.back().startTime = currentTime();
      disappearingLines.back().duration = LINE_DISAPPEAR_TIME;
      disappearingLines.back().row = row;
      for (int col = 0; col < FIELD_WIDTH; ++col)
      {
        if (field(row, col).bonus != bnNoBonus)
        {
          takesBonus(field(row, col).bonus);
          events.eraseEventType(etBonusDisappearance);
          planBonusAppearance();
        }
        disappearingLines.back().blockColor[col] = field(row, col).color;
        field(row, col).clear();

        lyingBlockImages[lyingBlockIndices[FieldCoords(row, col)]] = lyingBlockImages.back();
        // TODO: No, *that* was not the point of  lyingBlockIndices !
        for (map<FieldCoords, int>::iterator it = lyingBlockIndices.begin(); it != lyingBlockIndices.end(); ++it)
        {
          if (it->second == lyingBlockImages.size() - 1) {
            it->second = lyingBlockIndices[FieldCoords(row, col)];
            break;
          }
        }
        lyingBlockImages.pop_back();
        lyingBlockIndices.erase(FieldCoords(row, col));
      }

      if (latestLineCollapse < currentTime() + LINE_DISAPPEAR_TIME)
        latestLineCollapse = currentTime() + LINE_DISAPPEAR_TIME;
      latestLineCollapse += LINE_COLLAPSE_TIME;
      Event event(etLineCollapse, latestLineCollapse);
      event.parameters.lineCollapse.row = row;
      events.push(event);
    }
  }

  return fullLinesExisted;
}

void Player::collapseLine(int row)  // TODO: optimize
{
  for (int curRow = row; curRow < FIELD_HEIGHT; ++curRow)
  {
    for (int col = 0; col < FIELD_WIDTH; ++col)
    {
      field(curRow, col) = field(curRow + 1, col);
      if (field(curRow, col).isBlocked())
        moveLyingBlockImage(FieldCoords(curRow + 1, col), FieldCoords(curRow, col), LINE_COLLAPSE_ANIMATION_TIME);
    }
  }

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
  FieldCoords newPosition = fallingPiece.position + FieldCoords(0, direction);
  if (canDisposePiece(newPosition, fallingPiece.currentStructure()))
  {
    fallingPieceFrame.addMotion(newPosition - fallingPiece.position, currentTime(), PIECE_MOVING_ANIMATION_TIME);
    fallingPiece.position = newPosition;
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

  // TODO: simply create a  Piece  copy instead (?)
  FieldCoords oldPosition = fallingPiece.position;
  int oldRotationState = fallingPiece.rotationState;
  int newFallingPieceRotationState = (fallingPiece.rotationState + N_PIECE_ROTATION_STATES + direction) %
                                     N_PIECE_ROTATION_STATES;

  if (canDisposePiece(fallingPiece.position,
      fallingPiece.pieceTemplate->structure[newFallingPieceRotationState]))
  {
    fallingPiece.rotationState = newFallingPieceRotationState;
  }
  else if (canDisposePiece(fallingPiece.position + FieldCoords(0, 1),  // TODO: optimize
      fallingPiece.pieceTemplate->structure[newFallingPieceRotationState]))
  {
    fallingPiece.position += FieldCoords(0, 1);
    fallingPiece.rotationState = newFallingPieceRotationState;
  }
  else if (canDisposePiece(fallingPiece.position - FieldCoords(0, 1),  // TODO: optimize
      fallingPiece.pieceTemplate->structure[newFallingPieceRotationState]))
  {
    fallingPiece.position -= FieldCoords(0, 1);
    fallingPiece.rotationState = newFallingPieceRotationState;
  }

  for (size_t i = 0; i < fallingPiece.nBlocks(); ++i)
  {
    fallingBlockImages[i].addMotion(
            fallingPiece.absoluteCoords(i) -
            (oldPosition + fallingPiece.pieceTemplate->structure[oldRotationState].blocks[i]),
            currentTime(), PIECE_ROTATING_ANIMATION_TIME);
  }
}

bool Player::generateBonus()  // TODO: remake
{
  Bonus bonus = randomBonus();  // Is that possible (?)
  if (bonus == bnNoBonus)
    return false;

  for (int iter = 0; iter < N_BONUS_GENERATION_ATTEMPTS; ++iter)
  {
    for (int row = highestNonemptyRow(); row >= 0; --row)
    {
      if (float(rand()) / RAND_MAX > BONUS_ONE_ROW_CHANCE)
        continue;
      for (int colIter = 0; colIter < N_BONUS_ONE_ROW_ATTEMPTS; ++colIter)
      {
        int col = rand() % FIELD_WIDTH;
        if (field(row, col).isBlocked())
        {
          field(row, col).bonus = bonus;
          lyingBlockImages[lyingBlockIndices[FieldCoords(row, col)]].bonus = bonus;
          lyingBlockImages[lyingBlockIndices[FieldCoords(row, col)]].bonusImage.enable(BONUS_FADING_DURATION);
          planBonusDisappearance(FieldCoords(row, col));
          return true;
        }
      }
    }
  }
  return false;
}

void Player::removeBonuses()
{
  for (int row = 0; row < FIELD_HEIGHT; ++row)
  {
    for (int col = 0; col < FIELD_WIDTH; ++col)
    {
      if (field(row, col).isBlocked())
      {
        field(row, col).bonus = bnNoBonus;
        lyingBlockImages[lyingBlockIndices[FieldCoords(row, col)]].bonusImage.disable();
      }
    }
  }
  planBonusAppearance();
}

void Player::planBonusAppearance()
{
  events.push(etBonusAppearance, currentTime() + randomRange(MIN_BONUS_APPEAR_TIME, MAX_BONUS_APPEAR_TIME));
}

void Player::planBonusDisappearance(FieldCoords bonusCoords)
{
  Time bonusDisappearTime = currentTime() + randomRange(MIN_BONUS_LIFE_TIME, MAX_BONUS_LIFE_TIME);
  events.push(etBonusDisappearance, bonusDisappearTime);
  lyingBlockImages[lyingBlockIndices[bonusCoords]].bonusImage.stopAt(
          bonusDisappearTime - BONUS_FADING_DURATION);
}

/*void Player::applyBlockImagesMovements(vector<BlockImage>& imageArray)
{
  for (int row = BORDERED_FIELD_ROW_BEGIN; row < BORDERED_FIELD_ROW_END; ++row)
    for (int col = BORDERED_FIELD_COL_BEGIN; col < BORDERED_FIELD_COL_END; ++col)
      if (field(row, col).iNewBlockImage != NO_CHANGE)
      {
        field(row, col).iBlockImage = field(row, col).iNewBlockImage;
        field(row, col).iNewBlockImage = NO_CHANGE;
      }
}

void Player::addStandingBlockImage(vector<BlockImage>& imageArray, VisualObject* parent,
                                   Color color, FieldCoords position)
{
  imageArray.resize(imageArray.size() + 1);
  imageArray.back().placeNewImageAt(parent, color, position);
  field(position).iBlockImage = imageArray.size() - 1;
}

void Player::moveBlockImage(vector<BlockImage>& imageArray, FieldCoords movingFrom,
                            FieldCoords movingTo, Time movingDuration)
{
  if (movingFrom == movingTo)
    return;
  imageArray[field(movingFrom).iBlockImage].moveTo(movingTo, currentTime(), movingDuration);
  field(movingTo).iNewBlockImage = field(movingFrom).iBlockImage;
  if (field(movingFrom).iNewBlockImage == NO_CHANGE)
    field(movingFrom).iNewBlockImage = NO_BLOCK_IMAGE;
}

void Player::removeBlockImage(vector<BlockImage>& imageArray, FieldCoords position)
{
  if (field(position).iBlockImage == imageArray.size() - 1)
  {
    field(position).iBlockImage = NO_BLOCK_IMAGE;
  }
  else
  {
    imageArray[field(position).iBlockImage] = imageArray.back();
    field(imageArray.back().bindingCell).iBlockImage = field(position).iBlockImage;
    field(position).iBlockImage = NO_BLOCK_IMAGE;
  }
  imageArray.erase(imageArray.end() - 1);
}*/

void Player::moveLyingBlockImage(FieldCoords movingFrom, FieldCoords movingTo, Time movingDuration) {
  if (movingFrom == movingTo)
    return;
  lyingBlockImages[lyingBlockIndices[movingFrom]].addMotion(movingTo - movingFrom, currentTime(), movingDuration);
  lyingBlockIndices[movingTo] = lyingBlockIndices[movingFrom];
  lyingBlockIndices.erase(movingFrom);
}

void Player::routineSpeedUp()
{
  float maxSpeed = myMax(NORMAL_SPEED_LIMIT, speed);
  speed += ROUTINE_SPEED_UP_VALUE;
  speed = myMin(speed, maxSpeed);
  events.push(etRoutineSpeedUp, currentTime() + ROUTINE_SPEED_UP_INTERVAL);
}

void Player::bonusSpeedUp()
{
  speed += BONUS_SPEED_UP_VALUE;
  speed = myMin(speed, ABSOLUTE_SPEED_LIMIT);
}

void Player::bonusSlowDown()
{
  speed -= BONUS_SLOW_DOWN_VALUE;
  speed = myMax(speed, STARTING_SPEED);
}

void Player::cycleVictim()
{
  if (game->activePlayers.size() < 2)
    return;
  do
  {
    victimNumber = (victimNumber + 1) % MAX_PLAYERS;
  } while (!game->players[victimNumber].active);
}

void Player::enableBonusVisualEffect(Bonus bonus)
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
    // (!) It is enabled in beginClearField().  May be the conceptions needs changing
    //visualEffects.fieldCleaning.enable(BONUS_CLEAR_FIELD_DURATION / 2);  // (!) Change if effect type is changed
    break;
  case bnFlippedScreen:
    visualEffects.flippedScreen.enable(BONUS_FLIPPING_SCREEN_DURATION);
    break;
  case bnRotatingScreen:
    visualEffects.rotatingField.enable(BONUS_ROTATING_SCREEN_PERIOD);
    break;
  case bnWave:
    visualEffects.wave.enable(BONUS_WAVE_PERIOD);
    break;
  case bnLantern:
    visualEffects.lantern.enable(BONUS_LANTERN_FADING_TIME);
    break;
  case bnCrazyPieces:
    // no effect
    break;
  case bnTruncatedBlocks:
    visualEffects.semicubes.enable(BONUS_CUTTING_BLOCKS_DURATION);
    break;
  case bnNoHint:
    visualEffects.noHint.enable(BONUS_REMOVING_HINT_DURATION);
    break;
  case bnSpeedUp:
    // no effect
    break;
//  case bnFlipField:
//    // no effect *at this moment*
//    break;
  case bnNoBonus: ;
  }
}

void Player::disableBonusVisualEffect(Bonus bonus)
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
    // the effect ends itself
    break;
  case bnFlippedScreen:
    visualEffects.flippedScreen.disable();
    break;
  case bnRotatingScreen:
    visualEffects.rotatingField.disable();
    break;
  case bnWave:
    visualEffects.wave.disable();
    break;
  case bnLantern:
    visualEffects.lantern.disable();
    break;
  case bnCrazyPieces:
    // no effect
    break;
  case bnTruncatedBlocks:
    visualEffects.semicubes.disable();
    break;
  case bnNoHint:
    visualEffects.noHint.disable();
    break;
  case bnSpeedUp:
    // no effect
    break;
//  case bnFlipField:
//    // no effect *at this moment*
//    break;
  case bnNoBonus: ;
  }
}

void Player::stealPiece()
{
  if (victim() == NULL)
    return;
  PieceTheftEffect pieceTheftEffect;
  pieceTheftEffect.enable(currentTime());
  pieceTheftEffect.sender = number;
  pieceTheftEffect.target = victimNumber;
  game->globalEffects.pieceThefts.push_back(pieceTheftEffect);
  visualEffects.pieceTheftPtr = &game->globalEffects.pieceThefts.back();
}

void Player::flipBlocks()
{
  // TODO: implement  Player::flipBlocks()  OR  remove it
}
