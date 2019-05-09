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

#ifndef MCC_LAS_DATA_FORMAT_H
#define MCC_LAS_DATA_FORMAT_H

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

#include "IDataFormat.h"
#include "IXyzPointReader.h"
#include "IClassificationVector.h"

namespace mcc
{
  // Represents data files in LAS format.
  class LasDataFormat : public IDataFormat
  {
    public:
      // Open a data file for reading.
      boost::shared_ptr<IXyzPointReader> openFile(const boost::filesystem::path & path);

      // Copy point data from one file to another, and sets the classification
      // for the points in the new file.
      void copyFileAndSetClassification(const boost::filesystem::path & srcPath,
                                        const boost::filesystem::path & destPath,
										double scaleDomain2Spacing,
										double curvatureThreshold,
                                        const IClassificationVector &   ptClassifications);
  };
}

#endif
