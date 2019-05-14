// Copyright 2009 Green Code LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef MCC_GRID_H
#define MCC_GRID_H

#include <boost/multi_array.hpp>
#include "GridBase.h"

namespace mcc
{
  // A grid of values (one per rectangular cell).
  template <typename T>
  class Grid : public GridBase
  {
    public:
      Grid<T>(unsigned int          rows,
              unsigned int          columns,
              const XYCoordinates & lowerLeft,
              Coordinate            cellHeight,
              Coordinate            cellWidth);

      // Access the value for a cell.
      T & operator[](const Cell & cell);
      const T & operator[](const Cell & cell) const;

      // Access the value for a cell by its row and column.
      T & operator()(unsigned int row,
                     unsigned int column);
      const T & operator()(unsigned int row,
                           unsigned int column) const;

      // Specify and retrieve a NODATA value for the grid.
      void setNoDataValue(const T & value);
      const T & noDataValue() const;

      // Does the grid have a NODATA value?
      bool hasNoDataValue() const;

      // Get the data value for a cell if it's not the NODATA value.
      bool getData(const Cell & cell,
                   T &          value) const;
      bool getData(unsigned int row,
                   unsigned int column,
                   T &          value) const;

    private:
      boost::multi_array<T, 2> values_;
      bool hasNoDataValue_;
      T noDataValue_;
  };

  //---------------------------------------------------------------------------

  template <typename T>
  Grid<T>::Grid(unsigned int          rows,
                unsigned int          columns,
                const XYCoordinates & lowerLeft,
                Coordinate            cellHeight,
                Coordinate            cellWidth)
    : GridBase(rows, columns, lowerLeft, cellHeight, cellWidth),
      values_(boost::extents[rows][columns]),
      hasNoDataValue_(false)
  {
  }

  template <typename T>
  inline
  T & Grid<T>::operator[](const Cell & cell)
  {
    return values_[cell.row()][cell.column()];
  }

  template <typename T>
  inline
  const T & Grid<T>::operator[](const Cell & cell) const
  {
    return values_[cell.row()][cell.column()];
  }

  template <typename T>
  inline
  T & Grid<T>::operator()(unsigned int row,
                          unsigned int column)
  {
    return values_[row][column];
  }

  template <typename T>
  inline
  const T & Grid<T>::operator()(unsigned int row,
                                unsigned int column) const
  {
    return values_[row][column];
  }

  template <typename T>
  inline
  void Grid<T>::setNoDataValue(const T & value)
  {
    noDataValue_ = value;
    hasNoDataValue_ = true;
  }

  template <typename T>
  inline
  const T & Grid<T>::noDataValue() const
  {
    return noDataValue_;
  }

  template <typename T>
  inline
  bool Grid<T>::hasNoDataValue() const
  {
    return hasNoDataValue_;
  }

  template <typename T>
  inline
  bool Grid<T>::getData(const Cell & cell,
                        T &          value) const
  {
    return getData(cell.row(), cell.column(), value);
  }

  template <typename T>
  bool Grid<T>::getData(unsigned int row,
                        unsigned int column,
                        T &          value) const
  {
    const T & cellValue = operator()(row, column);
    if (hasNoDataValue_ && cellValue == noDataValue_) {
      return false;
    }
    else {
      value = cellValue;
      return true;
    }
  }

}

#endif
