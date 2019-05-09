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

#include <cmath>
#include <iostream>

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>

#include "Algorithm.h"
#include "DuplicatePoints.h"
#include "IPoint.h"
#include "IRasterSurface.h"
#include "ISurfaceInterpolation.h"
#include "IUnclassifiedPoints.h"
#include "LineIndent.h"
#include "PointCanopyHeightFile.h"
#include "StackedPoints.h"

namespace mcc
{
  Algorithm::Algorithm(ISurfaceInterpolation & surfaceInterpolation,
                       bool                    writeNongroundPts,
                       bool                    writeRasterSurfaces)
    : surfaceInterpolation_(surfaceInterpolation),
      writeNongroundPts_(writeNongroundPts),
      writeRasterSurfaces_(writeRasterSurfaces)
  {
  }

  //---------------------------------------------------------------------------

  const double Algorithm::tension = 1.5;

  //---------------------------------------------------------------------------

  void Algorithm::classifyPoints(IUnclassifiedPoints & points,
                                 double                scaleDomain2Spacing,
                                 double                curvatureThreshold)
  {
    // curvature thresholds for scale domains
    double t[4];  // array[0] not used; allow use of indexes in range of 1 to 3
    t[1] = curvatureThreshold;
    t[2] = t[1] + 0.1;
    t[3] = t[2] + 0.1;

     // cell resolutions for scale domains
    double CR[4];
    CR[2] = scaleDomain2Spacing;
    CR[1] = 0.5 * CR[2];
    CR[3] = 1.5 * CR[2];

    // percentage of points classified during a pass that represents convergence
    // for each scale domain
    double convergencePercent[4];
    convergencePercent[1] = 0.01;    // 1%
    convergencePercent[2] = 0.001;   // 0.1%
    convergencePercent[3] = 0.0001;  // 0.01%

    // points not yet classified
    IUnclassifiedPoints & U = points;

    // Locate points that are vertically stacked (at same x,y coordinates), and
    // within each stack, classify all points but the lowest one as non-ground.
    std::cout << "Searching for points with the same x,y coordinates..." << std::endl;
    std::vector<IPoint *> unclassifiedDuplicates;
    StackedPoints::classifyPointsAtSameXY(points, unclassifiedDuplicates);
    int nClassified = points.removeClassified();
    std::cout << "  " << nClassified << " points classified as non-ground" << std::endl;

    DuplicatePoints duplicatePoints(unclassifiedDuplicates);
    std::string pluralEnding = (duplicatePoints.setCount() == 1) ? "" : "s";
    std::cout << "Identified " << duplicatePoints.setCount() << " set" << pluralEnding << " of unclassified duplicate points" << std::endl;
    int nDuplicatesPutAside = duplicatePoints.putAsideAllButOnePointPerSet();
    nDuplicatesPutAside = points.removeClassified();

    LineIndent indent("  ");

    const int maxPasses = 100;  // max # of passes per scale domain
    int nDigitsForPass = (int) std::log10((double) maxPasses) + 1;
    std::string passFormat = boost::str(boost::format("0%d") % nDigitsForPass);

    for (int SD = 1; SD <= 3; SD++) {
      std::cout << "Scale domain: " << SD << " (cell resolution: " << CR[SD] << ")" << std::endl;
      int nPoints;
      int nClassified;
      double percentClassified;
      int pass = 0;
      do {
        pass++;
        if (pass > maxPasses) {
          std::cout << "Maximum # of passes reached for scale domain " << SD << std::endl;
          break;
        }
        std::cout << "SD " << SD << " - Pass " << pass << std::endl
                  << indent << "Interpolating " << U.count() << " points:" << std::endl;
        boost::shared_ptr<IRasterSurface> rasterSurface = surfaceInterpolation_(U, CR[SD], tension);

        std::cout << indent << "Averaging raster surface..." << std::endl;
        rasterSurface->average(3);  // kernel = 3x3

        if (writeRasterSurfaces_) {
          boost::format rasterNameFmt("surface_sd%d_p%|" + passFormat + "|.asc");
          std::string rasterName = boost::str(rasterNameFmt % SD % pass);
          std::cout << indent << "Writing raster surface to file \"" << rasterName  << "\"..." << std::endl;
          rasterSurface->writeAsciiGrid(rasterName);
        }

        std::cout << indent << "Identifying non-ground points:" << std::endl;
        PointCanopyHeightFile nongroundPtsFile;
        if (writeNongroundPts_) {
          boost::format fileName("nonground_sd%d_p%|" + passFormat + "|.csv");
          nongroundPtsFile.open(boost::str(fileName % SD % pass));
        }
        nPoints = U.count();
        BOOST_FOREACH( IPoint & point, U) {
          Coordinate surfaceHeight = (*rasterSurface)(point.x(), point.y());
          if (point.z() > surfaceHeight + t[SD]) {
            point.classifyAs(NonGround);
            if (nongroundPtsFile) {
              Coordinate canopyHeight = point.z() - surfaceHeight;
              nongroundPtsFile.writeRow(point, canopyHeight);
            }
          }
        }

        nClassified = U.removeClassified();
        percentClassified = nClassified / double(nPoints);
        boost::format percentFormat("%|.2|%%");
        std::string percentClassifiedStr = boost::str(percentFormat % (percentClassified * 100));
        std::cout << indent << "  " << nClassified << " points (" << percentClassifiedStr << ") classified as non-ground" << std::endl;
        if (nongroundPtsFile) {
          nongroundPtsFile.close();
          std::cout << indent << "  Wrote points to file \"" << nongroundPtsFile.path() << "\"" << std::endl;
        }
      } while ((percentClassified >= convergencePercent[SD]) && (U.count() > 0)); // until (n_C < convergence_% * n) or no more points

      if (U.count() == 0)
        break;
    }

    // Classify all the remaining points as ground
    std::cout << "Classifying " << U.count() << " points as ground..." << std::endl;
    BOOST_FOREACH( IPoint & point, U) {
      point.classifyAs(Ground);
    }

    pluralEnding = (nDuplicatesPutAside == 1) ? "" : "s";
    std::cout << "Copying classifications to " << nDuplicatesPutAside << " duplicate point" << pluralEnding << "..." << std::endl;
    duplicatePoints.copyClassificationAmongPointsInSet();
  }
}
