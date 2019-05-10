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

#include <iostream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>

#include "config.h"
#include "ProgramExceptions.h"
#include "ProgramParameters.h"

namespace po = boost::program_options;

namespace mcc
{
  // Helper function for getting the value of a required option from a
  // variable map.
  template <typename T>
  const T & getRequired(const po::variables_map & vars,
                        const char *              name,
                        const std::string &       description)
  {
    if (vars.count(name))
      return vars[name].as<T>();
    else {
      std::string message = "Missing " + description;
      throw ProgramParameterException(message);
    }
  }

  //---------------------------------------------------------------------------

  const char * const versionText[] = {
    "MCC-LIDAR " VERSION_RELEASE " (revision " SUBVERSION_REVISION ")" ,
    "Multiscale Curvature Classification for LIDAR Data" ,
    "Copyright 2009-2011 Green Code LLC" ,
    "" ,
    "Licensed under the Apache License, Version 2.0 (the \"License\");" ,
    "you may not use this software except in compliance with the License." ,
    "A copy of the License was installed with this software.  You may" ,
    "also obtain a copy of the License at" ,
    "" ,
    "    http://www.apache.org/licenses/LICENSE-2.0" ,
    "" ,
    "Unless required by applicable law or agreed to in writing, software" ,
    "distributed under the License is distributed on an \"AS IS\" BASIS," ,
    "WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied." ,
    "See the License for the specific language governing permissions and" ,
    "limitations under the License."
  };

  //---------------------------------------------------------------------------

  ParseResult ProgramParameters::parseCmdLine(int   argc,
                                              char* argv[])
  {
    try {
      // Required options
      po::options_description required("Required options");
      required.add_options()
        ("spacing,s", po::value<double>(), " spacing for scale domain 2\n"
                                           "   domain 1 spacing = 0.5 * domain 2 spacing\n"
                                           "   domain 3 spacing = 1.5 * domain 2 spacing")
        ("threshold,t", po::value<double>(), " curvature threshold")
      ;

      // Options for output files
      po::options_description outputOpts("Output options");
      outputOpts.add_options()
        ("cell-size,c", po::value<double>(), " cell size of output ground surface")
      ;

      // Pass-related options
      po::options_description passOpts("Pass-related options");
      passOpts.add_options()
        ("nonground-pts,N", "write non-ground points classified by each pass")
        ("surfaces,S", "write raster surfaces interpolated by each pass")
      ;

      po::options_description misc("Other options");
      misc.add_options()
        ("version,v", "  display version")
        ("help,h", "  display help message")
      ;

      // Hidden options that correspond to operands
      po::options_description hidden("Hidden options");
      hidden.add_options()
        ("input-file", po::value<std::string>(), "input file")
        ("output-files", po::value< std::vector<std::string> >(), "output files")
      ;

      po::positional_options_description operands;
      operands.add("input-file", 1).add("output-files", -1);

      po::options_description all("All options");
      all.add(required).add(outputOpts).add(passOpts).add(misc).add(hidden);

      po::variables_map vars;
      po::store(po::command_line_parser(argc, argv).options(all).positional(operands).run(),
                vars);
      po::notify(vars);

      if (vars.count("version")) {
        int numLines = sizeof(versionText) / sizeof(versionText[0]);
        for (int i = 0; i < numLines; ++i)
          std::cout << versionText[i] << std::endl;
        return ActionDone;
      }
      if (vars.count("help")) {
        std::cout << "Usage: " << argv[0] << " OPTIONS input-file output-file(s)" << std::endl
                  << "Output files:             At least one file required" << std::endl
                  << "  {filename}.las          Copy of input file with points classified by program" << std::endl
                  << "                            set to class code 2 (ground) or 1 (not ground)" << std::endl
                  << "  {filename}.asc          Raster surface interpolated from ground points" << std::endl
                  << "  {filename}.csv          Ground points in a comma-separated-values file" << std::endl
                  << required
                  << outputOpts
                  << passOpts
                  << misc;
        return ActionDone;
      }

      scaleDomain2Spacing = getRequired<double>(vars, "spacing", "post spacing");
      if (scaleDomain2Spacing <= 0.0)
        throw ProgramParameterException("spacing is not positive; it must be > 0");

      curvatureThreshold = getRequired<double>(vars, "threshold", "curvature threshold");
      if (curvatureThreshold < 0.0)
        throw ProgramParameterException("curvature threshold is negative; it must be = or > 0");

      writeNongroundPts   = vars.count("nonground-pts") > 0;
      writeRasterSurfaces = vars.count("surfaces")      > 0;

      inputFile  = getRequired<std::string>(vars, "input-file",  "input file");

      if (vars.count("output-files") == 0)
        throw ProgramParameterException("No output files");
      std::vector<std::string> outputFiles = vars["output-files"].as< std::vector<std::string> >();
      if (outputFiles.size() > 3) {
        std::string message("Too many output files:");
        BOOST_FOREACH(const std::string & outputFile, outputFiles) {
          message += " " + outputFile;
        }
        throw ProgramParameterException(message);
      }
      BOOST_FOREACH(const std::string & outputFile, outputFiles) {
        boost::filesystem::path outputPath(outputFile);
        if (boost::iequals(outputPath.extension().string(), ".asc")) {
          if (outputGroundSurface != "")
            throw ProgramParameterException("More than one output file with \".asc\" extension");
          outputGroundSurface = outputPath;
        }
        else if (boost::iequals(outputPath.extension().string(), ".csv")) {
          if (outputGroundPoints != "")
            throw ProgramParameterException("More than one output file with \".csv\" extension");
          outputGroundPoints = outputPath;
        }
        else if (boost::iequals(outputPath.extension().string(), ".las")) {
          if (outputLidarFile != "")
            throw ProgramParameterException("More than one output LIDAR file (*.las)");
          outputLidarFile = outputPath;
        }
        else if (outputPath.extension().string().size() <= 1) {  // extension = "." or ""
          throw ProgramParameterException("Missing extension for output file: \"" + outputPath.string() + "\"");
        }
        else {
          throw ProgramParameterException("Unknown extension for output file: \"" + outputPath.extension().string() + "\"");
        }
      }

      if (outputGroundSurface != "") {
        boost::format descriptionFormat("cell size for output file \"%s\"");
        std::string description = boost::str(descriptionFormat % outputGroundSurface);
        groundSurfaceCellSize  = getRequired<double>(vars, "cell-size",  description);
      }

      return Success;
    }
    catch (po::error & exc) {
      throw ProgramParameterException(exc.what());
    }
  }
}
