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

#include <cassert>

#include "tpsdemo/linalg3d-double.h"

#include "Geometry3D.h"

namespace mcc
{
  Vec Geometry3D::computeNormalToPlane(Vec pointA,
                                       Vec pointB,
                                       Vec pointC)
  {
    Vec vectorAB = pointB - pointA;
    Vec vectorAC = pointC - pointA;

    Vec normalToPlane = cross(vectorAB, vectorAC);
    return normalToPlane;
  }

  //---------------------------------------------------------------------------

  bool Geometry3D::areCollinearInXY(const std::vector<Vec> & points)
  {
    assert(points.size() >= 3);

    // Test first 3 points for collinearity.  If they are, the drop one point,
    // add the next point, and then retest.  Continue until all points
    // considered.
    for (std::vector<Vec>::size_type i = 0; i <= points.size() - 3; i++) {
      if (! Geometry3D::areCollinearInXY(points[i], points[i+1], points[i+2]))
        return false;
    }
    return true;
  }

  //---------------------------------------------------------------------------

  bool Geometry3D::areCollinearInXY(const Vec & pointP,
                                    const Vec & pointQ,
                                    const Vec & pointR)
  {
    //  From page 549 in chapter 33 (Writing Programs for The "Book", by Brian
    //  Hayes) in "Beautiful Code", eds: G. Wilson, A. Oram (2007: O'Reilly).
    if ((pointP.x - pointR.x) * (pointQ.y - pointR.y) ==
        (pointQ.x - pointR.x) * (pointP.y - pointR.y))
      return true;
    else
      return false;
  }
}
