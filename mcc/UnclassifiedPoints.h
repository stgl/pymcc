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

#ifndef MCC_UNCLASSIFIED_POINTS_H
#define MCC_UNCLASSIFIED_POINTS_H

#include <vector>
#include <boost/shared_ptr.hpp>
#include "IUnclassifiedPoints.h"

namespace mcc
{
  class Point;
  class PointVector;

  // A vector of unclassified points (points not yet classified)
  class UnclassifiedPoints : public IUnclassifiedPoints
  {
    public:
      using IPointVector::size_type;

      // Create an instance that references an existing point vector.
      UnclassifiedPoints(boost::shared_ptr<PointVector> points);

      // IUnclassifiedPoints interface
      IPointVector::size_type removeClassified();

      // IPointVector interface
      IPointVector::size_type count() const;
      IPointVector::iterator begin();
      IPointVector::iterator end();
      IPointVector::const_iterator begin() const;
      IPointVector::const_iterator end() const;

    private:
      boost::shared_ptr<PointVector> points_;
      std::vector<Point *> unclassifiedPoints_;

      void findUnclassifiedPoints();
  };
}

#endif
