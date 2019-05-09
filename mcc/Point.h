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

#ifndef MCC_POINT_H
#define MCC_POINT_H

#include "IPoint.h"
#include "XyzPoint.h"

namespace mcc
{
  class Point : public IPoint
  {
    public:
      // Create an unclassified point with coordinates (0, 0, 0)
      Point();

      // Set the point's coordinates.
      void setCoordinates(const XyzPoint & xyzPoint);

      Coordinate x() const;
      Coordinate y() const;
      Coordinate z() const;

      PointClassification classification() const;

      void classifyAs(PointClassification classification);

    private:
      Coordinate x_;
      Coordinate y_;
      Coordinate z_;
      PointClassification classification_;
  };

  //---------------------------------------------------------------------------
  
  inline
  Point::Point()
    : x_(0), y_(0), z_(0), classification_(NotYetClassified)
  {
  }

  inline
  void Point::setCoordinates(const XyzPoint & point)
  {
    x_ = point.x;
    y_ = point.y;
    z_ = point.z;
  }

  inline Coordinate Point::x() const { return x_; }
  inline Coordinate Point::y() const { return y_; }
  inline Coordinate Point::z() const { return z_; }

  inline
  PointClassification Point::classification() const { return classification_; }

  inline
  void Point:: classifyAs(PointClassification classification)
  {
    classification_ = classification;
  }
}

#endif
