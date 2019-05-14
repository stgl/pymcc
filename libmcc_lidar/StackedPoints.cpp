// Copyright 2009-2011 Green Code LLC
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

#include <algorithm>
#include <cassert>

#include <boost/foreach.hpp>

#include "IPoint.h"
#include "IUnclassifiedPoints.h"
#include "StackedPoints.h"

namespace mcc
{
  bool comparePoints(const IPoint * lhs,
                     const IPoint * rhs)
  {
    if (lhs->x() > rhs->x())
      return true;
    if (lhs->x() < rhs->x())
      return false;
    // x coordinates are the same, examine y
    if (lhs->y() > rhs->y())
      return true;
    if (lhs->y() < rhs->y())
      return false;
    // x and y coordinates are the same, examine z
    return lhs->z() > rhs->z();
  }

  //---------------------------------------------------------------------------

  void StackedPoints::classifyPointsAtSameXY(IUnclassifiedPoints &   points,
                                             std::vector<IPoint *> & unclassifiedDuplicates)
  {
    // A vector of pointers to the unclassified points
    std::vector<IPoint *> pts(points.count());
    std::vector<IPoint *>::size_type i = 0;
    BOOST_FOREACH(IPoint & pt, points) {
      pts[i] = &pt;
      ++i;
    }

    // Sort the points by their x,y,z coordinates in decreasing order, so
    // points with the same x,y are adjacent.
    std::sort(pts.begin(), pts.end(), comparePoints);

    std::vector<IPoint *>::iterator ptItor, nextPtItor;
    for (ptItor = pts.begin(), nextPtItor = ptItor + 1;
         nextPtItor < pts.end();
         ptItor = nextPtItor, ++nextPtItor) {
      IPoint & pt = (**ptItor);
      IPoint & nextPt = (**nextPtItor);
      if (pt.x() == nextPt.x() && pt.y() == nextPt.y()) {
        assert( pt.z() >= nextPt.z() );  // since they are in decreasing order

        // Found the start (the top; highest point) of a stack of points
        // (Note: pt = the top point in the stack, and *ptItor points to it).
        // Search for the end (the bottom; lowest point) in the stack.
        std::vector<IPoint *>::iterator stackEnd;
        for (stackEnd = nextPtItor, ++nextPtItor;
             nextPtItor != pts.end();
             stackEnd = nextPtItor, ++nextPtItor) {
          if (pt.x() != (*nextPtItor)->x() || pt.y() != (*nextPtItor)->y())
            break;  // nextPtItor is past end of the current stack
        }
        // NOTE: if the end of the stack is the last point in the whole vector,
        // then nextPtItor == pts.end().  Therefore, at the end of the current
        // pass through the outer "for" loop, nextPtItor will be incremented
        // past pts.end().  That's why the "for" loop's test condition compares
        // the two iterators with "<" instead of "!=".

        // Classify all the but the lowest point(s) in the stack as non-ground.
        Coordinate lowestZ = (*stackEnd)->z();
        while ((*ptItor)->z() > lowestZ) {
          (*ptItor)->classifyAs(NonGround);
          ++ptItor;
        }

        // If more than one point at lowest Z, then add them to duplicate list.
        if (ptItor != stackEnd) {
          for (; ptItor <= stackEnd; ++ptItor)
            unclassifiedDuplicates.push_back(*ptItor);
        }
      }
    }
  }
}
