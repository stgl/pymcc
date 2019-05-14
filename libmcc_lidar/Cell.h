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

#ifndef MCC_CELL_H
#define MCC_CELL_H

#include "XYCoordinates.h"

namespace mcc
{
  // A raster cell.
  class Cell
  {
    public:
      Cell(const XYCoordinates & center,
           unsigned int          row,
           unsigned int          column);
      Coordinate x() const;
      Coordinate y() const;
      unsigned int row() const;
      unsigned int column() const;

    private:
      XYCoordinates center_;
      unsigned int row_;
      unsigned int column_;
  };

  //---------------------------------------------------------------------------

  inline
  Cell::Cell(const XYCoordinates & center,
             unsigned int          row,
             unsigned int          column)
    : center_(center), row_(row), column_(column)
  {
  }

  inline
  Coordinate Cell::x() const
  {
    return center_.x;
  }

  inline
  Coordinate Cell::y() const
  {
    return center_.y;
  }

  inline
  unsigned int Cell::row() const
  {
    return row_;
  }

  inline
  unsigned int Cell::column() const
  {
    return column_;
  }
}

#endif
