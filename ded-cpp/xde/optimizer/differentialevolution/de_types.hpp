/*
 * Copyright (c) 2017, Adrian Michel
 * http://www.amichel.com
 *
 * This software is released under the 3-Clause BSD License
 *
 * The complete terms can be found in the attached LICENSE file
 * or at https://opensource.org/licenses/BSD-3-Clause
 */

#pragma once

#include <exception>
#include <vector>

namespace amichel {
namespace de {

using DVector = std::vector<double>;

/**
 * shared pointer to a DVector
 */
using DVectorPtr = std::shared_ptr<DVector>;

/**
 * de exception conforms to the C++ standard (MS implementation
 * of std::exception has non-standard constructors)
 */
class exception : public std::exception {
 private:
  const std::string m_what;

 public:
  virtual ~exception() throw() {}
  exception(const char* what) : m_what(what != 0 ? what : "") {}
  virtual const char* what() const throw() { return m_what.c_str(); }
};
}  // namespace de
}  // namespace amichel
