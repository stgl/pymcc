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

#ifndef MCC_IRASTER_SURFACE_H
#define MCC_IRASTER_SURFACE_H

#include <string>
#include "Coordinate.h"

namespace mcc
{
  class IRasterSurface
  {
    public:
      virtual void average(int kernelSize) = 0;
      virtual Coordinate operator()(Coordinate x,
                                    Coordinate y) const = 0;
      virtual void writeAsciiGrid(const std::string & path) const = 0;
      virtual ~IRasterSurface() { }
  };
}

#endif
