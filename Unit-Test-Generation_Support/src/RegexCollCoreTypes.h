#pragma once

/*
Match the basic c types
*/
std::regex eFinalType(
    "("
    "struct\\s[\\w\\d\\.:\\\\/]+"  "|"
    "union\\s[\\w\\d\\.:\\\\/]+"   "|"
    "struct\\s\\([^\\)]+\\)"       "|"
    "void"                         "|"
    "unsigned\\sshort\\sint"       "|"
    "unsigned\\sshort"             "|"
    "unsigned\\slong\\slong\\sint" "|"
    "unsigned\\slong\\slong"       "|"
    "unsigned\\slong\\sint"        "|"
    "unsigned\\slong"              "|"
    "unsigned\\sint"               "|"
    "unsigned\\schar"              "|"
    "unsigned"                     "|"
    "signed\\sshort\\sint"         "|"
    "signed\\sshort"               "|"
    "signed\\slong\\slong\\sint"   "|"
    "signed\\slong\\slong"         "|"
    "signed\\slong\\sint"          "|"
    "signed\\slong"                "|"
    "signed\\sint"                 "|"
    "signed\\schar"                "|"
    "signed"                       "|"
    "short\\sint"                  "|"
    "short"                        "|"
    "long\\slong\\sint"            "|"
    "long\\slong"                  "|"
    "long\\sint"                   "|"
    "long\\sdouble"                "|"
    "long"                         "|"
    "int"                          "|"
    "float"                        "|"
    "double"                       "|"
    "char"                         "|"    
    "[\\w\\d\\.:\\\\/]+"  // A typedef, can be anonymous like temp.c:2:3
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


