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

#include <boost/filesystem/fstream.hpp>

#include "BinaryFile.h"
#include "ProgramExceptions.h"

namespace filesys = boost::filesystem;

namespace mcc
{
  void BinaryFile::openInputFile(const filesys::path & path,
                                 filesys::ifstream &   inputFile)
  {
    if (! filesys::exists(path))
      throw OpenFileException(path, "file does not exist");
    if (! filesys::is_regular_file(path))
      throw OpenFileException(path, "path is not a file");
    inputFile.open(path, std::ios::in | std::ios::binary);
    if (! inputFile)
      throw OpenFileException(path, "check permissions");
  }

  //---------------------------------------------------------------------------

  void BinaryFile::openOutputFile(const filesys::path & path,
                                  filesys::ofstream &   outputFile)
  {
    if (filesys::is_directory(path))
      throw OpenFileException(path, "path is a directory");
    outputFile.open(path, std::ios::out | std::ios::binary);
    if (! outputFile)
      throw OpenFileException(path, "check permissions");
  }
}
