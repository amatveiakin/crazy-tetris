#ifndef CRAZYTETRIS_DECLARATIONS_H
#define CRAZYTETRIS_DECLARATIONS_H

#include <climits>
#include <vector>



typedef float Time;
const Time NEVER = -1.0f;



struct Color
{
  float r, g, b, a;
};



const int N_CONTROL_KEY = 7;
enum ControlKey { keyLeft, keyRight, keyRotateCW, keyRotateCCW, keyDown, keyDrop,
                  keyChangeVictim, keyUnassigned };
                  
typedef int RealKey;

bool playerKey(ControlKey key)
{
  return (key >= keyLeft) && (key <= keyChangeVictim);
}

bool systemKey(ControlKey key)
{
  return false;
}



struct FieldCoords
{
  int x, y;
};

template <typename T, int nRows, int nCols>
class Table : public std::vector<std::vector<T> >
{
public:
  Table()
  {
    std::vector<T>::resize(nRows);
    for (int i = 0; i < std::vector<T>::size(); ++i)
      std::vector<T>::operator[](i).resize(nCols);
  }
};

#endif
