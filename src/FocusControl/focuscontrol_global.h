#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(FOCUSCONTROL_LIB)
#  define FOCUSCONTROL_EXPORT Q_DECL_EXPORT
# else
#  define FOCUSCONTROL_EXPORT Q_DECL_IMPORT
# endif
#else
# define FOCUSCONTROL_EXPORT
#endif
