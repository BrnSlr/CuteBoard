#include "string_util.h"

QString removeSpecialChars(QString s) {
    s.replace("â", "a");
    s.replace("ä", "a");
    s.replace("á", "a");
    s.replace("à", "a");
    s.replace("ã", "a");
    s.replace("å", "a"); //a
    s.replace("ç", "c"); //c
    s.replace("ê", "e");
    s.replace("ë", "e");
    s.replace("é", "e");
    s.replace("è", "e"); //e
    s.replace("î", "i");
    s.replace("ï", "i");
    s.replace("í", "i");
    s.replace("ì", "i"); //i
    s.replace("ñ", "n"); //n
    s.replace("ô", "o");
    s.replace("ö", "o");
    s.replace("ó", "o");
    s.replace("ò", "o");
    s.replace("õ", "o");
    s.replace("°", "o");
    s.replace("ø", "o"); //o
    s.replace("ß", "ss"); //ß -> ss
    s.replace("û", "u");
    s.replace("ü", "u");
    s.replace("ú", "u");
    s.replace("ù", "u"); //u
    s.replace("æ", "ae");
    s.replace( QRegExp( "[" + QRegExp::escape( "\\/:*?\"<>| " ) + "]" ), QString( "_" ) );
    return s;
}
