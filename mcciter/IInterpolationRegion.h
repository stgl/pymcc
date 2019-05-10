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

#ifndef MCC_IINTERPOLATION_REGION_H
#define MCC_IINTERPOLATION_REGION_H

#include <vector>
#include "Cell.h"
#include "IPoint.h"

namespace mcc
{
  // A region where a single spline is calculated.
  class IInterpolationRegion
  {
    public:
      // The set of points for computing the spline.
      virtual const std::vector<const IPoint *> & points() const = 0;

      // The set of raster cells whose heights should be interpolated from the
      // spline.
      virtual const std::vector<Cell> & cells() const = 0;

      virtual ~IInterpolationRegion() { }
  };
}

#endif
