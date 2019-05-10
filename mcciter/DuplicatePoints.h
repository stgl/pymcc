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

#ifndef MCC_DUPLICATE_POINTS_H
#define MCC_DUPLICATE_POINTS_H

#include <vector>

namespace mcc
{
  class IPoint;
  class IUnclassifiedPoints;

  // Contains all the sets of unclassified duplicate points in the input point
  // cloud.
  class DuplicatePoints
  {
    public:
      // Initialize a new instance with a list of all the sets of unclassified
      // duplicate points.  The points for each set are adjacent in the list.
      DuplicatePoints(const std::vector<IPoint *> & unclassifiedDuplicates);

      // The number of sets of duplicate points.
      int setCount() const;

      // For each set of duplicate points, leave just one point in the set as
      // unclassified.  All the other points in the set are put aside so they
      // don't processed by the MCC algorithm.  Returns the total number of
      // points that are aside from all the sets.
      int putAsideAllButOnePointPerSet();

      // For each set of duplicate points, copy the classification code of the
      // one point that was processed by the MCC algorithm to all the other
      // points in the set (that were set aside).
      void copyClassificationAmongPointsInSet();

    private:
      std::vector< std::vector<IPoint *> > sets_;
  };

  //---------------------------------------------------------------------------

  inline
  int DuplicatePoints::setCount() const
  {
    return sets_.size();
  }
}

#endif
