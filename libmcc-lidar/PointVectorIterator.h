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

#ifndef MCC_POINT_VECTOR_ITERATOR_H
#define MCC_POINT_VECTOR_ITERATOR_H

#include <iterator>
#include <boost/shared_ptr.hpp>
#include "PointVectorIteratorImpl.h"

namespace mcc
{
  // A concrete class for iterators of IPointVector implementations.  The
  // iterators must be concrete classes in order to work with BOOST_FOREACH.

  template<class TPoint>  /* TPoint = IPoint or const IPoint */
  class PointVectorIterator
    : public std::iterator<std::forward_iterator_tag, TPoint>
  { 
    public:
      // Constructors
      PointVectorIterator();
      PointVectorIterator(boost::shared_ptr< PointVectorIteratorImpl<TPoint> > impl);
      PointVectorIterator(const PointVectorIterator<TPoint> & other);

      // Dereference the iterator
      TPoint & operator*() const;

      // Advance the iterator
      PointVectorIterator<TPoint> & operator++();

      // Comparison operators
      bool operator==(const PointVectorIterator<TPoint> & other) const;
      bool operator!=(const PointVectorIterator<TPoint> & other) const;

    private:
      boost::shared_ptr< PointVectorIteratorImpl<TPoint> > impl_;
  };

  //---------------------------------------------------------------------------

  template<class TPoint>
  PointVectorIterator<TPoint>::PointVectorIterator()
    : impl_()
  {
  }

  template<class TPoint>
  PointVectorIterator<TPoint>::PointVectorIterator(boost::shared_ptr< PointVectorIteratorImpl<TPoint> > impl)
    : impl_(impl)
  {
  }

  template<class TPoint>
  PointVectorIterator<TPoint>::PointVectorIterator(const PointVectorIterator<TPoint> & other)
    : impl_(other.impl_->clone())  // value semantics
  {
  }

  template<class TPoint>
  TPoint & PointVectorIterator<TPoint>::operator*() const
  {
    return impl_->operator*();
  }

  template<class TPoint>
  PointVectorIterator<TPoint> & PointVectorIterator<TPoint>::operator++()
  {
    impl_->operator++();
    return *this;
  }

  template<class TPoint>
  bool PointVectorIterator<TPoint>::operator==(const PointVectorIterator<TPoint> & other) const
  {
    return impl_->operator==(*(other.impl_));
  }

  template<class TPoint>
  bool PointVectorIterator<TPoint>::operator!=(const PointVectorIterator<TPoint> & other) const
  {
    return !operator==(other);
  }

}

#endif
