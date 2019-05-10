// Copyright 2009-2010 Green Code LLC
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
#ifdef _MSC_VER
#include "VectorWrapper.h"
#endif
#include "Point.h"
#include "PointVector.h"

namespace mcc
{
#ifdef _MSC_VER
  // Specializations of a template function for a standard vector of Point.
  template<>
  inline
  IPoint & dereference(const std::vector<Point>::iterator & itor)
  {
    return *itor;
  }

  template<>
  inline
  const IPoint & dereference(const std::vector<Point>::const_iterator & itor)
  {
    return *itor;
  }

  typedef VectorWrapper<Point> VectorOfPoints;
#endif

  //-------------------------------------------------------------------------

  PointVector::PointVector(IPointVector::size_type size)
    : points_(size)
  {
  }

  //-------------------------------------------------------------------------

  IPointVector::size_type PointVector::count() const
  {
    return points_.size();
  }

  //---------------------------------------------------------------------------

  IPointVector::const_iterator PointVector::begin() const
  {
#ifdef _MSC_VER
    IPointVector::const_iterator itor( boost::make_shared<VectorOfPoints::const_iterator>(points_.begin()) );
#else
    IPointVector::const_iterator itor;
    itor = points_.begin();
#endif
    return itor;
  }

  //---------------------------------------------------------------------------

  IPointVector::const_iterator PointVector::end() const
  {
#ifdef _MSC_VER
    IPointVector::const_iterator itor( boost::make_shared<VectorOfPoints::const_iterator>(points_.end()) );
#else
    IPointVector::const_iterator itor;
    itor = points_.end();
#endif
    return itor;
  }

  //---------------------------------------------------------------------------

  IPointVector::iterator PointVector::begin()
  {
#ifdef _MSC_VER
    IPointVector::iterator itor( boost::make_shared<VectorOfPoints::iterator>(points_.begin()) );
#else
    IPointVector::iterator itor;
    itor = points_.begin();
#endif
    return itor;
  }

  //---------------------------------------------------------------------------

  IPointVector::iterator PointVector::end()
  {
#ifdef _MSC_VER
    IPointVector::iterator itor( boost::make_shared<VectorOfPoints::iterator>(points_.end()) );
#else
    IPointVector::iterator itor;
    itor = points_.end();
#endif
    return itor;
  }
}
