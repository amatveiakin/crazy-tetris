#ifndef CRAZYTETRIS_DECLARATIONS_H
#define CRAZYTETRIS_DECLARATIONS_H

#include <cstdio>
#include <stdlib.h>
#include <bitset>



typedef float MyReal;

template<typename T>
T myMinThatDoesntMatchMinFromAnyLibrary(T x, T y)
{
  return (x < y) ? x : y;
}

template<typename T>
T myMaxThatDoesntMatchMaxFromAnyLibrary(T x, T y)
{
  return (x > y) ? x : y;
}



const int    FIELD_WIDTH = 10;
const int    FIELD_HEIGHT = 20;
const int    MAX_BLOCKS = FIELD_WIDTH * FIELD_HEIGHT;



typedef float Time;
const Time NEVER = -1000.0f;

struct Color
{
  float r, g, b, a;
};

const Color COLORLESS  = { -1.0, -1.0, -1.0, -1.0 };



const int N_CONTROL_KEYS = 7;
enum ControlKey { keyLeft, keyRight, keyRotateCW, keyRotateCCW, keyDown, keyDrop,
                  keyChangeVictim, keyUnassigned };


const ControlKey FIRST_PLAYER_KEY = keyLeft;
const ControlKey LAST_PLAYER_KEY = keyChangeVictim;

const ControlKey FIRST_GLOBAL_KEY = ControlKey(keyUnassigned + 1);
const ControlKey LAST_GLOBAL_KEY = keyUnassigned;

// To remove warnings in switches
#define SKIP_PLAYER_KEYS  case keyLeft:  case keyRight:  case keyRotateCW: case keyRotateCCW: \
                          case keyDown:  case keyDrop:  case keyChangeVictim:

#define SKIP_GLOBAL_KEYS

#define SKIP_NONPLAYER_KEYS  SKIP_GLOBAL_KEYS  case keyUnassigned:

#define SKIP_NONGLOBAL_KEYS  SKIP_PLAYER_KEYS  case keyUnassigned:


inline bool playerKey(ControlKey key)
{
  return (key >= FIRST_PLAYER_KEY) && (key <= LAST_PLAYER_KEY);
}

inline bool globalKey(ControlKey key)
{
  return (key >= FIRST_GLOBAL_KEY) && (key <= LAST_GLOBAL_KEY);
}



// TODO: there must be a ready-to-use class Coords with all operations
template <typename T>
struct Coord2D
{
  int row, col;
  
  Coord2D() { }
  Coord2D(const Coord2D &a) : row(a.row), col(a.col) { }
  Coord2D(int row__, int col__) : row(row__), col(col__) { }

  /*Coord2D& operator=(const Coord2D& a)
  {
    row = a.row;
    col = a.col;
  }*/

  Coord2D operator+(const Coord2D& a)
  {
    return Coord2D(row + a.row, col + a.col);
  }

  Coord2D operator-(const Coord2D& a)
  {
    return Coord2D(row - a.row, col - a.col);
  }
};

typedef Coord2D<int> FieldCoords;

typedef Coord2D<MyReal> FloatFieldCoords;



template <typename T, int firstRow, int firstCol, int lastRow, int lastCol>
class Fixed2DArray
{
public:
  T& operator()(int row, int col)
  {
    return elements_[(row - firstRow) * nCols + (col - firstCol)];
  }
  
  const T& operator()(int row, int col) const
  {
    return elements_[(row - firstRow) * nCols + (col - firstCol)];
  }
  
protected:
  static const int nRows = lastRow - firstRow + 1;
  static const int nCols = lastCol - firstCol + 1;
  T elements_[nRows * nCols];
};



template <int nElements, int firstElementsNumber>
class ShiftedBitSet : private std::bitset<nElements>
{
public:
  void add(size_t position)
  {
    std::bitset<nElements>::operator[](position - firstElementsNumber) = true;
  }
  
  bool check(size_t position) const
  {
    return std::bitset<nElements>::operator[](position - firstElementsNumber);
  }
  
  void remove(size_t position)
  {
    std::bitset<nElements>::operator[](position - firstElementsNumber) = false;
  }
  
  void clear()
  {
    std::bitset<nElements>::reset();
  }
};



class SmartFileHandler
{
public:
  SmartFileHandler(const char* path, const char* mode)
  {
    file_handle_ = fopen(path, mode);
  }
  
  ~SmartFileHandler()
  {
    fclose(file_handle_);
  }
  
  FILE* get()
  {
    return file_handle_;
  }
  
private:
  FILE* file_handle_;
  // Prevent copying and assignment
  SmartFileHandler(const SmartFileHandler&);
  SmartFileHandler& operator=(const SmartFileHandler&);
};



inline void skipWhitespace(FILE* file)
{
  int ch;
  do
  {
    ch = fgetc(file);
    if (ch == -1)
      return;
  } while (isspace(ch));
  ungetc(ch, file);
}

#endif
