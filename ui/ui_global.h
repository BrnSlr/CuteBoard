#ifndef UI_GLOBAL_H
#define UI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(BOARD_UI_LIBRARY)
#  define BOARD_UI_EXPORT Q_DECL_EXPORT
#else
#  define BOARD_UI_EXPORT Q_DECL_IMPORT
#endif

#endif // UI_GLOBAL_H
