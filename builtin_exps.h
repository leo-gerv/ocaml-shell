#ifndef BUILTIN_EXPS_H
#define BUILTIN_EXPS_H

#include <QStringList>

namespace BuiltIn_Expressions {
    static QStringList modules({});
    static int modules_count = modules.length();
    static QStringList functions({"int_of_string","string_of_int", "float_of_int", "int_of_float","string_of_float","float_of_string"
                                 ,"open","#load","#use","let",
                                  "Unix,Unix.sleep","\"unix.cma\"",
                                  "Random","Random.init","Random.int"});
    static int functions_count = functions.length();
    static QStringList words({});
    static int words_count = words.length();
}

#endif // BUILTIN_EXPS_H
