#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(MLCONFIG_LIB)
#  define MLCONFIG_EXPORT Q_DECL_EXPORT
# else
#  define MLCONFIG_EXPORT Q_DECL_IMPORT
# endif
#else
# define MLCONFIG_EXPORT
#endif
