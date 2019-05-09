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

#ifndef MCC_INTERPOLATION_REGION_H
#define MCC_INTERPOLATION_REGION_H

#include <vector>

#include "CellBlock.h"
#include "IInterpolationRegion.h"
#include "IPoint.h"

namespace mcc
{
  struct InterpolationRegion : public IInterpolationRegion
  {
    std::vector<const IPoint *> pts;  // points that lie in the region and were selected
    int nPtsNotSelected;              // # of points in the region that weren't selected
    CellBlock cellBlock;

    InterpolationRegion();

    // IInterpolationReigon interface
    const std::vector<const IPoint *> & points() const;
    const std::vector<Cell> & cells() const;

    // Shared by all instances to save memory
    static std::vector<const IPoint *> pointList;  // for computing spline; may
                                                   // include points from
                                                   // neighboring regions
    static std::vector<Cell> cellList;
  };

  //---------------------------------------------------------------------------

  inline
  InterpolationRegion::InterpolationRegion()
    : nPtsNotSelected(0)
  {
  }

  //---------------------------------------------------------------------------

  inline
  const std::vector<const IPoint *> & InterpolationRegion::points() const
  {
    return pointList;
  }

  //---------------------------------------------------------------------------

  inline
  const std::vector<Cell> & InterpolationRegion::cells() const
  {
    return cellList;
  }
}

#endif
