/*
Copyright(c) MLOptic Corp.All rights reserved.
*/

#ifndef ML_FILESYSTEM_H_
#define ML_FILESYSTEM_H_
#pragma once

#if __cplusplus >= 201703L //c++17
#include <filesystem>
namespace filesystem = std::filesystem;
#elif __cplusplus >= 201402L //c++14
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>
namespace filesystem = std::experimental::filesystem;
#else
#error "Cannot use std::filesystem or std::experimental::filesystem"
#endif //__cplusplus

#ifdef _WIN32
const std::string os_pathsep("\\");
#else
const std::string os_pathsep("/");
#endif  //_WIN32

#endif  //ML_FILESYSTEM_H_