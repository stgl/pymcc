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

#ifndef MCC_LINE_INDENT_H
#define MCC_LINE_INDENT_H

#include <iostream>
#include <string>

namespace mcc
{
  // Represents the text string for indenting a line written to an output
  // stream.  Each instance represents additional level to indent a line.
  //
  // For example, this code:
  //
  //   void foo ()
  //   {
  //     LineIndent indent(". ");
  //     cout << indent << "foo 1" << endl;
  //     bar()
  //     cout << indent << "foo 2" << endl;
  //   }
  //
  //   void bar()
  //   {
  //     LineIndent indent("> ");
  //     cout << indent << "bar" << endl;
  //   }
  //
  // will produce this output:
  //
  //  . foo 1
  //  . > bar
  //  . foo 2
  class LineIndent
  {
    // The current overall indent (all the active indents concatenated).
    static std::string overallIndent;

    public:
      // Append my indent to the overall indent.
      LineIndent(const std::string & myIndent);

      // Remove my indent from the overall ndent.
      ~LineIndent();

      // Write the current overall indent to an output stream.
      friend std::ostream & operator<<(std::ostream &     stream,
                                       const LineIndent & indent);

    private:
      std::string previousIndent_;  // previous value of overall indent
  };

  //---------------------------------------------------------------------------

  inline
  LineIndent::~LineIndent()
  {
    overallIndent = previousIndent_;
  }

  inline
  std::ostream & operator<<(std::ostream &     stream,
                            const LineIndent & indent)
  {
    stream << LineIndent::overallIndent;
    return stream;
  }
}

#endif
