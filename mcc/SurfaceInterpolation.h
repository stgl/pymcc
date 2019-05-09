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

#ifndef MCC_SURFACE_INTERPOLATION_H
#define MCC_SURFACE_INTERPOLATION_H

#include <boost/shared_ptr.hpp>

#include "ISurfaceInterpolation.h"
#include "PointSelector.h"
#include "RasterSurface.h"
#include "XyExtent.h"

namespace mcc
{
  class IPointVector;
  class IRasterSurface;

  class SurfaceInterpolation : public ISurfaceInterpolation
  {
    public:
      SurfaceInterpolation();

      // Set the bounding box around all the point data read into the program.
      void setXyExtent(const XyExtent & xyExtent);

      // ISurfaceInterpolation interface
      boost::shared_ptr<IRasterSurface> operator()(const IPointVector & points,
                                                   double               cellResolution,
                                                   double               tension);

      // Interpolate a raster surface from a selected group of points within a
      // point vector.
      boost::shared_ptr<IRasterSurface> operator()(const IPointVector & points,
                                                   PointSelector        pointSelector,
                                                   double               cellResolution,
                                                   double               tension);

    private:
      XyExtent inputExtent_;
      double prevCellResolution_;
      boost::shared_ptr<RasterSurface> rasterSurface_;
  };

  //---------------------------------------------------------------------------

  inline
  void SurfaceInterpolation::setXyExtent(const XyExtent & xyExtent)
  {
    inputExtent_ = xyExtent;
  }
}

#endif
