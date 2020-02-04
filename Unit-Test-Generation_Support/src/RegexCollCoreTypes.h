#pragma once

/*
Match the basic c types
*/
std::regex eFinalType(
    "("
    "struct\\s[\\w\\d]+"           "|"
    "union\\s[\\w\\d]+"            "|"
    "struct\\s\\([^\\)]+\\)"       "|"
    "void"                         "|"
    "char"                         "|"
    "char16_t"                     "|"
    "char32_t"                     "|"
    "wchar_t"                      "|"
    "signed char"                  "|"
    "signed short int"             "|"
    "signed int"                   "|"
    "signed long int"              "|"
    "signed long long int"         "|"
    "unsigned char"                "|"
    "unsigned short int"           "|"
    "unsigned int"                 "|"
    "unsigned long int"            "|"
    "unsigned long long int"       "|"
    "float"                        "|"
    "double"                       "|"
    "long double" 
    ")"
    "(.*)"
);


