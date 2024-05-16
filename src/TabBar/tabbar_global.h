#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(TABBAR_LIB)
#  define TABBAR_EXPORT Q_DECL_EXPORT
# else
#  define TABBAR_EXPORT Q_DECL_IMPORT
# endif
#else
# define TABBAR_EXPORT
#endif
