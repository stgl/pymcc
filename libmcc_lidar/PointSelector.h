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

#ifndef MCC_POINT_SELECTOR_H
#define MCC_POINT_SELECTOR_H

namespace mcc
{
  class IPoint;

  // A function for selecting points from a group of points.  Returns true if
  // a point is selected.
  typedef bool (*PointSelector)(const IPoint & point);
}

#endif
