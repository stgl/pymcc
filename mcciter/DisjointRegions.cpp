// Copyright 2009-2010 Green Code LLC
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

#include <algorithm>
#include <cmath>
#include <iostream>

#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

#include "tpsdemo/linalg3d-double.h"

#include "DisjointRegions.h"
#include "Geometry3D.h"
#include "Grid.h"
#include "IPointVector.h"
#include "LineIndent.h"
#include "ProgressBar.h"
#include "RasterSurface.h"

namespace mcc
{
  DisjointRegions::DisjointRegions()
    : iterationState_(RegionIteration_Done)
  {
  }

  //---------------------------------------------------------------------------

  const unsigned int desiredPtsPerRegion = 12;  // Default in ArcGIS Spline

  //---------------------------------------------------------------------------

  // Represents a point that's in a neighboring region.
  struct NeighborPoint
  {
    const IPoint * point;
    double distance;

    NeighborPoint(const IPoint *             neighborPt,
                  const CoordinateInterval & xInterval,
                  const CoordinateInterval & yInterval)
      : point(neighborPt)
    {
      // Compute distance from region specified by x and y intervals.
      double xDistance = distanceToInterval(point->x(), xInterval);
      double yDistance = distanceToInterval(point->y(), yInterval);
      distance = std::max(xDistance, yDistance);
    }

    double distanceToInterval(Coordinate                 value,
                              const CoordinateInterval & interval)
    {
      if (value < interval.lowerBound())
        return interval.lowerBound() - value;
      else if (interval.upperBound() < value)
        return value - interval.upperBound();
      else
        return 0.0;
    }

    bool operator<(const NeighborPoint & rhs) const
    {
      return distance < rhs.distance;
    }
  };

  //---------------------------------------------------------------------------

  // Functor class for adding a neighbor point to a region's point list.
  class AppendPoint
  {
    public:
      AppendPoint(std::vector<const IPoint *> & points)
        : points_(points)
      {
      }

      void operator()(const NeighborPoint & neighbor) const
      {
        points_.push_back(neighbor.point);
      }

    private:
      std::vector<const IPoint *> & points_;
  };

  //---------------------------------------------------------------------------

  // Location of a cell's neighbor relative to the cell itself.
  struct RelativeLocation
  {
    int rowOffset;
    int columnOffset;

    RelativeLocation(int rOffset,
                     int cOffset)
      : rowOffset(rOffset),
        columnOffset(cOffset)
    {
    }

    RelativeLocation & operator+=(const RelativeLocation & relLoc)
    {
      rowOffset += relLoc.rowOffset;
      columnOffset += relLoc.columnOffset;
      return *this;
    }
  };

  //---------------------------------------------------------------------------

  // Get the neighboring region to a particular region (specified by its cell).
  // If the relative location of the neighbor is outside the grid of regions,
  // 0 is returned.
  const InterpolationRegion * getNeighbor(Grid<InterpolationRegion> &  regions,
                                          const Cell &                 cell,
                                          const RelativeLocation       neighborLocation)
  {
    long row = cell.row() + neighborLocation.rowOffset;
    if (row < 0 || row >= (long) regions.rows())
      return 0;
    long column = cell.column() + neighborLocation.columnOffset;
    if (column < 0 || column >= (long) regions.columns())
      return 0;
    return &(regions((unsigned int) row, (unsigned int) column));
  }

  //---------------------------------------------------------------------------

