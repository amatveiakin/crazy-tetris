#ifndef CRAZYTETRIS_DECLARATIONS_H
#define CRAZYTETRIS_DECLARATIONS_H

#include <bitset>



template <typename T>
T min(T x, T y)
{
  return (x < y) ? x : y;
}

template <typename T>
T max(T x, T y)
{
  return (x > y) ? x : y;
}



const int    FIELD_WIDTH = 10;
const int    FIELD_HEIGHT = 20;



typedef float Time;
const Time NEVER = -1.0f;

struct Color
{
  float r, g, b, a;
};



const int N_CONTROL_KEYS = 7;
enum ControlKey { keyLeft, keyRight, keyRotateCW, keyRotateCCW, keyDown, keyDrop,
                  keyChangeVictim, keyUnassigned };
                  
inline bool playerKey(ControlKey key)
{
  return (key >= keyLeft) && (key < keyUnassigned);
}

inline bool systemKey(ControlKey key)
{
  return false;
}



struct FieldCoords
{
  int row, col;
};

struct FloatFieldCoords
{
  float row, col;
};



/*template <typename T, int nRows, int nCols>
class Table : public std::vector<std::vector<T> >
{
public:
  Table()
  {
    std::vector<T>::resize(nRows);
    for (int i = 0; i < std::vector<T>::size(); ++i)
      std::vector<T>::operator[](i).resize(nCols);
  }
};*/

/*template <typename T, int nElements>
class FixedSizeVector
{
public:
  T& operator[](int index)
  {
    return element_[index];
  }
  const T& operator[](int index) const
  {
    return element_[index];
  }
protected:
  T element_[nElements];
};

template <typename T, int nRows, int nCols>
typedef FixedSizeVector<FixedSizeVector<T, nCols>, nRows> FixedSizeTable;*/

template <typename T, int nRows, int nCols>
class FixedSizeTable
{
public:
  T& operator()(int row, int col)
  {
    return elements_[row * nCols + col];
  }
  const T& operator()(int row, int col) const
  {
    return elements_[row * nCols + col];
  }
protected:
  T elements_[nRows * nCols];
};



template <int nElements, int firstElementsNumber>
class ShiftedBitSet : private std::bitset<nElements>
{
public:
  /*bool operator[](size_t pos) const
  {
    return std::bitset<nElements>::operator[](pos - firstElementsNumber);
  }
  std::bitset<nElements>::reference operator[](size_t pos)
  {
    return std::bitset<nElements>::operator[](pos - firstElementsNumber);
  }*/
  void add(size_t pos)
  {
    std::bitset<nElements>::operator[](pos - firstElementsNumber) = true; // TODO: thers is a mote beautiful way to way it
  }
  bool check(size_t pos) const
  {
    return std::bitset<nElements>::operator[](pos - firstElementsNumber);
  }
  void remove(size_t pos)
  {
    std::bitset<nElements>::operator[](pos - firstElementsNumber) = false; // TODO: thers is a mote beautiful way to way it
  }
  void clear()
  {
    std::bitset<nElements>::reset();
  }
};

#endif
