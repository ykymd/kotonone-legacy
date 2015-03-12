#pragma once
#include <string>

#ifdef UNICODE
#define tstring ::std::wstring
#define tvsnprintf  vswprintf
#else
#define tstring ::std::string
#define tvsnprintf  vsnprintf
#endif