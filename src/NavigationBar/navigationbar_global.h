#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(NAVIGATIONBAR_LIB)
#  define NAVIGATIONBAR_EXPORT Q_DECL_EXPORT
# else
#  define NAVIGATIONBAR_EXPORT Q_DECL_IMPORT
# endif
#else
# define NAVIGATIONBAR_EXPORT
#endif
