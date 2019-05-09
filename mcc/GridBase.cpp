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

#include <cassert>
#include <cmath>

#include "GridBase.h"

namespace mcc
{
  GridBase::GridBase(unsigned int  rows,
                     unsigned int  columns,
                     XYCoordinates lowerLeft,
                     Coordinate    cellHeight,
                     Coordinate    cellWidth)
    : rows_(rows),
      columns_(columns),
      lowerLeft_(lowerLeft),
      cellHeight_(cellHeight),
      cellWidth_(cellWidth),
      rowCenters_(rows),
      rowEdges_(rows + 1),
      columnCenters_(columns),
      columnEdges_(columns + 1)
  {
    typedef std::vector<Coordinate>::size_type index;

    Coordinate cellHalfHeight = cellHeight/2;
    for (index i = 0; i < rows; ++i) {
      rowEdges_[i] = lowerLeft.y + i * cellHeight;
      rowCenters_[i] = rowEdges_[i] + cellHalfHeight;
    }
    rowEdges_[rows] = lowerLeft.y + rows * cellHeight;

    Coordinate cellHalfWidth = cellWidth/2;
    for (index i = 0; i < columns; ++i) {
      columnEdges_[i] = lowerLeft.x + i * cellWidth;
      columnCenters_[i] = columnEdges_[i] + cellHalfWidth;
    }
    columnEdges_[columns] = lowerLeft.x + columns * cellWidth;
  }

  //---------------------------------------------------------------------------

  Cell GridBase::getCell(unsigned int row,
                         unsigned int column) const
  {
    assert(row < rows_);
    assert(column < columns_);

    XYCoordinates center(columnCenters_[column], rowCenters_[row]);
    return Cell(center, row, column);
  }

  //---------------------------------------------------------------------------

  Cell GridBase::getCell(Coordinate x,
                         Coordinate y) const
  {
    unsigned int row    = (unsigned int)( std::floor((y - lowerLeft_.y) / cellHeight_) );
    unsigned int column = (unsigned int)( std::floor((x - lowerLeft_.x) / cellWidth_)  );
    return getCell(row, column);
  }

  //---------------------------------------------------------------------------

  CoordinateInterval GridBase::getYInterval(unsigned int row) const
  {
    assert(row < rows_);
    return CoordinateInterval(rowEdges_[row], rowEdges_[row + 1]);
  }

  //---------------------------------------------------------------------------

  CoordinateInterval GridBase::getXInterval(unsigned int column) const
  {
    assert(column < columns_);
    return CoordinateInterval(columnEdges_[column], columnEdges_[column + 1]);
  }
}
