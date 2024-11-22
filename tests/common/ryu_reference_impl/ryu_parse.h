#pragma once

#include <core_compiler.h>

PRAGMA_WARNING_PUSH
PRAGMA_WARNING_SUPPRESS_ALL

namespace ryu {

enum Status {
  SUCCESS,
  INPUT_TOO_SHORT,
  INPUT_TOO_LONG,
  MALFORMED_INPUT
};

enum Status s2d_n(const char * buffer, const int len, double * result);
enum Status s2d(const char * buffer, double * result);

enum Status s2f_n(const char * buffer, const int len, float * result);
enum Status s2f(const char * buffer, float * result);

} // namespace ryu

PRAGMA_WARNING_POP
