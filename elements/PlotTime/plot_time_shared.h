#ifndef PLOT_TIME_SHARED_H
#define PLOT_TIME_SHARED_H

#ifdef PLOT_TIME_STATIC
#   define PLOT_TIME_EXPORT
#else
#  if defined(PLOT_TIME_LIBRARY)
#    define PLOT_TIME_EXPORT Q_DECL_EXPORT
#  else
#    define PLOT_TIME_EXPORT Q_DECL_IMPORT
#  endif
#endif

#endif // PLOT_TIME_SHARED_H
