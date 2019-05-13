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

#ifndef MCC_ICLASSIFICATION_VECTOR_H
#define MCC_ICLASSIFICATION_VECTOR_H

#include <boost/cstdint.hpp>
#include "PointClassification.h"

namespace mcc
{
  // A vector of point classifications.
  class IClassificationVector
  {
    public:
      struct index_type
      {
        boost::uint_least32_t value;
      };

      // Get the classification for a particular point.  The point is denoted
      // by its index in the data file.
      virtual PointClassification operator[](index_type index) const = 0;

      virtual ~IClassificationVector() { }
  };
}

#endif
