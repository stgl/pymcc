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

#include <cassert>
#include <vector>
#include <boost/foreach.hpp>

#include "AsciiGrid.h"
#include "RasterSurface.h"

namespace mcc
{
  RasterSurface::RasterSurface(unsigned int          rows,
                               unsigned int          columns,
                               const XYCoordinates & lowerLeft,
                               Coordinate            cellSize)
    : Grid<Coordinate>(rows, columns, lowerLeft, cellSize, cellSize)
  {
    assert(rows > 0);
    assert(columns > 0);
    assert(cellSize > 0);
  }

  //---------------------------------------------------------------------------

  // Determine the row or column index for a kernel, adjusting for those cells
  // near an edge.
  unsigned int getKernelEdge(unsigned int index,
                             unsigned int edgeIndex,
                             unsigned int kernelRadius)
  {
    if (index == edgeIndex)
      return edgeIndex;
    else if (index < edgeIndex) {
      // increasing index moves toward edge
      if (edgeIndex - index < kernelRadius)
        return edgeIndex;
      else
        return index + kernelRadius;
    } else {
      // decreasing index moves toward edge
      if (index - edgeIndex < kernelRadius)
        return edgeIndex;
      else
        return index - kernelRadius;
    }
  }

  //---------------------------------------------------------------------------

  void RasterSurface::average(int kernelSize)
  {
    assert((kernelSize > 0) && (kernelSize % 2 == 1)); // kernel is positive and odd

    // Allocate row buffers to hold averages
    boost::multi_array<Coordinate, 2> averages(boost::extents[kernelSize][columns()]);
    std::vector<bool> rowNeedsCopying(kernelSize);
    for (int i = 0; i < kernelSize; ++i)
      rowNeedsCopying[i] = false;
    std::vector<unsigned int> correspondingRasterRow(kernelSize);

    int currentBufferRow = 0;
    Grid<Coordinate> & grid = *this;

    // "radius" for the square kernel
    unsigned int kernelRadius = (kernelSize - 1) / 2;

    BOOST_FOREACH(unsigned int row, topToBottom()) {
      // If the buffer row that we're about to fill has averages to be copied,
      // then do copying now.
      if (rowNeedsCopying[currentBufferRow]) {
        unsigned int rasterRow = correspondingRasterRow[currentBufferRow];
        BOOST_FOREACH(unsigned int column, leftToRight()) {
          grid(rasterRow, column) = averages[currentBufferRow][column];
        }
        rowNeedsCopying[currentBufferRow] = false;
      }

      // Compute averages for current raster row, and store them in buffer
      BOOST_FOREACH(unsigned int column, leftToRight()) {
        Coordinate average;
        // If a cell has NODATA value, then leave it as such.  Only compute an
        // average if the cell has a value.
        if (grid.hasNoDataValue() && grid(row, column) == grid.noDataValue()) {
          average = grid.noDataValue();
        }
        else {
          // Determine the cell's neighborhood based on kernelSize
          unsigned int kernelTop    = getKernelEdge(row,    topRow(),      kernelRadius);
          unsigned int kernelBottom = getKernelEdge(row,    bottomRow(),   kernelRadius);
          unsigned int kernelLeft   = getKernelEdge(column, leftColumn(),  kernelRadius);
          unsigned int kernelRight  = getKernelEdge(column, rightColumn(), kernelRadius);
          Coordinate sum = 0;
          unsigned int count = 0;
          BOOST_FOREACH(unsigned int r, Sequence<unsigned int>(kernelTop, kernelBottom)) {
            BOOST_FOREACH(unsigned int c, Sequence<unsigned int>(kernelLeft, kernelRight)) {
              Coordinate value;
              if (grid.getData(r, c, value)) {
                sum += value;
                count++;
              }
            }
          }
          average = sum / count;
        }
        averages[currentBufferRow][column] = average;
      }
      rowNeedsCopying[currentBufferRow] = true;
      correspondingRasterRow[currentBufferRow] = row;

      // Advance to the next buffer row
      currentBufferRow = (currentBufferRow + 1) % kernelSize;
    }

    // Copy those buffer rows that remain to be copied.
    for (int bufferRow = 0; bufferRow < kernelSize; ++bufferRow) {
      if (rowNeedsCopying[bufferRow]) {
        unsigned int rasterRow = correspondingRasterRow[bufferRow];
        BOOST_FOREACH(unsigned int column, leftToRight()) {
          grid(rasterRow, column) = averages[bufferRow][column];
        }
        rowNeedsCopying[bufferRow] = false;
      }
    }
  }

  //---------------------------------------------------------------------------

  void RasterSurface::writeAsciiGrid(const std::string & path) const
  {
    mcc::writeAsciiGrid<Coordinate>(*this, path);
  }
}
