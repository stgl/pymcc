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

#include "ProgressBar.h"

namespace mcc
{
  ProgressBar::ProgressBar(std::ostream & stream,
                           double         workToBeDone)
    : stream_(stream),
      workToBeDone_(workToBeDone)
  {
    stream_ << "0%" << std::flush;
    percentDisplayed_ = 0;
  }

  //---------------------------------------------------------------------------

  void ProgressBar::update(double workDone)
  {
    int percentDone = int((workDone / workToBeDone_) * 100);

    // Round down to nearest 2% for display purposes
    int percentToDisplay = (percentDone & (~ 0x01));  // Clear least-significant bit

    while (percentDisplayed_ < percentToDisplay) {
      percentDisplayed_ += 2;  // Advance by 2%
      if (percentDisplayed_ % 10 == 0) {
        stream_ << percentDisplayed_ << std::flush;
        if (percentDisplayed_ >= 100)
          stream_ << "%" << std::flush;
      }
      else
        stream_ << "." << std::flush;
    }
  }
}
