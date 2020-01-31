#pragma once

/*
|-RecordDecl 0x1b57f115d98 <temp.c:1:1, line:9:1> line:1:1 struct definition
A B         CD            E F         G H       I J       K
*/
std::regex eSourceRef(
    /*A  */ "[^\\w<]*"
    /*B 1*/ "([\\w<]+)"
    /*C  */ "\\s"
    /*D  */ "0x[\\da-f]{6,7}"
    /*E  */ "\\s<"
    /*F 2*/ "(col:\\d+|"
    /*   */ "line:\\d+:\\d+|"
    /*   */ "\\w+\\.\\w+:line:\\d+:\\d+)"
    /*G  */ ",\\s"
    /*H 3*/ "(col:\\d+|"
    /*   */ "line:\\d+:\\d+|"
    /*   */ "\\w+\\.\\w+:line:\\d+:\\d+)"
    /*I  */ ">\\s"
    /*J 4*/ "(col:\\d+|"
    /*   */ "line:\\d+:\\d+|"
    /*   */ "\\w+\\.\\w+:line:\\d+:\\d+)"
    /*K  */ ".*"
);
std::smatch smSourceRef;

std::regex eCatchGlobals(R"(([^\w<]*)([\w<]+).*)");
std::smatch smCatchGlobals;

std::regex eCatchGlobalName(R"###([^\w<]*[\w<]+\s0x[\da-f]{6,7}\s<[^>]*>\s(?:col:\d+|line:\d+:\d+)(?:\sused)?\s(\w+)\s'([^']+)')###");
std::smatch smCatchGlobalName;

/*
Extracts the source location with column only.
    1
col:1
A  BC
*/
std::regex eCol("col:(\\d+)");
std::smatch smCol;

/*
Extracts the source location with line and column only.
     1 2
line:1:1
A   BCDE
*/
std::regex eLineCol(
    /*A  */    "line:"
    /*B 1*/    "(\\d+)"
    /*C  */    ":"
    /*D 2*/    "(\\d+)"
);
std::smatch smLineCol;

/*
Extracts the source location complete with line source and column.
1--- 2 3 4 
temp.c:1:1
A   BCDEFG 
*/
std::regex eFileLineCol(
    /*A 1*/    "([\\w\\d]+)"
    /*B  */    "\\."
    /*C 2*/    "([\\w\\d]+)"
    /*D  */    ":"
    /*E 3*/    "(\\d+)"
    /*F  */    ":"
    /*G 4*/    "(\\d+)"
);
std::smatch smFileLineCol;

std::regex eIntegralType(R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s'([^']+)'\s(\d+))###");
std::smatch smIntegralType;

std::regex eVarDecl(R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s(?:col:\d+|line:\d+:\d+)(?:\sused)?\s(\w+)\s'([^']+)')###");
std::smatch smVarDecl;

std::regex eDeclRefExpr(R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s'([^']+)':?(?:'(?:[^']+)')?\slvalue\sVar\s0x[\da-f]{6,11}\s'(\w+)')###");
std::smatch smDeclRefExpr;

std::regex eImplicitCastExpr(R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s'([^']+)'\s<([^>]*)>)###");
std::smatch smImplicitCastExpr;

