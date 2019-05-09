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

#ifndef MCC_REGULARIZED_SPLINE_H
#define MCC_REGULARIZED_SPLINE_H

#include <vector>
#include <boost/shared_ptr.hpp>

#include "Coordinate.h"
#include "IPoint.h"
#include "tpsdemo/spline.h"

namespace mcc
{
  // A regularized thin plate spline.
  //
  // Utilizes the implementation in TPS Demo program of the methods described
  // in "Approximation Methods for Thin Plate Spline Mappings and Principal
  // Warps", G. Donato and S. Belongie, 2002. 
  class RegularizedSpline
  {
    public:
      RegularizedSpline(const std::vector<const IPoint *> & points,
                        double                              regularization);

      Coordinate interpolateHeight(Coordinate x,
                                   Coordinate y) const;

    private:
      std::vector<Vec> controlPoints_;
      boost::shared_ptr<tpsdemo::Spline> spline_;
  };
}

#endif
