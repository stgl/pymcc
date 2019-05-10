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

#ifndef MCC_STACKED_POINTS_H
#define MCC_STACKED_POINTS_H

#include <vector>

namespace mcc
{
  class IPoint;
  class IUnclassifiedPoints;

  //  Methods for handling points that are vertically stacked, i.e., at the
  //  the same X,Y location.
  class StackedPoints
  {
    public:
      // Locate all the sets of stacked points among the unclassified points.
      // For each set of stacked points, classify all points in the set except
      // the lowest point as non-ground.  Any unclassified duplicate points
      // (i.e., points with the same x,y,z coordinates) are returned in the
      // "duplicatePoints" parameters.  Duplicate points are adjacent in the
      // vector.
      static void classifyPointsAtSameXY(IUnclassifiedPoints &   points,
                                         std::vector<IPoint *> & unclassifiedDuplicates);
  };
}

#endif
