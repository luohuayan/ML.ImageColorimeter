// Copyright 2023 MLOptic

#ifndef LOGIC_MOTION_MLFILTERWHEEL_MLFILTERWHEELCALLBACK_H_
#define LOGIC_MOTION_MLFILTERWHEEL_MLFILTERWHEELCALLBACK_H_

#ifdef EXPORTING_MLFILTERWHEEL
#define MLFILTERWHEEL_API __declspec(dllexport)
#else
#define MLFILTERWHEEL_API __declspec(dllimport)
#endif

#include <memory>
#include <string>

#include "MLFilterWheelClass.h"

/// <summary>
///  This file contains the declaration of the callback class
///  MLFilterWheelCallback.
/// The class provides a common interface for callback function for FilterWheel
/// control.
/// </summary>

namespace ML {
namespace MLFilterWheel {
class MLFILTERWHEEL_API MLFilterWheelCallback {
 public:
    virtual void NotifyFilterStatusChanged(
        const std::string object, ML::MLFilterWheel::MLFilterStatus status) = 0;
};
}  // namespace MLFilterWheel
}  // namespace ML

#endif  // LOGIC_MOTION_MLFILTERWHEEL_MLFILTERWHEELCALLBACK_H_
