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

#ifndef MCC_SEQUENCE_H
#define MCC_SEQUENCE_H

#include <limits>
#include <boost/iterator.hpp>

namespace mcc
{
  // Represents a sequence of integer values (increasing or decreasing).
  // Designed for use with Boost Foreach library.
  // Template argument T represents the underlying integer type.
  template <typename T>
  class Sequence
  {
    public:
      // Create a sequence [start, end]
      Sequence(T start,
               T end);

      class iterator;
      typedef iterator const_iterator;

      iterator begin();
      iterator end();
      const_iterator begin() const;
      const_iterator end() const;

    private:
      T start_;
      T end_;
      int increment_;
      int end_increment_; // for end() methods
  };

  //---------------------------------------------------------------------------

  template <typename T>
  class Sequence<T>::iterator : public std::iterator<std::forward_iterator_tag, T>
  {
    public:
      iterator(T value,
               int increment);
      T & operator*();
      iterator & operator++();
      bool operator==(const iterator & other);

    private:
      T value_;
      int increment_;
      T lastValue_;
  };

  //---------------------------------------------------------------------------

  template <typename T>
  Sequence<T>::iterator::iterator(T value,
                                  int increment)
    : value_(value),
      increment_(increment),
      lastValue_(increment > 0 ? std::numeric_limits<T>::max()
                               : std::numeric_limits<T>::min())
  {
  }

  template <typename T>
  inline
  T & Sequence<T>::iterator::operator*()
  {
    return value_;
  }

  template <typename T>
  typename Sequence<T>::iterator & Sequence<T>::iterator::operator++()
  {
    if (value_ == lastValue_)
      increment_ = 0;         // So we don't advance past last value
    else
      value_ += increment_;
    return *this;
  }

  template <typename T>
  inline
  bool Sequence<T>::iterator::operator==(const typename Sequence<T>::iterator & other)
  {
    // value AND increment must be the same
    return (value_ == other.value_) && (increment_ == other.increment_);
  }

  //---------------------------------------------------------------------------

  template <typename T>
  inline
  Sequence<T>::Sequence(T start,
                        T end)
    : start_(start), end_(end), increment_(start <= end ? 1 : -1)
  {
    // If we're counting up and end value is the maximum value for T, then
    // we set the end increment to 0, since we can't represent end+1 in T.
    if ((increment_ == 1) && (end == std::numeric_limits<T>::max()))
      end_increment_ = 0;

    // If we're counting down and end value is the minimum value for T, then
    // we set the end increment to 0, since we can't represent end-1 in T.
    else if ((increment_ == -1) && (end == std::numeric_limits<T>::min()))
      end_increment_ = 0;
    else
      end_increment_ = increment_;
  }

  template <typename T>
  inline
  typename Sequence<T>::iterator Sequence<T>::begin()
  {
    return iterator(start_, increment_);
  }

  template <typename T>
  inline
  typename Sequence<T>::iterator Sequence<T>::end()
  {
    return iterator(end_ + end_increment_, end_increment_);
  }

  template <typename T>
  inline
  typename Sequence<T>::const_iterator Sequence<T>::begin() const
  {
    return iterator(start_, increment_);
  }

  template <typename T>
  inline
  typename Sequence<T>::const_iterator Sequence<T>::end() const
  {
    return iterator(end_ + end_increment_, end_increment_);
  }
}

#endif
