#ifndef BUILTIN_EXPS_H
#define BUILTIN_EXPS_H

#include <QStringList>

namespace BuiltIn_Expressions {
    static QStringList modules;
    static int modules_count;
    static QStringList functions({"int_of_string","string_of_int", "float_of_int", "int_of_float"});
    static int functions_count = functions.length();
    static QStringList words;
    static int words_count;
}

#endif // BUILTIN_EXPS_H
