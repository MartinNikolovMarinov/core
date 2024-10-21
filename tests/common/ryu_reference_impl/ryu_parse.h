#pragma once

namespace ryu {

enum Status {
  SUCCESS,
  INPUT_TOO_SHORT,
  INPUT_TOO_LONG,
  MALFORMED_INPUT
};

enum Status s2f_n(const char * buffer, const int len, float * result);
enum Status s2f(const char * buffer, float * result);

} // namespace ryu
