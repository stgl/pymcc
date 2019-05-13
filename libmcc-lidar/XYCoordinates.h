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

#ifndef MCC_XY_COORDINATES_H
#define MCC_XY_COORDINATES_H

#include "Coordinate.h"

namespace mcc
{
  // A pair of (x,y) coordinates.
  struct XYCoordinates
  {
    Coordinate x;
    Coordinate y;

    // Create a pair of (0, 0) coordinates.
    XYCoordinates();

    // Create a pair of specific coordinates.
    XYCoordinates(Coordinate xCoordinate,
                  Coordinate yCoordinate);
  };
  
  //---------------------------------------------------------------------------

  inline
  XYCoordinates::XYCoordinates()
    : x(0), y(0)
  {
  }

  inline
  XYCoordinates::XYCoordinates(Coordinate xCoordinate,
                               Coordinate yCoordinate)
    : x(xCoordinate), y(yCoordinate)
  {
  }
}

#endif
