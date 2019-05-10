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

#ifndef MCC_GRID_BASE_H
#define MCC_GRID_BASE_H

#include <vector>

#include "Cell.h"
#include "CoordinateInterval.h"
#include "Sequence.h"

namespace mcc
{
  // Base class for a 2-dimensional grid of rectangular cells.
  class GridBase
  {
    public:
      GridBase(unsigned int  rows,
               unsigned int  columns,
               XYCoordinates lowerLeft,
               Coordinate    cellHeight,
               Coordinate    cellWidth);

      // Attributes
      unsigned int rows() const;
      unsigned int columns() const;
      XYCoordinates lowerLeft() const;
      Coordinate cellHeight() const;
      Coordinate cellWidth() const;

      // Attributes for iterating through row and column indexes.
      unsigned int topRow() const;
      unsigned int bottomRow() const;
      Sequence<unsigned int> topToBottom() const;

      unsigned int leftColumn() const;
      unsigned int rightColumn() const;
      Sequence<unsigned int> leftToRight() const;

      // Get a cell at a particular row and column.
      Cell getCell(unsigned int row,
                   unsigned int column) const;

      // Get the cell that contains a certain point.  The point isn't
      // necessarly the cell's center.
      Cell getCell(Coordinate x,
                   Coordinate y) const;

      // Get the Y-coordinate interval for a row.
      CoordinateInterval getYInterval(unsigned int row) const;

      // Get the X-coordinate interval for a column.
      CoordinateInterval getXInterval(unsigned int column) const;

    private:
      unsigned int rows_;
      unsigned int columns_;
      XYCoordinates lowerLeft_;
      Coordinate  cellHeight_;
      Coordinate  cellWidth_;
      std::vector<Coordinate> rowCenters_;     // Y coordinate of each row's center line
      std::vector<Coordinate> rowEdges_;       // Y coordinates of row boundaries:
                                               //   vector[R] = bottom edge of row R
                                               //   vector[R+1] = top edge of row R
      std::vector<Coordinate> columnCenters_;  // X coordinate of each column's center line
      std::vector<Coordinate> columnEdges_;    // X coordinates of column boundaries:
                                               //   vector[C] = left edge of column C
                                               //   vector[C] = right edge of column C
  };

  // Helper functions for moving row indexes up and down, and column indexes
  // left and right.
  int up(unsigned int nCells);
  int down(unsigned int nCells);
  int left(unsigned int nCells);
  int right(unsigned int nCells);

  //---------------------------------------------------------------------------

  inline
  unsigned int GridBase::rows() const
  {
    return rows_;
  }

  inline
  unsigned int GridBase::columns() const
  {
    return columns_;
  }

  inline
  XYCoordinates GridBase::lowerLeft() const
  {
    return lowerLeft_;
  }

  inline
  Coordinate GridBase::cellHeight() const
  {
    return cellHeight_;
  }

  inline
  Coordinate GridBase::cellWidth() const
  {
    return cellWidth_;
  }

  inline
  unsigned int GridBase::topRow() const
  {
    return rows_ - 1;
  }

  inline
  unsigned int GridBase::bottomRow() const
  {
    return 0;
  }

  inline
  Sequence<unsigned int> GridBase::topToBottom() const
  {
    return Sequence<unsigned int>(topRow(), bottomRow());
  }

  inline
  unsigned int GridBase::leftColumn() const
  {
    return 0;
  }

  inline
  unsigned int GridBase::rightColumn() const
  {
    return columns_ - 1;
  }

  inline
  Sequence<unsigned int> GridBase::leftToRight() const
  {
    return Sequence<unsigned int>(leftColumn(), rightColumn());
  }

  //---------------------------------------------------------------------------

  // Rows are numbered from bottom to top.
  inline int   up(unsigned int nCells) { return  nCells; }
  inline int down(unsigned int nCells) { return -int(nCells); }

  // Columns are numbered from left to right.
  inline int  left(unsigned int nCells) { return -int(nCells); }
  inline int right(unsigned int nCells) { return  nCells; }
}

#endif
