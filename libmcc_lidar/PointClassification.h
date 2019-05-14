// Copyright 2009-2011 Green Code LLC
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

#ifndef MCC_POINT_CLASSIFICATION_H
#define MCC_POINT_CLASSIFICATION_H

namespace mcc
{
  // Has a point been classified as ground or non-ground yet?
  enum PointClassification {
    NotYetClassified = 0,
    Ground = 1,
    NonGround = 2,
    Duplicate = 3,      // has same x,y,z coordinates as 1 or more other points
    GroundDuplicate = 4 // a duplicate of a ground point 
  };

  // A function to improve the readability of testing for a ground-related
  // classification.
  inline
  bool isGround(PointClassification classification)
  {
    return (classification == Ground) || (classification == GroundDuplicate);
  }
}

#endif
