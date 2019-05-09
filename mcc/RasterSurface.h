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

#ifndef MCC_RASTER_SURFACE_H
#define MCC_RASTER_SURFACE_H

#include "Cell.h"
#include "Grid.h"
#include "IRasterSurface.h"
#include "XYCoordinates.h"

namespace mcc
{
  class RasterSurface : public IRasterSurface,
                        public Grid<Coordinate>
  {
    public:
      RasterSurface(unsigned int          rows,
                    unsigned int          columns,
                    const XYCoordinates & lowerLeft,
                    Coordinate            cellSize);

      Coordinate cellSize() const;

      // IRasterSurface interface
      void average(int kernelSize);
      Coordinate operator()(Coordinate x,
                            Coordinate y) const;
      void writeAsciiGrid(const std::string & path) const;
  };

  //---------------------------------------------------------------------------

  inline
  Coordinate RasterSurface::cellSize() const
  {
    return cellWidth();  // same as cellHeight()
  }

  inline
  Coordinate RasterSurface:: operator()(Coordinate x,
                                        Coordinate y) const
  {
    return operator[](getCell(x, y));
  }
}

#endif
