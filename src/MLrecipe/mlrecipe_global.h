#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(MLRECIPE_LIB)
#  define MLRECIPE_EXPORT Q_DECL_EXPORT
# else
#  define MLRECIPE_EXPORT Q_DECL_IMPORT
# endif
#else
# define MLRECIPE_EXPORT
#endif
