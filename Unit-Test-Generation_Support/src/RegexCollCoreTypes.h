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
    "signed\\schar"                "|"
    "unsigned\\schar"              "|"
    "short"                        "|"
    "short\\sint"                  "|"
    "signed\\sshort"               "|"
    "signed\\sshort\\sint"         "|"
    "unsigned\\sshort"             "|"
    "unsigned\\sshort\\sint"       "|"
    "int"                          "|"
    "signed"                       "|"
    "signed\\sint"                 "|"
    "unsigned"                     "|"
    "unsigned\\sint"               "|"
    "long"                         "|"
    "long\\sint"                   "|"
    "signed\\slong"                "|"
    "signed\\slong\\sint"          "|"
    "unsigned\\slong"              "|"
    "unsigned\\slong\\sint"        "|"
    "long\\slong"                  "|"
    "long\\slong\\sint"            "|"
    "signed\\slong\\slong"         "|"
    "signed\\slong\\slong\\sint"   "|"
    "unsigned\\slong\\slong"       "|"
    "unsigned\\slong\\slong\\sint" "|"
    "float"                        "|"
    "double"                       "|"
    "long\\sdouble"
    ")"
    "(.*)"
);


/*
unsigned int ***
*/
std::regex eRoundBrack(
    "(\\*)"                 /*Captures first  asterix*/
);

std::regex eRightBrack(
    "(\\[\\d+\\])"          /*Captures first  [1]*/
);


