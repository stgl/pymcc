// Copyright 2009-2010 Green Code LLC
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
//
// CHANGELOG:
//
// 05/09/19: Modified by George E. Hilley to use OpenMP for performance enhancements.
//	        This required eliminating use of BOOST_FOREACH in interpolation loop.

#include <cmath>
#include <iostream>
#include <stdlib.h>

#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <omp.h>


#include "DisjointRegions.h"
#include "IInterpolationRegion.h"
#include "IPointVector.h"
#include "IRegionGenerator.h"
#include "LineIndent.h"
#include "Point.h"
#include "ProgressBar.h"
#include "RasterSurface.h"
#include "RegularizedSpline.h"
#include "SplineExceptions.h"
#include "SurfaceInterpolation.h"
#include "XYCoordinates.h"

namespace mcc
{

  int pointIncrement = 0;
  int lastPointSelected = 0;

  // A point selector that selects every point.
  bool useEveryPoint(const IPoint & point)
  {
    return true;
  }

  //---------------------------------------------------------------------------

  SurfaceInterpolation::SurfaceInterpolation() :
    prevCellResolution_(0)
  {
  }

  //---------------------------------------------------------------------------

  boost::shared_ptr<IRasterSurface> SurfaceInterpolation::operator()(const IPointVector & points,
                                                                     double               cellResolution,
                                                                     double               tension)
  {
      std::cout << "Using all points." << std::endl;
      return this->operator()(points, &useEveryPoint, cellResolution, tension);
  }

  //---------------------------------------------------------------------------

  boost::shared_ptr<IRasterSurface> SurfaceInterpolation::operator()(const IPointVector & points,
                                                                     PointSelector        pointSelector,
                                                                     double               cellResolution,
                                                                     double               tension)
  {
    // If the cell resolution is different from the previous call, then
    // create a new raster based on the new cell size.
    if (cellResolution != prevCellResolution_) {
      // Determine the desired raster dimensions by adding a 1/2 cell-wide
      // margin around the boundaries of the point cloud read by the program.
      double margin = cellResolution / 2;
      double desiredWidth  = (inputExtent_.maxX - inputExtent_.minX) + 2 * margin; // left & right margins
      double desiredHeight = (inputExtent_.maxY - inputExtent_.minY) + 2 * margin; // top & bottom margins

      // Compute the numer of rows & columns needed to minimally cover the
      // desired raster dimensions.
      unsigned int cols = int(std::ceil(desiredWidth  / cellResolution));
      unsigned int rows = int(std::ceil(desiredHeight / cellResolution));

      // Determine the lower-left corner of the raster by centering its actual
      // area around the original input extent.
      double actualWidth = cols * cellResolution;
      double horizontalMargin = (actualWidth - desiredWidth) / 2;
      Coordinate x0 = Coordinate(inputExtent_.minX - horizontalMargin);

      double actualHeight = rows * cellResolution;
      double verticalMargin = (actualHeight - desiredHeight) / 2;
      Coordinate y0 = Coordinate(inputExtent_.minY - verticalMargin);

      XYCoordinates lowerLeft(x0, y0);

      rasterSurface_ = boost::make_shared<RasterSurface>(rows, cols, lowerLeft, Coordinate(cellResolution));
      prevCellResolution_ = cellResolution;
    }


    // Determine where splines will be interpolated for the points and the
    // raster.
    boost::shared_ptr<IRegionGenerator> regions = boost::make_shared<DisjointRegions>();
    int nRegions = regions->subdivide(points, pointSelector, *rasterSurface_);

    rasterSurface_->setNoDataValue(-9999);

    LineIndent indent("  ");

    // For each region, compute a spline for its points and then interpolate
    // heights for its cells.
    std::cout << indent << "Computing splines for regions and cell heights for raster surface:" << std::endl
              << indent << "  ";
    //ProgressBar progressBar(std::cout, nRegions);
    int nSplinesComputed = 0;

    int sj, sstop, tn, tj;

    sj = -1;     // shared loop counter
    sstop = 0;   // shared stopping condition
    const IInterpolationRegion *region; // Region, declared as private below

    #pragma omp parallel private(tn,tj,region)
    {
      tn = omp_get_thread_num();
      while (!sstop)
      {
        /* Threads update the shared counter by turns */
        #pragma omp critical
        {
          sj++;      // increment the shared loop counter...
          tj = sj;   // ...and keep a private copy of it
        }

        const Cell *cell = regions->getNextCell();
        if(!cell) {
          sstop = 1;
          #pragma omp flush(sstop)
        } else {
          region = regions->getRegionForCell(cell);
          bool splineComputed = false;
          while (! splineComputed) {
            try {
              std::vector<const IPoint *> points;
              std::vector<Cell> cells;
              regions->getPointsAndCellsForCell(cell, 0, points, cells);
              if(points.size() >= 3) {
                RegularizedSpline spline(points, 0.0, nSplinesComputed);
                splineComputed = true;

            	  for(std::vector<Cell>::size_type i = 0; i < cells.size(); i++) {
            	    (*rasterSurface_)[cells[i]] = spline.interpolateHeight(cells[i].x(), cells[i].y());
            	  }
              } else {
                std::cout << "did not compute. Fewer than 3 points." << std::endl;
              }
            }
            catch (SingularMatrixException) {
              std::cout << indent << "Caught singular matrix for spline" << std::endl;
              // Add another neighboring point and try the spline calculation again.

              // Add neighboring points needs to be written in terms of grid location.
              std::vector<const IPoint *> points;
              std::vector<Cell> cells;
              regions->getPointsAndCellsForCell(cell, 1, points, cells);

              // A safety check to prevent an endless loop from consuming all the
              // point cloud.
              if (points.size() >= 300)
                throw;  // Bail
            }
          }
          nSplinesComputed++;
          //std::cout << "updating progress bar" << std::endl;
          //progressBar.update(nSplinesComputed);
          //std::cout << "updated progress bar" << std::endl;
        }
        /* When sstop=1, most threads continue to this statment */
      }
    }

    std::cout << std::endl;

    std::cout << "returning from interpolation." << std::endl;

    return rasterSurface_;
  }
}
