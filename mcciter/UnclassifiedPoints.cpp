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
#else
#include <boost/iterator/indirect_iterator.hpp>
#endif
#include "Point.h"
#include "PointClassification.h"
#include "UnclassifiedPoints.h"
#include "PointVector.h"

namespace mcc
{
#ifdef _MSC_VER
  // Specializations of a template function for a standard vector of Point *.
  template<>
  inline
  IPoint & dereference(const std::vector<Point *>::iterator & itor)
  {
    return **itor;
  }

  template<>
  inline
  const IPoint & dereference(const std::vector<Point *>::const_iterator & itor)
  {
    return **itor;
  }

  typedef VectorWrapper<Point *> VectorOfPointers;
#endif

  //-------------------------------------------------------------------------

  UnclassifiedPoints::UnclassifiedPoints(boost::shared_ptr<PointVector> points)
    : points_(points), unclassifiedPoints_(points->count())
  {
    PointVector & pts = *points_;
    for (IPointVector::size_type i = 0; i < pts.count(); ++i) {
      unclassifiedPoints_[i] = &(pts[i]);
    }
  }

  //-------------------------------------------------------------------------

  IPointVector::size_type UnclassifiedPoints::count() const
  {
    return unclassifiedPoints_.size();
  }

  //---------------------------------------------------------------------------

  IPointVector::const_iterator UnclassifiedPoints::begin() const
  {
#ifdef _MSC_VER
    IPointVector::const_iterator itor( boost::make_shared<VectorOfPointers::const_iterator>(unclassifiedPoints_.begin()) );
#else
    IPointVector::const_iterator itor;
    itor = boost::make_indirect_iterator(unclassifiedPoints_.begin());
#endif
    return itor;
  }

  //---------------------------------------------------------------------------

  IPointVector::const_iterator UnclassifiedPoints::end() const
  {
#ifdef _MSC_VER
    IPointVector::const_iterator itor( boost::make_shared<VectorOfPointers::const_iterator>(unclassifiedPoints_.end()) );
#else
    IPointVector::const_iterator itor;
    itor = boost::make_indirect_iterator(unclassifiedPoints_.end());
#endif
    return itor;
  }

  //---------------------------------------------------------------------------

  // Predicate to determine if a point has been classified
  bool isClassified(const Point * point)
  {
    return point->classification() != NotYetClassified;
  }

  //---------------------------------------------------------------------------

  IPointVector::size_type UnclassifiedPoints::removeClassified()
  {
    IPointVector::size_type originalCount = unclassifiedPoints_.size();
    unclassifiedPoints_.erase(std::remove_if(unclassifiedPoints_.begin(),
                                             unclassifiedPoints_.end(),
                                             isClassified),
                              unclassifiedPoints_.end());
    return originalCount - unclassifiedPoints_.size();
  }

  //---------------------------------------------------------------------------

  IPointVector::iterator UnclassifiedPoints::begin()
  {
#ifdef _MSC_VER
    IPointVector::iterator itor( boost::make_shared<VectorOfPointers::iterator>(unclassifiedPoints_.begin()) );
#else
    IPointVector::iterator itor;
    itor = boost::make_indirect_iterator(unclassifiedPoints_.begin());
#endif
    return itor;
  }

  //---------------------------------------------------------------------------

  IPointVector::iterator UnclassifiedPoints::end()
  {
#ifdef _MSC_VER
    IPointVector::iterator itor( boost::make_shared<VectorOfPointers::iterator>(unclassifiedPoints_.end()) );
#else
    IPointVector::iterator itor;
    itor = boost::make_indirect_iterator(unclassifiedPoints_.end());
#endif
    return itor;
  }
}
