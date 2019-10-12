#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(XEROPATHCOMMON_LIB)
#  define XEROPATHCOMMON_EXPORT Q_DECL_EXPORT
# else
#  define XEROPATHCOMMON_EXPORT Q_DECL_IMPORT
# endif
#else
# define XEROPATHCOMMON_EXPORT
#endif
