#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(MLBUSINESSMANAGE_LIB)
#  define MLBUSINESSMANAGE_EXPORT Q_DECL_EXPORT
# else
#  define MLBUSINESSMANAGE_EXPORT Q_DECL_IMPORT
# endif
#else
# define MLBUSINESSMANAGE_EXPORT
#endif
