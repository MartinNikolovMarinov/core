#pragma once

#include <inttypes.h>
#include <core_compiler.h>

PRAGMA_WARNING_PUSH
PRAGMA_WARNING_SUPPRESS_ALL

namespace ryu {

int d2s_buffered_n(double f, char* result);
void d2s_buffered(double f, char* result);
char* d2s(double f);

int f2s_buffered_n(float f, char* result);
void f2s_buffered(float f, char* result);
char* f2s(float f);

int d2fixed_buffered_n(double d, uint32_t precision, char* result);
void d2fixed_buffered(double d, uint32_t precision, char* result);
char* d2fixed(double d, uint32_t precision);

int d2exp_buffered_n(double d, uint32_t precision, char* result);
void d2exp_buffered(double d, uint32_t precision, char* result);
char* d2exp(double d, uint32_t precision);

} // namespace ryu

PRAGMA_WARNING_POP
