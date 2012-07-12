#ifndef CRAZYTETRIS_DECLARATIONS_H
#define CRAZYTETRIS_DECLARATIONS_H

#include <cstdio>
#include <bitset>
#include "d3dapp/d3dUtil.h"



template<typename T>
T myMin(T x, T y)
{
  return (x < y) ? x : y;
}

template<typename T>
T myMax(T x, T y)
{
  return (x > y) ? x : y;
}

template<typename T>  // TODO: isn't there a standard function?
T boundValue(T value, T minValue, T maxValue)
{
  return (value < minValue) ? minValue : ((value > maxValue) ? maxValue : value);
}

template<typename T>
T mySqr(T x)
{
  return x * x;
}



const int    FIELD_WIDTH = 10;
const int    FIELD_HEIGHT = 20;
const int    MAX_BLOCKS = FIELD_WIDTH * FIELD_HEIGHT;
const int    MAX_BLOCK_IMAGES = MAX_BLOCKS;

const int    MAX_PIECE_SIZE = 4;
const int    MAX_BLOCKS_IN_PIECE = MAX_PIECE_SIZE * MAX_PIECE_SIZE;

const int    MAX_PLAYERS = 4;



typedef float Time;
const Time NEVER_HAPPENED = -1e30f;  // TODO: replace with MINFLOAT
const Time WILL_NEVER_HAPPEN = 1e30f;  // TODO: replace with MAXFLOAT
// TODO: declare Speed type too



#define Color D3DXCOLOR
const Color COLORLESS = Color(-1.0, -1.0, -1.0, -1.0);



enum Bonus
{
  //
  bnNoBonus,

  // buffs
  bnEnlargeHintQueue,
  bnPieceTheft,

  // kind sorceries
  bnHeal,
  bnSlowDown,
  bnClearField,

  // debuffs
  bnFlippedScreen,
  bnRotatingScreen,
  bnWave,
  bnLantern,
  bnCrazyPieces,
  bnTruncatedBlocks, // name --> (?)
  bnNoHint,

  // evil sorceries
  bnSpeedUp
//  bnFlipField
};

inline Bonus& operator++(Bonus& bonus)  // (?) Isn't there really a better way?
{
  bonus = Bonus(int(bonus) + 1);
  return bonus;
}

const Bonus  FIRST_BONUS = bnNoBonus;
const Bonus  FIRST_REAL_BONUS = bnEnlargeHintQueue;
const Bonus  LAST_BONUS = bnSpeedUp;
const Bonus  LAST_REAL_BONUS = bnSpeedUp;
const int    N_BONUSES = LAST_BONUS - FIRST_BONUS + 1;
const int    N_REAL_BONUSES = LAST_REAL_BONUS - FIRST_REAL_BONUS + 1;

const std::wstring BONUS_NAME[N_BONUSES] =
{
  L"NoBonus",
  L"EnlargeHintQueue",
  L"PieceTheft",
  L"Heal",
  L"SlowDown",
  L"ClearField",
  L"FlippedScreen",
  L"RotatingScreen",
  L"Wave",
  L"Lantern",
  L"CrazyPieces",
  L"TruncatedBlocks",
  L"NoHint",
  L"SpeedUp"
//  L"FlipField"
};




inline float randomRange(float min__, float max__)
{
  return min__ + float(rand()) * (max__ - min__) / RAND_MAX;
}



// TODO: there must be a ready-to-use class Coords with all operations
template <typename T>
struct Coord2D
{
  T row, col;

  Coord2D() { }
  Coord2D(const Coord2D& a) : row(a.row), col(a.col) { }
  Coord2D(T row__, T col__) : row(row__), col(col__) { }

  bool operator==(const Coord2D& a)
  {
    return (row == a.row) && (col == a.col);
  }

  Coord2D& operator=(const Coord2D& a)
  {
    row = a.row;
    col = a.col;
    return *this;
  }

