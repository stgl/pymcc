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

#ifndef MCC_PROGRAM_PARAMETERS_H
#define MCC_PROGRAM_PARAMETERS_H

#include <boost/filesystem.hpp>

namespace mcc
{
  // The result of parsing the command-line arguments for program parameters.
  enum ParseResult {
    Success,    // The arguments were successfully parsed.
    ActionDone, // The arguments were successfully parsed, and the requested
                // action was performed (for example, the "--help" option was
                // parsed and a help message was printed).
  };
    
  struct ProgramParameters
  {
    boost::filesystem::path inputFile;
    boost::filesystem::path outputLidarFile;
    boost::filesystem::path outputGroundPoints;
    boost::filesystem::path outputGroundSurface;
    double groundSurfaceCellSize;
    double scaleDomain2Spacing;
    double curvatureThreshold;
    bool writeNongroundPts;
    bool writeRasterSurfaces;

    // Parse command-line arguments for the program's parameters.  If there's
    // an error with an argument, then ProgramParameterException is thrown.
    ParseResult parseCmdLine(int   argc,
                             char* argv[]);
  };
}

#endif
