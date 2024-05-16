#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(PLUGINCORE_LIB)
#  define PLUGINCORE_EXPORT Q_DECL_EXPORT
# else
#  define PLUGINCORE_EXPORT Q_DECL_IMPORT
# endif
#else
# define PLUGINCORE_EXPORT
#endif
