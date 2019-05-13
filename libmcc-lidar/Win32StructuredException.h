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

#ifndef MCC_WIN32_STRUCTURED_EXCEPTION_H
#define MCC_WIN32_STRUCTURED_EXCEPTION_H

namespace mcc
{
  // Represents a Win32 Structured Exception
  class Win32StructuredException
  {
    public:
      //  Construct an instance for a particular exception code.
      Win32StructuredException(unsigned int code);
 
      unsigned int code() const;

      // Set the function that translates a structured exception into
      // a C++ exception.  The function throws an instance of this
      // class.
      static void setTranslator();

    private:
        unsigned int code_;
  };

  //--------------------------------------------------------------------------

  inline
  Win32StructuredException::Win32StructuredException(unsigned int code)
    : code_(code)
  {
  }

  inline
  unsigned int Win32StructuredException::code() const
  {
    return code_;
  }
}

#endif
