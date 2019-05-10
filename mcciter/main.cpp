// Copyright 2009-2011 Green Code LLC
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

#include <iostream>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

#include "Algorithm.h"
#include "IDataFormat.h"
#include "IXyzPointReader.h"
#include "LasDataFormat.h"
#include "LocaleUtilities.h"
#include "PointCsvFile.h"
#include "PointIO.h"
#include "PointVector.h"
#include "ProgramExceptions.h"
#include "ProgramParameters.h"
#include "SplineExceptions.h"
#include "SurfaceInterpolation.h"
#include "UnclassifiedPoints.h"
#include "XyExtent.h"

#ifdef _MSC_VER
#include "Win32StructuredException.h"
#endif

using namespace mcc;
using boost::shared_ptr;
namespace filesys = boost::filesystem;

//---------------------------------------------------------------------------

shared_ptr<IDataFormat> determineFormat(const filesys::path & inputPath)
{
  if (boost::iequals(inputPath.extension().string(), ".las")) {
    return shared_ptr<IDataFormat>(new LasDataFormat());
  }
  else if (boost::iequals(inputPath.extension().string(), ".lda")) {
    // TO DO: return shared_ptr<IDataFormat>(new FusionDataFormat());
    return shared_ptr<IDataFormat>();
  }
  else {
    return shared_ptr<IDataFormat>();
  }
}

//---------------------------------------------------------------------------

// Read a vector of points from a LIDAR data file in a given format.
shared_ptr<PointVector> readPoints(shared_ptr<IDataFormat>         dataFormat,
                                   const boost::filesystem::path & filePath,
                                   XyExtent &                      xyExtent)
{
  shared_ptr<IXyzPointReader> ptReader = dataFormat->openFile(filePath);
  std::cout << "Reading " << ptReader->count() << " points from '"
            << filePath << "' ..." << std::endl;
  shared_ptr<PointVector> pts(new PointVector( ptReader->count() ));
  PointVector::size_type index = 0;
  while (ptReader->readNextPoint()) {
    (*pts)[index].setCoordinates(ptReader->currentPoint());
    index++;
  }
  xyExtent = ptReader->xyExtent();
  return pts;
}

//---------------------------------------------------------------------------

// Selector for the ground points in a point cloud.
bool selectGroundPoints(const IPoint & point)
{
  // Note: ignore duplicates of ground points, otherwise, they'll cause errors
  //       with the surface interpolation.
  return point.classification() == Ground;
}

//---------------------------------------------------------------------------

int main(int   argc,
         char* argv[])
{
#ifdef _MSC_VER
  Win32StructuredException::setTranslator();
#endif

  // Set standard out to format numbers with thousands separator.
  std::cout.imbue(LocaleUtilities::getLocaleWithNumGrouping());

  try {
    ProgramParameters progParams;
    if (progParams.parseCmdLine(argc, argv) == ActionDone)
      return 0;

    filesys::path inputPath(progParams.inputFile);
    shared_ptr<IDataFormat> dataFormat = determineFormat(inputPath);
    if (! dataFormat) {
      std::cerr << "Error: input file has ";
      if (inputPath.extension().string().size() == 0)
        std::cerr << "no extension";
      else
        std::cerr << "unknown extension: '" << inputPath.extension() << "'";
      std::cerr << std::endl;
      return 1;
    }

    XyExtent xyExtent;
    shared_ptr<PointVector> points = readPoints(dataFormat, inputPath, xyExtent);
    std::cout << "  min X Y : " << toStr(xyExtent.minX) << " " << toStr(xyExtent.minY) << std::endl
              << "  max X Y : " << toStr(xyExtent.maxX) << " " << toStr(xyExtent.maxY) << std::endl;

    SurfaceInterpolation surfaceInterpolation;
    surfaceInterpolation.setXyExtent(xyExtent);

    Algorithm algorithm(surfaceInterpolation, progParams.writeNongroundPts, progParams.writeRasterSurfaces);
    UnclassifiedPoints unclassifiedPoints(points);
    algorithm.labelPointsUsingPass(unclassifiedPoints, progParams.scaleDomain2Spacing);

    if (progParams.outputLidarFile != "") {
      filesys::path outputPath(progParams.outputLidarFile);
      std::cout << "Writing output file \"" << outputPath << "\"..." << std::endl;
      dataFormat->copyFileAndSetClassification(inputPath, outputPath, progParams.scaleDomain2Spacing, progParams.curvatureThreshold, *points);
    }

    if (progParams.outputGroundSurface != "") {
      std::cout << "Interpolating surface from ground points:" << std::endl;
      shared_ptr<IRasterSurface> groundSurface = surfaceInterpolation(*points, selectGroundPoints, progParams.groundSurfaceCellSize, Algorithm::tension);
      filesys::path groundSurfaceFile(progParams.outputGroundSurface);
      std::cout << "Writing ground surface to \"" << groundSurfaceFile << "\"..." << std::endl;
      groundSurface->writeAsciiGrid(groundSurfaceFile.string());
    }

    if (progParams.outputGroundPoints != "") {
      std::cout << "Writing ground points to \"" << progParams.outputGroundPoints << "\"..." << std::endl;
      PointCsvFile groundPtsFile;
      groundPtsFile.open(progParams.outputGroundPoints.string());
      BOOST_FOREACH(const IPoint & point, *points) {
        if (isGround(point.classification()))
          groundPtsFile.writeRow(point);
      }
      groundPtsFile.close();
    }

    return 0;
  }
  catch (ProgramParameterException & exc) {
    std::cerr << "Error: " << exc.what() << std::endl;
    return 1;
  }
  catch (OpenFileException & exc) {
    std::cerr << "Error opening \"" << exc.path.string() << "\": " << exc.what() << std::endl;
    return 1;
  }
  catch (SingularMatrixException & exc) {
    std::cerr << "Error: " << exc.what() << std::endl
              << "  Spline control points:" << std::endl;
    BOOST_FOREACH(const XyzPoint point, exc.points) {
      std::cerr << "    " << point << std::endl;
    }
    return 1;
  }
  catch (std::bad_alloc) {
    std::cerr << "Error: insufficient memory; if applicable, use a larger post spacing." << std::endl;
    return 1;
  }
#ifdef _MSC_VER
  catch (const Win32StructuredException & exc) {
    std::cerr << "Internal program error (exception code = " << std::showbase << std::hex << exc.code() << ")" << std::endl;
    return 1;
  }
#endif
  catch (std::exception & exc) {
    std::cerr << "Internal program error: " << exc.what() << std::endl;
    return 1;
  }
}
