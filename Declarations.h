#ifndef CRAZYTETRIS_DECLARATIONS_H
#define CRAZYTETRIS_DECLARATIONS_H

#include <climits>
#include <vector>

typedef int Time;
const int TICKS_PER_SECOND = 1000;
const Time NEVER = INT_MIN;

typedef int Color; // value or index? (?)

const int N_CONTROL_KEY = 8;
enum ControlKey { keyLeft, keyRight, keyRotateCW, keyRotateCCW, keyDown, keyDrop,
                  keyHeal, keyChangeTarget, keyUnassigned };

struct FieldCoords {
  int x, y;
};

template <typename T, int nRows, int nCols>
class Table : public std::vector<std::vector<T> > {
public:
  Table() {
    std::vector<T>::resize(nRows);
    for (int i = 0; i < std::vector<T>::size(); ++i)
      std::vector<T>::operator[](i).resize(nCols);
  }
};

#endif
