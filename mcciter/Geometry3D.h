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

#ifndef MCC_GEOMETRY_3D_H
#define MCC_GEOMETRY_3D_H

#include <vector>

class Vec;

namespace mcc
{
  // Helper functions for 3-D geometry.
  class Geometry3D
  {
    public:
      //  Compute the normal vector for a plane represented by 3 points.
      static Vec computeNormalToPlane(Vec pointA,
                                      Vec pointB,
                                      Vec pointC);

      //  Is a list of 3 or more points collinear when projected onto the X,Y
      //  plane?
      static bool areCollinearInXY(const std::vector<Vec> & points);

      //  Are 3 points collinear when projected onto the X,Y plane?
      static bool areCollinearInXY(const Vec & pointP,
                                   const Vec & pointQ,
                                   const Vec & pointR);
  };
}

#endif
