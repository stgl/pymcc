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

#ifndef MCC_IXYZ_POINT_READER_H
#define MCC_IXYZ_POINT_READER_H

#include <boost/cstdint.hpp>

#include "XyExtent.h"
#include "XyzPoint.h"

namespace mcc
{
  //  Reads a sequence of points with (x, y, z) coordinates from a data file.
  class IXyzPointReader
  {
    public:
      // The number of points in the data file.
      virtual boost::uint32_t count() const = 0;

      // Read the next point from the file.  Returns true if a point was read;
      // false otherwise.
      virtual bool readNextPoint() = 0;

      // The current point that was most recently read from the file.
      virtual const XyzPoint & currentPoint() const = 0;

      // The bounding box around the points read from the file.
      virtual const XyExtent & xyExtent() const = 0;

      // Close the data file.
      virtual ~IXyzPointReader() { }
  };
}

#endif
