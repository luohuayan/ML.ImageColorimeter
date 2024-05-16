#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(FILTERWHEEL_LIB)
#  define FILTERWHEEL_EXPORT Q_DECL_EXPORT
# else
#  define FILTERWHEEL_EXPORT Q_DECL_IMPORT
# endif
#else
# define FILTERWHEEL_EXPORT
#endif
