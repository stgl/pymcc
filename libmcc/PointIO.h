// Copyright 2010 Green Code LLC
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

#ifndef MCC_POINT_IO_H
#define MCC_POINT_IO_H

#include <iostream>
#include <boost/format.hpp>
#include "XyzPoint.h"

namespace mcc
{
  // Helper formats and functions for point I/O

  // Format specification for a single coordinate.
  std::string coordinateFmtSpec();

  // Formatter for a single point coordinate.
  extern boost::format coordinateFmt;

  // Convert coordinate to a string.
  std::string toStr(Coordinate coordinate);

  // Write a point as "(x, y, z)" to an output stream.
  std::ostream & operator<<(std::ostream &   stream,
                            const XyzPoint & point);
}

#endif
