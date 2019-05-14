// Copyright 2009, 2011 Green Code LLC
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

#include <exception>

#include <liblas/writer.hpp>
#include <boost/format.hpp>

#include "BinaryFile.h"
#include "config.h"
#include "IClassificationVector.h"
#include "LasDataFormat.h"
#include "LasDataReader.h"

namespace filesys = boost::filesystem;
using boost::shared_ptr;

namespace mcc
{
  shared_ptr<IXyzPointReader> LasDataFormat::openFile(const filesys::path & path)
  {
    return shared_ptr<IXyzPointReader>(new LasDataReader(path));
  }

  //---------------------------------------------------------------------------

  void LasDataFormat::copyFileAndSetClassification(const filesys::path &         srcPath,
                                                   const filesys::path &         destPath,
												   double scaleDomain2Spacing,
												   double curvatureThreshold,
                                                   const IClassificationVector & ptClassifications)
  {
    filesys::ifstream srcFile;
    BinaryFile::openInputFile(srcPath, srcFile);
    liblas::Reader lasReader(srcFile);

    liblas::Header header(lasReader.GetHeader());
    header.SetSoftwareId("MCC-LIDAR " VERSION_RELEASE_ABBR " (rev " SUBVERSION_REVISION ")");
    header.SetSystemId("GROUND CLASSIFICATION");
	
	liblas::Header h;
	header.SetCreationDOY(h.GetCreationDOY());
    header.SetCreationYear(h.GetCreationYear());

	// write mcc-lidar parameters into variable length records
	liblas::VariableRecord vlr;
	vlr.SetUserId("MCC_LiDAR");
	vlr.SetRecordId(static_cast<boost::uint16_t>(101));
	std::string scale = boost::str(boost::format("%d") % scaleDomain2Spacing);
	std::string threshold = boost::str(boost::format("%d") % curvatureThreshold);
	vlr.SetDescription("Classified at s="  + scale + ", t=" + threshold);
	std::vector<boost::uint8_t> vlrData;
	vlrData.push_back(static_cast<boost::uint8_t>(scaleDomain2Spacing));
	vlrData.push_back(static_cast<boost::uint8_t>(curvatureThreshold));
	vlr.SetData(vlrData);
	vlr.SetRecordLength(static_cast<boost::uint16_t>(vlrData.size()));
	header.AddVLR(vlr);

    unsigned count = header.GetPointRecordsCount();

    filesys::ofstream destFile;
    BinaryFile::openOutputFile(destPath, destFile);
    liblas::Writer lasWriter(destFile, header);

    // write src path's header to dest path
    IClassificationVector::index_type i;
    for (i.value = 0; i.value < count; i.value++) {
      if (! lasReader.ReadNextPoint())
        throw std::runtime_error("cannot read point from input file");
      liblas::Point lasPoint = lasReader.GetPoint();
      if (isGround(ptClassifications[i]))
        lasPoint.SetClassification(liblas::Point::eGround);
      else {
        // NOTE: there are multiple vegetation classifications:
        //        eLowVegetation
        //        eMediumVegetation
        //        eHighVegetation
        // Per LAS Specification 2.0, footnote 3 (pgs 14-15), assign class 1
        // (Unclassified) to points that the MCC algorithm didn't classify as
        // ground.
        lasPoint.SetClassification(liblas::Point::eUnclassified);
      }
      if (! lasWriter.WritePoint(lasPoint))
        throw std::runtime_error("cannot write point from output file");
    }
    if (srcFile)
      srcFile.close();
    if (destFile)
      destFile.close();
  }
}
