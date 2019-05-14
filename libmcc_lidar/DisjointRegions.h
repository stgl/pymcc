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

#ifndef MCC_DISJOINT_REGIONS_H
#define MCC_DISJOINT_REGIONS_H

#include <boost/shared_ptr.hpp>

#include "Grid.h"
#include "InterpolationRegion.h"
#include "IRegionGenerator.h"

namespace mcc
{
  class IInterpolationRegion;
  class IPointVector;
  class RasterSurface;

  // Divides the points into disjoint (non-overlapping) regions.
  class DisjointRegions: public IRegionGenerator
  {
    public:
      DisjointRegions();

      int subdivide(const IPointVector &  points,
                    PointSelector         pointSelector,
                    const RasterSurface & raster);

      const IInterpolationRegion * getNextRegion();

      void addNeighborPointsToCurrentRegion(int nPoints);

    private:
      boost::shared_ptr< Grid<InterpolationRegion> > regions_;
      const RasterSurface * raster_;
      enum { RegionIteration_Initialized,
             RegionIteration_InProgress,
             RegionIteration_Done } iterationState_;
      unsigned int currentRegionRow_;
      unsigned int currentRegionColumn_;
  };
}

#endif
