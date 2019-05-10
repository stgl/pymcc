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

#include <fstream>

#include "BinaryFile.h"
#include "LasDataReader.h"

namespace filesys = boost::filesystem;
using boost::shared_ptr;

namespace mcc
{
  LasDataReader::LasDataReader(const boost::filesystem::path & path)
  {
    BinaryFile::openInputFile(path, inputFile_);
    lasReader_ = shared_ptr<liblas::Reader>(new liblas::Reader(inputFile_));
    liblas::Header const & header = lasReader_->GetHeader();
    count_ = header.GetPointRecordsCount();
    xyExtent_.minX = Coordinate(header.GetMinX());
    xyExtent_.maxX = Coordinate(header.GetMaxX());
    xyExtent_.minY = Coordinate(header.GetMinY());
    xyExtent_.maxY = Coordinate(header.GetMaxY());
  }

  //---------------------------------------------------------------------------

  bool LasDataReader::readNextPoint()
  {
    if (lasReader_->ReadNextPoint()) {
      const liblas::Point & lasPoint = lasReader_->GetPoint();
      xyzPoint_.x = Coordinate(lasPoint.GetX());
      xyzPoint_.y = Coordinate(lasPoint.GetY());
      xyzPoint_.z = Coordinate(lasPoint.GetZ());
      return true;
    }
    else
      return false;
  }

  //---------------------------------------------------------------------------

  // Close the file.
  LasDataReader::~LasDataReader()
  {
    inputFile_.close();
  }
}
