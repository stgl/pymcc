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

#ifndef MCC_IREGION_GENERATOR_H
#define MCC_IREGION_GENERATOR_H

#include "PointSelector.h"

namespace mcc
{
  class IInterpolationRegion;
  class IPointVector;
  class RasterSurface;

  // Embodies a method for subdividing a set of data points and its associated
  // target raster into regions for spline interpolation.
  class IRegionGenerator
  {
    public:
      // Determine the interpolation regions for a set of points and its
      // associated target raster.  Only the points selected by the
      // pointSelector parameter are put in the interpolation regions.
      // Returns the number of regions.
      virtual int subdivide(const IPointVector &  points,
                            PointSelector         pointSelector,
                            const RasterSurface & raster) = 0;

      // Return the next region.  If no more regions, then returns 0.
      virtual const IInterpolationRegion * getNextRegion() = 0;

      // Add points from neighboring regions to the current region (i.e., the
      // region returned by getNextRegion).
      virtual void addNeighborPointsToCurrentRegion(int nPoints) = 0;

      virtual ~IRegionGenerator() { }
  };
}

#endif
