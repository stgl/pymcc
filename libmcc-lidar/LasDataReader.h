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

#ifndef MCC_LAS_DATA_READER_H
#define MCC_LAS_DATA_READER_H

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <liblas/point.hpp>
#include <liblas/reader.hpp>

#include "IXyzPointReader.h"
//#include "LasXyzPoint.h"

namespace mcc
{
  // Read a sequence of points with (x, y, z) coordinates from a data file in
  // LAS format.
  class LasDataReader : public IXyzPointReader
  {
    public:
      // Open a LAS file for reading.
      LasDataReader(const boost::filesystem::path & path);

      // The number of points in the data file.
      boost::uint32_t count() const;

      // Read the next point from the file.  Returns true if a point was read;
      // false otherwise.
      bool readNextPoint();

      // The current point that was most recently read from the file.
      const XyzPoint & currentPoint() const;

      // The bounding box around the points read from the file.
      const XyExtent & xyExtent() const;
      
      // Close the file.
      ~LasDataReader();

    private:
      boost::filesystem::ifstream inputFile_;
      boost::shared_ptr<liblas::Reader> lasReader_;
      boost::uint32_t count_;
      XyzPoint xyzPoint_;
      XyExtent xyExtent_;
  };

  //--------------------------------------------------------------------------

  inline
  boost::uint32_t LasDataReader::count() const
  {
    return count_;
  }

  //--------------------------------------------------------------------------

  inline
  const XyzPoint & LasDataReader::currentPoint() const
  {
    return xyzPoint_;
  }

  //--------------------------------------------------------------------------

  inline
  const XyExtent & LasDataReader::xyExtent() const
  {
    return xyExtent_;
  }
}

#endif
