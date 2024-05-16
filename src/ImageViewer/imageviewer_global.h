#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(IMAGEVIEWER_LIB)
#  define IMAGEVIEWER_EXPORT Q_DECL_EXPORT
# else
#  define IMAGEVIEWER_EXPORT Q_DECL_IMPORT
# endif
#else
# define IMAGEVIEWER_EXPORT
#endif
