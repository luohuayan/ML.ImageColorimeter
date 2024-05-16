#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(CAMERACONTROL_LIB)
#  define CAMERACONTROL_EXPORT Q_DECL_EXPORT
# else
#  define CAMERACONTROL_EXPORT Q_DECL_IMPORT
# endif
#else
# define CAMERACONTROL_EXPORT
#endif
