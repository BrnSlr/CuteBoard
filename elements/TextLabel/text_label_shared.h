#ifndef TEXT_LABEL_SHARED_H
#define TEXT_LABEL_SHARED_H
#include <QtGlobal>

#ifdef TEXTLABEL_STATIC
#   define TEXTLABEL_EXPORT
#else
#  if defined(TEXTLABEL_LIBRARY)
#    define TEXTLABEL_EXPORT Q_DECL_EXPORT
#  else
#    define TEXTLABEL_EXPORT Q_DECL_IMPORT
#  endif
#endif

#endif // TEXT_LABEL_SHARED_H
