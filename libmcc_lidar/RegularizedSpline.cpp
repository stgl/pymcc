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

#include <boost/foreach.hpp>

#include "IPoint.h"
#include "RegularizedSpline.h"
#include "SplineExceptions.h"

//-----------------------------------------------------------------------------

mcc::RegularizedSpline::RegularizedSpline(const std::vector<const IPoint *> & points,
                                          double                              regularization)
  : controlPoints_(points.size())
{
  std::vector<Vec>::size_type i = 0;
  BOOST_FOREACH(const IPoint * point, points) {
    // NOTE: Although the TPS Demo documentation has equations showing the z
    // coordinate as height, its source code treats y coordinate as the height.
    // So we interchange the y and z coordinates as we pass points to it.
    controlPoints_[i] = Vec(point->x(), point->z(), point->y());
    i++;
  }
  try {
    spline_ = boost::shared_ptr<tpsdemo::Spline>(new tpsdemo::Spline(controlPoints_, regularization));
  }
  catch (tpsdemo::SingularMatrixError) {
    throw SingularMatrixException(points);
  }
}

//-----------------------------------------------------------------------------

mcc::Coordinate mcc::RegularizedSpline::interpolateHeight(mcc::Coordinate x,
                                                          mcc::Coordinate y) const
{
  // NOTE: See the note in the constructor as why the y coordinate is passed to
  // the method below as the second parameter (which is the z coordinate in
  // the TPS Demo code).
  return Coordinate(spline_->interpolate_height(x, y));
}
