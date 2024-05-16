#pragma once
#include <ctime>
#include <string>
#include <opencv2/opencv.hpp>
// Export functions through shared library
#if defined(_WIN32)
#  if defined(EXPORTING_ALGORITHMLIBRARY)
#    define DECLSPEC __declspec(dllexport)
#  else
#    define DECLSPEC __declspec(dllimport)
#  endif
#else // non windows
#  define DECLSPEC
#endif
DECLSPEC std::string openCVMatTypeToString(int type);

