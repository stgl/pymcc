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

#ifndef MCC_CELL_BLOCK_H
#define MCC_CELL_BLOCK_H

#include <vector>

#include "Cell.h"

namespace mcc
{
  // A rectangular block of cells
  struct CellBlock
  {
    Cell upperLeftCell;
    unsigned int height;  // # of rows in block
    unsigned int width;   // # of columns in block

    // Create a cell block with a single cell (height and width both = 1).
    CellBlock(const Cell & upperLeft);

    // Create a cell block with height and width = 0.
    CellBlock();
  };

  //---------------------------------------------------------------------------

  inline
  CellBlock::CellBlock(const Cell & upperLeft)
    : upperLeftCell(upperLeft), height(1), width(1)
  {
  }

  inline
  CellBlock::CellBlock()
    : upperLeftCell(XYCoordinates(0,0), 0, 0), height(0), width(0)
  {
  }
}

#endif
