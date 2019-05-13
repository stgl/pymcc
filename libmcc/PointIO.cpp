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

#include <limits>
#include <boost/format.hpp>

#include "PointIO.h"
#include "XyzPoint.h"

namespace mcc
{
  // Format specification for a point coordinate.
  std::string coordinateFmtSpec()
  {
    boost::format spec("%%.%|d|g");
    spec % std::numeric_limits<Coordinate>::digits10;
    return spec.str();
  }

  boost::format coordinateFmt(coordinateFmtSpec());

  //---------------------------------------------------------------------------

  std::string toStr(Coordinate coordinate)
  {
    return boost::str(coordinateFmt % coordinate);
  }

  //---------------------------------------------------------------------------

  std::string pointFmtSpec()
  {
    boost::format spec("(%1%, %1%, %1%)");
    spec % coordinateFmtSpec();
    return spec.str();
  }

  boost::format pointFmt(pointFmtSpec());

  //---------------------------------------------------------------------------

  std::ostream & operator<<(std::ostream &   stream,
                            const XyzPoint & point)
  {
    stream << pointFmt % point.x % point.y % point.z;
    return stream;
  }
}
