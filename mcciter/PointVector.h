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

#ifndef MCC_POINT_VECTOR_H
#define MCC_POINT_VECTOR_H

#include <iostream>
#include <vector>

#include <boost/range.hpp>

#include "IClassificationVector.h"
#include "IPointVector.h"
#include "Point.h"

namespace mcc
{
  class PointVector : public IPointVector,
                      public IClassificationVector
  {
    public:
      using IPointVector::size_type;

      // Create a new point vector.
      PointVector(IPointVector::size_type size);

      // Access a point at a given index in the vector.
      Point & operator[](IPointVector::size_type index);
      const Point & operator[](IPointVector::size_type index) const;

      // IPointVector interface
      IPointVector::size_type count() const;
      IPointVector::iterator begin();
      IPointVector::iterator end();
      IPointVector::const_iterator begin() const;
      IPointVector::const_iterator end() const;

      // IClassificationVector interface
      PointClassification operator[](IClassificationVector::index_type index) const;

    private:
      std::vector<Point> points_;
  };

  //---------------------------------------------------------------------------

  inline
  Point & PointVector::operator[](IPointVector::size_type index)
  {
    return points_[index];
  }

  //---------------------------------------------------------------------------

  inline
  const Point & PointVector::operator[](IPointVector::size_type index) const
  {
    return points_[index];
  }

  //---------------------------------------------------------------------------

  inline
  PointClassification PointVector::operator[](IClassificationVector::index_type index) const
  {
    return points_[index.value].classification();
  }
}

#endif
