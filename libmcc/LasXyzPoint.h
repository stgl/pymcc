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

#ifndef MCC_LAS_XYZ_POINT_H
#define MCC_LAS_XYZ_POINT_H

#include <liblas/laspoint.hpp>
#include "IXyzPoint.h"

namespace mcc
{
  // An interface to a libLAS point as a simple (x, y, z) point.
  class LasXyzPoint : public IXyzPoint
  {
    public:
      LasXyzPoint(liblas::LASPoint & lasPoint);
      Coordinate x() const;
      Coordinate y() const;
      Coordinate z() const;

    private:
      liblas::LASPoint & lasPoint_;
  };

  //--------------------------------------------------------------------------

  inline
  LasXyzPoint::LasXyzPoint(liblas::LASPoint & lasPoint)
    : lasPoint_(lasPoint)
  {
  }

  //--------------------------------------------------------------------------

  inline Coordinate LasXyzPoint::x() const { return lasPoint_.GetX(); }
  inline Coordinate LasXyzPoint::y() const { return lasPoint_.GetY(); }
  inline Coordinate LasXyzPoint::z() const { return lasPoint_.GetZ(); }
}

#endif