std::regex eBinaryOperator(R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s'([^']+)'\s'([^']+)')###");
std::smatch smBinaryOperator;

std::regex eUnaryOperator(R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s'([^']+)'(?:\slvalue)?\s(postfix|prefix)\s'([^']+)')###");
std::smatch smUnaryOperator;

std::regex eDeclStmt(R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>)###");
std::smatch smDeclStmt;

std::regex eArrayDecl(R"###((.*)\[(\d+)\])###");
std::smatch smArrayDecl;

std::regex ePointerDecl(R"###(.*\s\*)###");
std::smatch smPointerDecl;

std::regex eStructType(R"###(struct\s([\w\d]+).*)###");
std::smatch smStructType;

std::regex eStructDefinition(R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s(?:col:\d+|line:\d+:\d+)\sstruct\s(\w+)\sdefinition)###");
std::smatch smStructDefinition;

std::regex eFieldDeclaration(R"###([^\w<]*FieldDecl+\s0x[\da-f]{6,11}\s<[^>]*>\s(?:col:\d+|line:\d+:\d+)(\sreferenced)?\s(\w+)\s'([^']+)')###");
std::smatch smFieldDeclaration;

/*
    | |-MemberExpr 0x26114378700 <col:5, col:8> 'int':'int' lvalue .a 0x26114378368
A-------B---------CD------------EF-------------GH----IJ----KL-----MNOPQ------------
*/
std::regex eMemberExpr(
    "[^\\w<]*"          /* A Tree lines, connectors between lines inthe ast files           */ \
    "MemberExpr"        /* B Match excatly what you see                                     */ \
    "\\s"               /* C A space                                                        */ \
    "0x[\\da-f]{6,11}"  /* D The hex identifier of the ast node 0x123456                    */ \
    "\\s"               /* E A space                                                        */ \
    "<[^>]*>"           /* F Source location that is included in angular brackets <>        */ \
    "\\s"               /* G A space                                                        */ \
    "'([^']+)'"         /* H Type which this node return 'int'                              */ \
    ":?"                /* I A colon (for user types) 'mytype':'mytype'                     */ \
    "(?:'(?:[^']+)')?"  /* J The second 'mytype', see above                                 */ \
    "\\s"               /* K A space                                                        */ \
    "lvalue"            /* L matches exactly "lvalue"                                       */ \
    "\\s"               /* M A space                                                        */ \
    "(?:\\.|->)"        /* N mathes . or -> for members or pointers to members              */ \
    "([\\w\\d]+)"       /* O The member name which can have numbers after the first char    */ \
    "\\s"               /* P A space                                                        */ \
    "0x[\\da-f]{6,11}"  /* Q The hex identifier of the ast member node 0x123456             */ \
);
std::smatch smMemberExpr;
/*
Generic type regex
capturing groups:
1: union or struct or empty
2: basic type or type or struct type
3: indirection(pointer)
4: nested type
5: array size
*/
/*
struct abc *(abc)[5]
A     BC  DEFG  H IL
*/
std::regex eGenericType(
    "(union|struct|)?"
    "\\s?"
    "([\\w\\d]+|)?"
    "\\s?"
    "(\\**)"
    "("
    "(?:\\(.+\\))?)"
    "\\[?((?:\\d+)?)\\]?"
);
std::smatch smGenericType;

/*
                                                     1---------- 2  3--------
|-TypedefDecl 0x261143784f0 <line:8:1, col:19> col:19 referenced k 'struct st':'struct st'
A-B----------CD------------EF-----------------GH-----I----------JKLM----------
*/
std::regex eTypeDef(
    "[^\\w<]*"                     /* A Lines connecting the tree nodes in the ast file  */ \
    "TypedefDecl"                  /* B Matches what you see                             */ \
    "\\s"                          /* C A space                                          */ \
    "0x[\\da-f]{6,11}"             /* D The node hex identifier                          */ \
    "\\s"                          /* E A space                                          */ \
    "<[^>]*>"                      /* F Source file location                             */ \
    "\\s"                          /* G A space                                          */ \
    "(?:col:\\d+|line:\\d+:\\d+)"  /* H Source file exact location                       */ \
    /*1*/  "(\\sreferenced|)?"     /* I Might match what you see                         */ \
    "\\s"                          /* J A space                                          */ \
    /*2*/  "(\\w+)"                /* K The typedef name                                 */ \
    "\\s"                          /* L A space                                          */ \
    /*3*/  "'([^']+)'"             /* M The referenced type                              */ \
);
std::smatch smTypeDef;


