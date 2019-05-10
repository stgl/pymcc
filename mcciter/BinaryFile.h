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

#ifndef MCC_BINARY_FILE_H
#define MCC_BINARY_FILE_H

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace mcc
{
  class BinaryFile
  {
    public:
      // Open a binary file for reading.
      // Throws OpenFileException if the path does not exist, if it refers to
      // a directory, or if it cannot be opened.
      static void openInputFile(const boost::filesystem::path & path,
                                boost::filesystem::ifstream &   inputFile);

      // Open a binary file for writing.
      // Throws OpenFileException if the path refers to a directory, or if it
      // cannot be opened.
      static void openOutputFile(const boost::filesystem::path & path,
                                 boost::filesystem::ofstream &   outputFile);
  };
}

#endif
