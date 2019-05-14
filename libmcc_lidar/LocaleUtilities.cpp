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

#include "LocaleUtilities.h"

namespace mcc
{
  std::locale LocaleUtilities::getLocaleFromEnvVars()
  {
#if defined(__APPLE__)
    // On OS X, the LANG and LC_* environment variables aren't handled properly.
    // Even if they're correctly set, a std::runtime_error is thrown.  So we
    // just return the default locale instead.
    return std::locale();
#else
    return std::locale("");
#endif
  }

  //---------------------------------------------------------------------------

  // A custom numeric punctuation facet that uses the period for decimal point,
  // the comma for thousands-separator, and groups digits into groups of 3.
  struct CustomNumPunct : std::numpunct<char>
  {
    char do_decimal_point() const    { return '.';  }
    char do_thousands_sep() const    { return ',';  }
    std::string do_grouping() const  { return "\3"; }
  };

  //---------------------------------------------------------------------------

  std::locale LocaleUtilities::getLocaleWithNumGrouping()
  {
    std::locale localeFromEnv(getLocaleFromEnvVars());

    // Does the locale do grouping for numbers?
    const std::numpunct<char> & numpunct = std::use_facet< std::numpunct<char> >(localeFromEnv);
    if (numpunct.grouping() != "")
      return localeFromEnv;

    // Create a custom locale from C locale with custom numeric punctation
    return std::locale(std::locale("C"), new CustomNumPunct());
  }
}
