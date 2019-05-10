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

#ifndef MCC_PROGRESS_BAR_H
#define MCC_PROGRESS_BAR_H

#include <iostream>

namespace mcc
{
  // Represents a progress bar for output streams like std::cout.  The bar
  // shows what percentage of a task is done.  When a bar is created, it
  // writes:
  //
  //   0%
  //
  // A bar has a resolution of 2%, so when the task is 24.0% to 25.999% done,
  // this is what has been written:
  //
  //   0%....10....20..
  //
  // When the task is complete, the bar has written this:
  //
  //   0%....10....20....30....40.....50....60....70....80....90....100%

  class ProgressBar
  {
    public:
      // Initialize the bar to write to a particular output stream.  "0%" is
      // written to the stream.  The workToBeDone parameter represents the
      // amount of work to be done in units convenient to the caller.
      ProgressBar(std::ostream & stream,
                  double         workToBeDone);

      // Update the progress bar on the amount of work done so far.
      void update(double workDone);

    private:
      std::ostream & stream_;
      double workToBeDone_;
      int percentDisplayed_;  // 0 to 100, multiple of 2; i.e., 0, 2, 4, ...
  };
}

#endif