  // Get points from the regions in the outermost ring of a region's
  // neighborhood.  The ring is traversed in clock-wise order:
  //
  //   +---+---+---+---+---+
  //   | P | A | B | C | D |
  //   +---+---+---+---+---+
  //   | O | 8 | 1 | 2 | E |
  //   +---+---+---+---+---+
  //   | N | 7 | * | 3 | F |
  //   +---+---+---+---+---+
  //   | M | 6 | 5 | 4 | G |
  //   +---+---+---+---+---+
  //   | L | K | J | I | H |
  //   +---+---+---+---+---+
  //
  //  For the region marked "*" in the diagram above, if its neighborhood is
  //  3x3, then the eight regions marked 1 to 8 are traversed.  If the
  //  neighborhood is 5x5, then the regions marked "A" to "P" are traversed.
  //
  //  The points in the traversed neighbors are appended to the "neighborPts"
  //  parameter.
  void getPointsFromOuterRing(Grid<InterpolationRegion> &  regions,
                              const Cell &                 cell,
                              int                          neighborhoodSize,
                              std::vector<NeighborPoint> & neighborPts)
  {
    CoordinateInterval xInterval = regions.getXInterval(cell.column()); // = cell.xInterval();
    CoordinateInterval yInterval = regions.getYInterval(cell.row());    // = cell.yInterval();

    RelativeLocation traverseRingAlongEdges[] = {
      RelativeLocation( 0,       right(1) ),  // top edge, left to right
      RelativeLocation( down(1), 0        ),  // right edge, top to bottom
      RelativeLocation( 0,       left(1)  ),  // bottom edge, right to left
      RelativeLocation( up(1),   0        ),  // left edge, bottom to top
    };

    // The upper left cell in current neighborhood ring
    int neighborhoodRadius = (neighborhoodSize - 1) / 2;
    RelativeLocation neighborLocation(up(neighborhoodRadius), left(neighborhoodRadius));

    int nCellsPerEdge = neighborhoodSize - 1;

    BOOST_FOREACH(const RelativeLocation & locOfNextNeighborAlongEdge, traverseRingAlongEdges) {
      for (int i = 0; i < nCellsPerEdge; ++i) {
        neighborLocation += locOfNextNeighborAlongEdge;
        const InterpolationRegion * neighbor = getNeighbor(regions, cell, neighborLocation);
        if (neighbor) {
          BOOST_FOREACH(const IPoint * point, neighbor->pts) {
            neighborPts.push_back(NeighborPoint(point, xInterval, yInterval));
          }
        }
      }
    }
  }

  //---------------------------------------------------------------------------

