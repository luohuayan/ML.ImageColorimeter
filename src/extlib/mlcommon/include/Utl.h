// Copyright 2023 MLOptic

#ifndef LOGIC_CAMERA_UTL_H_
#define LOGIC_CAMERA_UTL_H_

#pragma region Macros and defines
// Export functions through shared library
#if defined(_WIN32)
#  if defined(EXPORTING_MLCAMERA)
#    define DECLSPEC __declspec(dllexport)
#  else
#    define DECLSPEC __declspec(dllimport)
#  endif
#else  // non windows
#  define DECLSPEC
#endif
#pragma endregion

#endif  // LOGIC_CAMERA_UTL_H_
