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

#ifndef MCC_VECTOR_ITERATORS_H
#define MCC_VECTOR_ITERATORS_H

#include <vector>
#include <boost/make_shared.hpp>
#include "PointVectorIteratorImpl.h"
#include "IPoint.h"

namespace mcc
{
  // Template function for dereferencing a vector iterator.  This is just the
  // declaration, so specializations must be defined by the client code.
  template<class TPoint, class TVectorIterator>
  TPoint & dereference(const TVectorIterator & itor);

  // A helper struct that defines implementations of PointVectorIterator based
  // a standard vector of some type that leads to an IPoint type (either Point
  // or pointer to Point).
  template<class TVectorElement>  // TVectorElement = Point or Point *
  struct VectorWrapper
  {
    // A helper class that implements PointVectorIterator by wrapping an
    // iterator of a standard vector.
    template<class TPoint, class TVectorIterator>
    class IteratorWrapper : public PointVectorIteratorImpl<TPoint>
    {
      public:
        IteratorWrapper(const TVectorIterator & itor)
          : itor_(itor)
        {
        }

        boost::shared_ptr< PointVectorIteratorImpl<TPoint> > clone() const
        {
          return boost::make_shared< IteratorWrapper<TPoint, TVectorIterator> >(itor_);
        }

        TPoint & operator*() const
        {
          return dereference<TPoint, TVectorIterator>(itor_); 
        }

        PointVectorIteratorImpl<TPoint> & operator++()
        {
          itor_++;
          return *this;
        }

        bool operator==(const PointVectorIteratorImpl<TPoint> & other) const
        {
          return itor_ == dynamic_cast<const IteratorWrapper<TPoint, TVectorIterator> &>(other).itor_;
        }

      private:
        TVectorIterator itor_;
    };

    // Convenience typedefs
    typedef typename IteratorWrapper<IPoint, typename std::vector<TVectorElement>::iterator> iterator;
    typedef typename IteratorWrapper<const IPoint, typename std::vector<TVectorElement>::const_iterator> const_iterator;
  };

}

#endif