  int DisjointRegions::subdivide(const IPointVector &  points,
                                 PointSelector         pointSelector,
                                 const RasterSurface & raster)
  {
    // Subdivide the raster's area into non-overlapping regions.  The goal is
    // to have approximately the same # of points in each region. 

    int desiredNumRegions = points.count() / desiredPtsPerRegion;
      // Rounding down because one less region means possibly more pts per
      // region.

    double rasterHeight = raster.rows() * raster.cellSize();
    double rasterWidth = raster.columns() * raster.cellSize();
    double rasterArea = rasterHeight * rasterWidth;
    double desiredRegionArea = rasterArea / desiredNumRegions;

    // The size of a region if it was the optimal shape = square
    double desiredRegionSize = std::sqrt(desiredRegionArea);

    // But the desired region size is not guaranteed to divide both the height
    // and width of the raster evenly.  So stretch the regions' heights and
    // widths until they evenly divide the raster's height and width,
    // respectively.  Stretching ensures that regions do not lose points.  The
    // resulting region shape is likely not to be square.
    int nRows = int(std::floor(rasterHeight / desiredRegionSize));
	if (nRows > int(raster.rows())){
		nRows = int(raster.rows());
	} 
    double regionHeight = rasterHeight / nRows;

    int nColumns = int(std::floor(rasterWidth / desiredRegionSize));
	if (nColumns > int(raster.columns())){
		nColumns = int(raster.columns());
	} 
    double regionWidth = rasterWidth / nColumns;

    // Create the 2-d arry of InterpolationRegion (nRows, nColumns)
    regions_ = boost::make_shared< Grid<InterpolationRegion> >(nRows, nColumns, raster.lowerLeft(),
                                                               Coordinate(regionHeight),
                                                               Coordinate(regionWidth));

    LineIndent indent("  ");

    // Sort points into the regions
    std::cout << indent << "Sorting points into regions..." << std::endl;
    BOOST_FOREACH(const IPoint & point, points) {
      Cell cell = regions_->getCell(point.x(), point.y());
      if ((*pointSelector)(point))
        (*regions_)[cell].pts.push_back(& point);
      else
        (*regions_)[cell].nPtsNotSelected++;
    }

    // Determine the cell block for each region.
    int regionRow = regions_->topRow();
    Coordinate regionRow_minY = regions_->getYInterval(regionRow).lowerBound();
    BOOST_FOREACH(unsigned int rasterRow, raster.topToBottom()) {
      // If first (top) row, then scan across cell columns computing widths of
      // cell blocks.
      if (rasterRow == raster.topRow()) {
          const unsigned int regionTopRow = regions_->topRow();
          unsigned int regionColumn = regions_->leftColumn();
          InterpolationRegion * currentRegion = &( (*regions_)(regionTopRow, regionColumn) );
          Coordinate currentRegion_maxX = regions_->getXInterval(regionColumn).upperBound();
          BOOST_FOREACH(unsigned int rasterColumn, raster.leftToRight()) {
            Cell cell = raster.getCell(raster.topRow(), rasterColumn);
            if (rasterColumn == raster.leftColumn()) {
              currentRegion->cellBlock = CellBlock(cell /* upperLeft */);  // cell block height and width = 1
            }
            else if (cell.x() < currentRegion_maxX) {
              // The cell is in the current region, so increment the cell
              // block's width.
              currentRegion->cellBlock.width += 1;
            } else {
              // We've moved right into a new region.
              regionColumn += right(1);
              currentRegion = &( (*regions_)(regionTopRow, regionColumn) );
              currentRegion->cellBlock = CellBlock(cell /* upperLeft */);  // cell block height and width = 1
              currentRegion_maxX = regions_->getXInterval(regionColumn).upperBound();
            }
          }
      } else {
        // Current raster row is not the top row.  So two possibilities:
        //   A) the centers of the current row's cells are in the current
        //      region row, or
        //   B) the current row's cells in the next lower region row
        Cell leftMostCell = raster.getCell(rasterRow, raster.leftColumn());
        if (leftMostCell.y() > regionRow_minY) {
          // Case (A) - still in current region row, so go through the regions
          // on the current region row, and increment their cell block heights.
          BOOST_FOREACH(unsigned int regionColumn, regions_->leftToRight()) {
            (*regions_)(regionRow, regionColumn).cellBlock.height += 1;
          }
        } else {
          // Case (B) - moved down into new region row, so scan across region
          // columns, copying the cell block widths from the regions in the 
          // top region row into their corresponding regions of the current
          // region row.
          regionRow += down(1);
          regionRow_minY = regions_->getYInterval(regionRow).lowerBound();
          BOOST_FOREACH(unsigned int regionColumn, regions_->leftToRight()) {
            const InterpolationRegion & correspondingRegionInTopRow = (*regions_)(regions_->topRow(), regionColumn);
            int upperLeftCell_column = correspondingRegionInTopRow.cellBlock.upperLeftCell.column();

            CellBlock & currentRegion_cellBlock = (*regions_)(regionRow, regionColumn).cellBlock;
            currentRegion_cellBlock.upperLeftCell = raster.getCell(rasterRow, upperLeftCell_column);
            currentRegion_cellBlock.height = 1;
            currentRegion_cellBlock.width = correspondingRegionInTopRow.cellBlock.width;
          }
        }  // else Case (B)
      }  // else current raster row not top row
    }  // for each raster row

    iterationState_ = RegionIteration_Initialized;
    raster_ = &(raster);

    return nRows * nColumns;
  }

  //---------------------------------------------------------------------------

  // Variables with information about the current region

  // The current region's cell in the grid of regions.
  Cell currentRegionCell(XYCoordinates(0,0), 0, 0);

  // The current size of the neighborhood of the current region, from which
  // points are being used for the region's spline calculation.  Neighborhood
  // size is NxN where N is odd, i.e., 1x1 (for the region itself), 3x3, 5x5,
  // etc.
  int neighborhoodSize;

  // List of points from the outer ring of the region's neighborhood.
  std::vector<NeighborPoint> neighborPts;

