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

#ifndef MCC_XYZ_POINT_H
#define MCC_XYZ_POINT_H

#include "Coordinate.h"

namespace mcc
{
  // A point with just x, y, and z coordinates.
  struct XyzPoint
  {
    Coordinate x;
    Coordinate y;
    Coordinate z;

    // Create a point with coordinates (0, 0, 0).
    XyzPoint();

    // Create a point with specific coordinates.
    XyzPoint(Coordinate xCoordinate,
             Coordinate yCoordinate,
             Coordinate zCoordinate);
  };
  
  //---------------------------------------------------------------------------

  inline
  XyzPoint::XyzPoint()
    : x(0), y(0), z(0)
  {
  }

  inline
  XyzPoint::XyzPoint(Coordinate xCoordinate,
                     Coordinate yCoordinate,
                     Coordinate zCoordinate)
    : x(xCoordinate), y(yCoordinate), z(zCoordinate)
  {
  }
}

#endif