  Coord2D operator+(const Coord2D& a) const
  {
    return Coord2D(row + a.row, col + a.col);
  }

  Coord2D operator-(const Coord2D& a) const
  {
    return Coord2D(row - a.row, col - a.col);
  }

  Coord2D operator*(T x) const
  {
    return Coord2D(row * x, col * x);
  }

  Coord2D operator/(T x) const
  {
    return Coord2D(row / x, col / x);
  }

  // TODO: find out how it can be declared
  //friend Coord2D operator*(T, const Coord2D&);

  Coord2D& operator+=(const Coord2D& a)
  {
    row += a.row;
    col += a.col;
    return *this;
  }

  Coord2D& operator-=(const Coord2D& a)
  {
    row -= a.row;
    col -= a.col;
    return *this;
  }

  // To use is  std::set
  bool operator<(const Coord2D& a) const
  {
    if (row != a.row)
      return row < a.row;
    else
      return col < a.col;
  }
};

typedef Coord2D<int> FieldCoords;

//typedef Coord2D<float> FloatFieldCoords;

struct FloatFieldCoords : public Coord2D<float> // (?) What's happening here?! Why should I declare constuctors myself?
{
  FloatFieldCoords() { }

  FloatFieldCoords(const FloatFieldCoords& a)
  {
    row = a.row;
    col = a.col;
  }

//  FloatFieldCoords(const FieldCoords a) : row(a.row), col(a.col) { }   //  TODO: find out why this is wrong
  FloatFieldCoords(const FieldCoords& a)
  {
    row = float(a.row);
    col = float(a.col);
  }

  FloatFieldCoords(const Coord2D<float>& a)
  {
    row = float(a.row);
    col = float(a.col);
  }

  FloatFieldCoords(float row__, float col__)
  {
    row = row__;
    col = col__;
  }

  FloatFieldCoords(int row__, int col__)
  {
    row = float(row__);
    col = float(col__);
  }

  float vectorLength() const
  {
    return sqrt(mySqr(row) + mySqr(col));
  }
};



template <typename T, int size>
class FixedZeroBasedArray
{
public:
  T& operator[](int index)
  {
    assert(0 <= index);
    assert(index < size);
    return elements_[index];
  }

  const T& operator[](int index) const
  {
    assert(0 <= index);
    assert(index < size);
    return elements_[index];
  }

protected:
  T elements_[size];
};



template <typename T, int firstRow, int firstCol, int lastRow, int lastCol>
class Fixed2DArray
{
public:
  T& operator()(int row, int col)
  {
    assert(firstRow <= row);
    assert(row <= lastRow);
    assert(firstCol <= col);
    assert(col <= lastCol);
    return elements_[(row - firstRow) * nCols + (col - firstCol)];
  }

  const T& operator()(int row, int col) const
  {
    assert(firstRow <= row);
    assert(row <= lastRow);
    assert(firstCol <= col);
    assert(col <= lastCol);
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
    assert(firstElementsNumber <= position);
    assert(position < nElements + firstElementsNumber);
    std::bitset<nElements>::operator[](position - firstElementsNumber) = true;
  }

  bool test(size_t position) const
  {
    assert(firstElementsNumber <= position);
    assert(position < nElements + firstElementsNumber);
    return std::bitset<nElements>::operator[](position - firstElementsNumber);
  }

  bool any() const
  {
    return std::bitset<nElements>::any();
  }

  bool none() const
  {
    return std::bitset<nElements>::none();
  }

  void remove(size_t position)
  {
    assert(firstElementsNumber <= position);
    assert(position < nElements + firstElementsNumber);
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
  SmartFileHandler(const wchar_t* path, const wchar_t* mode)
  {
//    file_handle_ = wfopen(path, mode);
    _wfopen_s(&file_handle_, path, mode);   // TODO: So, why is it safer?
  }

  ~SmartFileHandler()
  {
    if (file_handle_ != NULL)
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
