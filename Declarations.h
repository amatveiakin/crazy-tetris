#ifndef CRAZYTETRIS_DECLARATIONS_H
#define CRAZYTETRIS_DECLARATIONS_H

#include <cstdio>
#include <bitset>
#include "d3dapp/d3dUtil.h" // temporary



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



const int    FIELD_WIDTH = 10;
const int    FIELD_HEIGHT = 20;
const int    MAX_BLOCKS = FIELD_WIDTH * FIELD_HEIGHT;
const int    MAX_BLOCK_IMAGES = MAX_BLOCKS;



typedef float Time;
const Time NEVER = -1000.0f;

#define Color D3DXCOLOR
const Color COLORLESS = Color(-1.0, -1.0, -1.0, -1.0);



/*typedef int BlockID;

inline BlockID FieldCoordsToID(FieldCoords coords)
{
  return coords.row * FIELD_WIDTH + coords.row;
}*/



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
};

typedef Coord2D<int> FieldCoords;

//typedef Coord2D<float> FloatFieldCoords;

struct FloatFieldCoords : public Coord2D<float> // (?) What's happening here?!!
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
  
  bool check(size_t position) const
  {
    assert(firstElementsNumber <= position);
    assert(position < nElements + firstElementsNumber);
    return std::bitset<nElements>::operator[](position - firstElementsNumber);
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
  SmartFileHandler(const char* path, const char* mode)
  {
//    file_handle_ = fopen(path, mode);
    fopen_s(&file_handle_, path, mode);   // TODO: So, why is it safer?
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
