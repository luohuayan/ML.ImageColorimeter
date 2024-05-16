#pragma once

#include "QtCore/qglobal.h"

#ifndef BUILD_STATIC
# if defined(MENUBAR_LIB)
#  define MENUBAR_EXPORT Q_DECL_EXPORT
# else
#  define MENUBAR_EXPORT Q_DECL_IMPORT
# endif
#else
# define MENUBAR_EXPORT
#endif
