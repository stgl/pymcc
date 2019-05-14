// Copyright 2010-2011 Green Code LLC
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

#include <boost/foreach.hpp>

#include "DuplicatePoints.h"
#include "IPoint.h"

namespace mcc
{
  DuplicatePoints::DuplicatePoints(const std::vector<IPoint *> & unclassifiedDuplicates)
  {
    std::vector<IPoint *> * currentSet = 0;
    IPoint * previousPoint = 0;

    BOOST_FOREACH( IPoint * point, unclassifiedDuplicates ) {
      if (previousPoint && (previousPoint->x() == point->x())
                        && (previousPoint->y() == point->y())
                        && (previousPoint->z() == point->z())) {
        currentSet->push_back(point);
      }
      else {
        sets_.push_back( std::vector<IPoint *>() );
        currentSet = &( sets_.back() );
        currentSet->push_back(point);
      }
      previousPoint = point;
    }
  }

  //---------------------------------------------------------------------------

  int DuplicatePoints::putAsideAllButOnePointPerSet()
  {
    int nPutAside = 0;
    BOOST_FOREACH( std::vector<IPoint *> & set , sets_ ) {
      IPoint * firstPoint = 0;
      BOOST_FOREACH( IPoint * point, set ) {
        if (firstPoint)
          point->classifyAs(Duplicate);
        else
          firstPoint = point;
      }
      nPutAside += set.size() - 1;
    }
    return nPutAside;
  }

  //---------------------------------------------------------------------------

  void DuplicatePoints::copyClassificationAmongPointsInSet()
  {
    BOOST_FOREACH( std::vector<IPoint *> & set , sets_ ) {
      IPoint * firstPoint = 0;
      PointClassification classificationToCopy = NotYetClassified;
      BOOST_FOREACH( IPoint * point, set ) {
        if (firstPoint)
          point->classifyAs(classificationToCopy);
        else {
          firstPoint = point;
          if (firstPoint->classification() == Ground)
            classificationToCopy = GroundDuplicate;
          else
            classificationToCopy = NonGround;
        }
      }
    }
  }
}
