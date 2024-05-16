#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(LOGGINGWRAPPER_LIB)
#  define LOGGINGWRAPPER_EXPORT Q_DECL_EXPORT
# else
#  define LOGGINGWRAPPER_EXPORT Q_DECL_IMPORT
# endif
#else
# define LOGGINGWRAPPER_EXPORT
#endif
