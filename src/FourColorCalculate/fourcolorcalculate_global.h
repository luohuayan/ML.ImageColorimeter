#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(FOURCOLORCALCULATE_LIB)
#  define FOURCOLORCALCULATE_EXPORT Q_DECL_EXPORT
# else
#  define FOURCOLORCALCULATE_EXPORT Q_DECL_IMPORT
# endif
#else
# define FOURCOLORCALCULATE_EXPORT
#endif
