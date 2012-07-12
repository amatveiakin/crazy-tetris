#include "Engine.h"

bool keyPressed(RealKey key)
{
  key = key;
  return false;
}

int main()
{
  Game game;
  try
  {
    game.init();
  }
  catch (string msg)
  {
    printf("%s\n", msg.c_str());
    return 0;
  }
  catch (...)
  {
    printf("Unhandled exception!\n");
    return 0;
  }
  game.newMatch();
  game.newRound(0.0);
  
  return 0;
}