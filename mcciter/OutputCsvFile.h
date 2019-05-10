// Copyright 2010 Green Code LLC
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

#ifndef MCC_OUTPUT_CSV_FILE_H
#define MCC_OUTPUT_CSV_FILE_H

#include <fstream>
#include <string>

namespace mcc
{
  //  A text file that data is written to as rows of comma-separated values.
  class OutputCsvFile
  {
    public:
      // Open the specified path for writing, and write a row with column
      // headings.
      void virtual open(const std::string & path);

      // The path of the currently open file.
      const std::string & path() const;

      void close();

      // Is the file open?
      bool isOpen() const;
      operator bool();

      virtual ~OutputCsvFile();
 
    protected:
      void virtual writeHeadings() = 0;
      std::ofstream & stream();

    private:
      std::string path_;
      std::ofstream file_;
  };

  //---------------------------------------------------------------------------

  inline bool OutputCsvFile::isOpen() const
  {
    return file_.is_open();
  }

  inline const std::string & OutputCsvFile::path() const
  {
    return path_;
  }

  inline OutputCsvFile::operator bool()
  {
    return file_.is_open();
  }

  inline OutputCsvFile::~OutputCsvFile()
  {
  }

  inline std::ofstream & OutputCsvFile::stream()
  {
    return file_;
  }
}

#endif
