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

#ifndef MCC_ALGORITHM_H
#define MCC_ALGORITHM_H

namespace mcc
{
  class ISurfaceInterpolation;
  class IUnclassifiedPoints;

  class Algorithm
  {
    public:
      Algorithm(ISurfaceInterpolation & surfaceInterpolation,
                bool                    writeNongroundPts,
                bool                    writeRasterSurfaces);
      void classifyPoints(IUnclassifiedPoints & points,
                          double                scaleDomain2Spacing,
                          double                curvatureThreshold);

      static const double tension;  // parameter "f" in MCC paper

    private:
      ISurfaceInterpolation & surfaceInterpolation_;
      bool writeNongroundPts_;
      bool writeRasterSurfaces_;
  };
}

#endif
