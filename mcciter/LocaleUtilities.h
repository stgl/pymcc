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

#ifndef MCC_LOCALE_UTILITIES_H
#define MCC_LOCALE_UTILITIES_H

#include <locale>

namespace mcc
{
  // Helper functions for working with locales.
  class LocaleUtilities
  {
    public:

      //  Get the locale as specified by the environment variables LANG and
      //  LC_ALL.
      //
      //  Note: because of bug on OS X with getting locale from these
      //  environment variables, the function returns the default locale
      //  instead.
      static std::locale getLocaleFromEnvVars();

      // Get a locale that has grouping for numbers (i.e., uses thousands
      // separator).  If the locale specified by environment variables LANG and
      // LC_ALL has grouping, then the function returns it.  Otherwise, it will
      // return a custom locale based on the classic C locale with a custom
      // facet for numeric punctuation:
      //
      //   decimal_point = '.'
      //   thousands_sep = ','
      //   grouping = "\x03"
      static std::locale getLocaleWithNumGrouping();
  };
}

#endif
