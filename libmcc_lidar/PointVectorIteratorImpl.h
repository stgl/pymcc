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

#ifndef MCC_POINT_VECTOR_ITERATOR_IMPL_H
#define MCC_POINT_VECTOR_ITERATOR_IMPL_H

#include <boost/shared_ptr.hpp>

namespace mcc
{
  //  Abstract base class for implementing iterators of IPointVector implementations.

  template<class TPoint>  /* TPoint = IPoint or const IPoint */
  class PointVectorIteratorImpl
  { 
    public:
      // Clone an implementation.
      virtual boost::shared_ptr< PointVectorIteratorImpl<TPoint> > clone() const = 0;

      // Deference the iterator.
      virtual TPoint & operator*() const = 0;

      // Advance the iterator.
      virtual PointVectorIteratorImpl<TPoint> & operator++() = 0;

      // Is the iterator equal to another iterator?
      virtual bool operator==(const PointVectorIteratorImpl<TPoint> & other) const = 0;

      virtual ~PointVectorIteratorImpl() { }
  };

}

#endif
