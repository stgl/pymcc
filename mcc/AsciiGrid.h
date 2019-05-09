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

#ifndef MCC_ASCII_GRID_H
#define MCC_ASCII_GRID_H

#include <exception>
#include <fstream>
#include <string>

#include <boost/foreach.hpp>

#include "Grid.h"
#include "PointIO.h"

namespace mcc
{
  // Write an ASCII Grid file for a grid of data.
  template<typename T>
  void writeAsciiGrid(const Grid<T> &     grid,
                      const std::string & path)
  {
    std::ofstream outFile(path.c_str());
    if (! outFile)
      throw std::runtime_error("Could not open file for writing");

    outFile << "ncols        " << grid.columns()     << std::endl
            << "nrows        " << grid.rows()        << std::endl
            << "xllcorner    " << toStr(grid.lowerLeft().x) << std::endl
            << "yllcorner    " << toStr(grid.lowerLeft().y) << std::endl
            << "cellsize     " << grid.cellHeight()  << std::endl;

    if (grid.hasNoDataValue())
      outFile << "NODATA_value " << grid.noDataValue() << std::endl;

    // Write cell values
    BOOST_FOREACH(unsigned int row, grid.topToBottom()) {
      BOOST_FOREACH(unsigned int column, grid.leftToRight()) {
        outFile << grid(row, column) << " ";
      }
      outFile << std::endl;
    }
    outFile.close();
  }
}

#endif
