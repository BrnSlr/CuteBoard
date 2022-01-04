#ifndef PLOT_XY_SHARED_H
#define PLOT_XY_SHARED_H

#if defined(PLOT_XY_LIBRARY)
#  define PLOT_XY_EXPORT Q_DECL_EXPORT
#else
#  define PLOT_XY_EXPORT Q_DECL_IMPORT
#endif

#endif // PLOT_XY_SHARED_H
