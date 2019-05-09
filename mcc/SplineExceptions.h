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

#ifndef MCC_SPLINE_EXCEPTIONS_H
#define MCC_SPLINE_EXCEPTIONS_H

#include <string>

#include "IPoint.h"
#include "ProgramExceptions.h"
#include "XyzPoint.h"

namespace mcc
{
  // Exception when a singular matrix occurs while computing a thin plate spline.
  struct SingularMatrixException : public ProgramException
  {
    std::vector<XyzPoint> points;

    SingularMatrixException(const std::vector<const IPoint *> & points_)
      : ProgramException("singular matrix while computing thin plate spline"),
        points(points_.size())
    {
      for (std::vector<IPoint *>::size_type i = 0; i < points_.size(); ++i) {
        const IPoint * point = points_[i];
        points[i] = XyzPoint(point->x(), point->y(), point->z());
      }
    }

    ~SingularMatrixException() throw()
    {
    }
  };
}

#endif
