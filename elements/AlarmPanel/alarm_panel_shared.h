#ifndef ALARM_PANEL_SHARED_H
#define ALARM_PANEL_SHARED_H
#include <QtGlobal>

#ifdef ALARMPANEL_STATIC
#   define ALARMPANEL_EXPORT
#else
#  if defined(ALARMPANEL_LIBRARY)
#    define ALARMPANEL_EXPORT Q_DECL_EXPORT
#  else
#    define ALARMPANEL_EXPORT Q_DECL_IMPORT
#  endif
#endif

#endif // ALARM_PANEL_SHARED_H