  // The number of neighboring points left in the outer ring that have not yet
  // been added to the current region's set of points for spline calculation.
  int nPointsLeftInOuterRing;

  // The location in the list of the neighboring points from the outer ring of
  // the next available point that can be added to the current region's set of
  // points.
  int indexNextAvailableNeighbor;

  //---------------------------------------------------------------------------

  const IInterpolationRegion * DisjointRegions::getNextRegion()
  {
    switch (iterationState_) {
      case RegionIteration_Initialized :
        // Return the upper left region
        currentRegionRow_ = regions_->topRow();
        currentRegionColumn_ = regions_->leftColumn();
        iterationState_ = RegionIteration_InProgress;
        break;

      case RegionIteration_InProgress :
        // Advance to next cell in row major order
        if (currentRegionColumn_ != regions_->rightColumn())
          currentRegionColumn_ += right(1);
        else {
          // Advance to next row
          if (currentRegionRow_ != regions_->bottomRow()) {
            currentRegionRow_ += down(1);
            currentRegionColumn_ = regions_->leftColumn();
          } else {
            iterationState_ = RegionIteration_Done;
            return 0;
          }
        }
        break;

      default :
        assert(iterationState_ == RegionIteration_Done);
        return 0;
    }

    InterpolationRegion * currentRegion = &( (*regions_)(currentRegionRow_, currentRegionColumn_) );
    currentRegionCell = regions_->getCell(currentRegionRow_, currentRegionColumn_);

    // Determine the full list of points for current region's spline calculation.
    // Get points from neighboring regions if the region has less than the desired
    // # of selected points.
    InterpolationRegion::pointList = currentRegion->pts;

    neighborhoodSize = 1;
    neighborPts.clear();
    nPointsLeftInOuterRing = 0;

    unsigned int nSelectedPts = currentRegion->pts.size();
    if (nSelectedPts < desiredPtsPerRegion) {
      addNeighborPointsToCurrentRegion(desiredPtsPerRegion - nSelectedPts);
    }

    // Compute the cell list for the current region
    InterpolationRegion::cellList.clear();

    const CellBlock & cellBlock = currentRegion->cellBlock;
    const Cell & upperLeftCell = cellBlock.upperLeftCell;
    unsigned int blockTop    = upperLeftCell.row();
    unsigned int blockBottom = blockTop + down(cellBlock.height - 1);
    unsigned int blockLeft   = upperLeftCell.column();
    unsigned int blockRight  = blockLeft + right(cellBlock.width - 1);

    BOOST_FOREACH(unsigned int row, Sequence<unsigned int>(blockTop, blockBottom)) {
      BOOST_FOREACH(unsigned int column, Sequence<unsigned int>(blockLeft, blockRight)) {
        InterpolationRegion::cellList.push_back(raster_->getCell(row, column));
      }
    }
    return currentRegion;
  }

  //---------------------------------------------------------------------------

  void DisjointRegions::addNeighborPointsToCurrentRegion(int nPoints)
  {
    while (nPoints > 0) {
      while (nPointsLeftInOuterRing == 0) {
        // Expand the neighborhood, and get points from the new outer ring.
        neighborPts.clear();
        neighborhoodSize += 2;  // first 3x3, then 5x5, 7x7, ...
        getPointsFromOuterRing(*regions_, currentRegionCell, neighborhoodSize, neighborPts);
        nPointsLeftInOuterRing = neighborPts.size();
        std::sort(neighborPts.begin(), neighborPts.begin());
        indexNextAvailableNeighbor = 0;
      }

      int nPtsToAdd = (nPointsLeftInOuterRing < nPoints) ? nPointsLeftInOuterRing : nPoints;
      std::for_each(neighborPts.begin() + indexNextAvailableNeighbor,
                    neighborPts.begin() + indexNextAvailableNeighbor + nPtsToAdd,
                    AppendPoint(InterpolationRegion::pointList));
      indexNextAvailableNeighbor += nPtsToAdd;
      nPointsLeftInOuterRing -= nPtsToAdd;
      nPoints -= nPtsToAdd;
    }
  }
}
