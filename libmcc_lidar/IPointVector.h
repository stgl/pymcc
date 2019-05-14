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

#ifndef MCC_IPOINT_VECTOR_H
#define MCC_IPOINT_VECTOR_H

#include <boost/cstdint.hpp>
#ifdef _MSC_VER
// See note below.
#include "PointVectorIterator.h"
#include "PointVectorIteratorImpl.h"
#else
#include <boost/iterator.hpp>
#include "any_iterator/any_iterator.hpp"
#endif

namespace mcc
{
  class IPoint;

  class IPointVector
  {
    public:
      typedef boost::uint_least32_t size_type;

#ifdef _MSC_VER
      // The Microsoft compiler crashes when compiling PointVector.cpp
      // (perhaps the combination of IteratorTypeErase::any_iterator and
      // boost::indirect_iterator is too complex template code for it to
      // handle).  Anyhow, as a workaround, we use a template for concrete
      // classes that implement iterators using the Pimpl idiom.
      typedef PointVectorIterator<IPoint>
      iterator;

      typedef PointVectorIterator<const IPoint>
      const_iterator;
#else
      typedef IteratorTypeErasure::any_iterator<
        IPoint,
        boost::forward_traversal_tag
      >
      iterator;

      typedef IteratorTypeErasure::any_iterator<
        const IPoint,
        boost::forward_traversal_tag
      >
      const_iterator;
#endif

      // Number of points in vector
      virtual size_type count() const = 0;

      virtual iterator begin() = 0;
      virtual iterator end() = 0;

      virtual const_iterator begin() const = 0;
      virtual const_iterator end() const = 0;

      virtual ~IPointVector() { }
  };
}

#endif
