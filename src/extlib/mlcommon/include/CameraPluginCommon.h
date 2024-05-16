// Copyright 2023 MLOptic
#pragma once
#include <string>
#include <vector>
#include <map>

namespace ML {
namespace CameraV2 {

struct CameraConfigInfo {
    // matroxv2, vimba, ximea...
    std::string type;
    std::string sn;
};

}  // namespace CameraV2
}  // namespace ML
