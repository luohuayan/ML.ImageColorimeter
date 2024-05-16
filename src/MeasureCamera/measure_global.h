#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(MEASURECAMERA_LIB)
#  define MEASURE_EXPORT Q_DECL_EXPORT
# else
#  define MEASURE_EXPORT Q_DECL_IMPORT
# endif
#else
# define MEASURE_EXPORT
#endif
