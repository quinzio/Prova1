#pragma once
std::string strCol("col:(\\d+)");
/*
Extracts the source location with column only.
col:1
A  col
B  :
C1 1
*/
std::regex eCol(strCol);

/*
Extracts the source location with line and column only.
line:1:1
*/
std::regex eLineCol(
    /*A  */    "line:"
    /*B 1*/    "(\\d+)"
    /*C  */    ":"
    /*D 2*/    "(\\d+)"
);

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

/*
Matches the following lines
temp.c:1:1
line:1:2
col:1

Captures:
1 Col
2 Col#
3 Line:Col
4 Line#
5 Col#
6 File:Line:Col
7 File#
8 Line#
9 Col#
*/
std::regex eSourcePoint(
    "(col:(\\d+))|(line:(\\d+):(\\d+))|((\\w+\\.\\w+):(\\d+):(\\d+))"
);

/*
|-RecordDecl 0x1b57f115d98 <temp.c:1:1, line:9:1> line:1:1 struct definition
A B         CD            E F         G H       I J       K
*/
std::regex eSourceRef(
    /*A  */ "[^\\w<]*"
    /*B 1*/ "([\\w<]+)"
    /*C  */ "\\s"
    /*D  */ "0x[\\da-f]{6,11}"
    /*E  */ "\\s<"
    /*F 2*/ "(col:\\d+|"
    /*   */ "line:\\d+:\\d+|"
    /*   */ "\\w+\\.\\w+:\\d+:\\d+)"
    /*G  */ "(?:,\\s)?"
    /*H 3*/ "(col:\\d+|"
    /*   */ "line:\\d+:\\d+|"
    /*   */ "\\w+\\.\\w+:\\d+:\\d+)?"
    /*I  */ ">\\s"
    /*J 4*/ "(col:\\d+|"
    /*   */ "line:\\d+:\\d+|"
    /*   */ "\\w+\\.\\w+:\\d+:\\d+|0)?"
    /*K  */ ".*"
);

std::regex eCatchGlobals(
    R"(([^\w<]*)([\w<]+).*)");

std::regex eCatchGlobalName(
    R"###([^\w<]*[\w<]+\s0x[\da-f]{6,7}\s<[^>]*>\s(?:col:\d+|line:\d+:\d+)(?:\sused)?\s(\w+)\s'([^']+)')###");

std::regex eIntegralType(
    R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s'([^']+)'\s(\d+))###");

std::regex eVarDecl(
    R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s(?:col:\d+|line:\d+:\d+)(?:\sused)?\s(\w+)\s'([^']+)')###");

/*
_____________________________________________________1---------- 2__3--------___4--------
|-TypedefDecl 0x261143784f0 <line:8:1, col:19> col:19 referenced k 'struct st':'struct st'
A-B----------CD------------EF-----------------GH-----I----------JKLM----------N-----------
*/
std::regex eTypeDef(
    /* */ "[^\\w<]*"                     /* A Lines connecting the tree nodes in the ast file  */
    /* */ "TypedefDecl"                  /* B Matches what you see                             */
    /* */ "\\s"                          /* C A space                                          */
    /* */ "0x[\\da-f]{6,11}"             /* D The node hex identifier                          */
    /* */ "\\s"                          /* E A space                                          */
    /* */ "<[^>]*>"                      /* F Source file location                             */
    /* */ "\\s"                          /* G A space                                          */
    /* */ "(?:col:\\d+|line:\\d+:\\d+)"  /* H Source file exact location                       */
    /*1*/ "(?:\\s(referenced|))?"        /* I Might match what you see                         */
    /* */ "\\s"                          /* J A space                                          */
    /*2*/ "(\\w+)"                       /* K The typedef name                                 */
    /* */ "\\s"                          /* L A space                                          */
    /*3*/ "'([^']+)'"                    /* M The referenced type                              */
    /*4*/ "(?::'([^']+)')?"              /* N Unsugared type                                   */
);

/*
    | |   `-DeclRefExpr 0x23a31f28510 <col:3> 'struct (anonymous struct at temp.c:3:1)':'struct (anonymous at temp.c:3:1)' lvalue Var 0x23a31f28358 'vb' 'struct (anonymous struct at temp.c:3:1)':'struct (anonymous at temp.c:3:1)'
*/
std::regex eDeclRefExpr(
    "[^\\w<]*"             /*    | |   `-                                                                     */
    "[\\w<]+"              /*DeclRefExpr                                                                      */
    "\\s"                  /*                                                                                 */
    "0x[\\da-f]{6,11}"     /*0x23a31f28510                                                                    */
    "\\s"                  /*                                                                                 */
    "<[^>]*>"              /*<col:3>                                                                          */
    "\\s"                  /*                                                                                 */
    "'([^']+)'"            /*'struct (anonymous struct at temp.c:3:1)'                                        */
    ":?"                   /*:                                                                                */
    "(?:'(?:[^']+)')?"     /*'struct (anonymous at temp.c:3:1)'                                               */
    "\\s"                  /*                                                                                 */
    "lvalue"               /*lvalue                                                                           */
    "\\s"                  /*                                                                                 */
    "Var"                  /*Var                                                                              */
    "\\s"                  /*                                                                                 */
    "0x[\\da-f]{6,11}"     /*0x23a31f28358                                                                    */
    "\\s"                  /*                                                                                 */
    "'([\\w\\d]+)'"        /*'vb'                                                                             */
);                         /* 'struct (anonymous struct at temp.c:3:1)':'struct (anonymous at temp.c:3:1)'    */

std::regex eImplicitCastExpr(
    R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s'([^']+)'\s<([^>]*)>)###");

std::regex eBinaryOperator(
    R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s'([^']+)'\s'([^']+)')###");

std::regex eUnaryOperator(
    R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s'([^']+)'(?:\slvalue)?\s(postfix|prefix)\s'([^']+)')###");

std::regex eDeclStmt(
    R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>)###");

std::regex eArrayDecl(
    R"###((.*)\[(\d+)\])###");

std::regex ePointerDecl(
    R"###(.*\s\*)###");

std::regex eStructType(
    R"###(struct\s([\w\d]+).*)###");

std::regex eStructTypeAnonymous(
    "\\(anonymous\\sstruct\\sat\\s" /*(anonymous struct at */
    "([^\\)]+)"                     /*temp.c:4:5*/ /*Source point is always complete with file, line and col info*/
    "\\)"                           /*)*/
);

std::regex eUnionTypeAnonymous(
    "\\(anonymous\\sunion\\sat\\s"  /*(anonymous struct at */
    "([^\\)]+)"                     /*temp.c:4:5*/ /*Source point is always complete with file, line and col info*/
    "\\)"                           /*)*/
);

std::regex eStructTypeAnonymous2(
    "struct\\s\\(anonymous\\sat\\s"   /*struct (anonymous struct at */
    "([^\\)]+)"                         /*temp.c:4:5*/ /*Source point is always complete with file, line and col info*/
    "\\)"                               /*)*/
);

std::regex eStructDefinition(
    R"###([^\w<]*[\w<]+\s(0x[\da-f]{6,11})\s<[^>]*>\s(?:col:\d+|line:\d+:\d+)\sstruct\s?(\w+)?\sdefinition)###");

/*
| `-FieldDecl 0x247143d8818 <line:12:5, line:15:6> col:6 fx2 'struct x2':'struct x2'
| `-FieldDecl 0x23a31f28250 <line:4:5, line:7:6> col:6 referenced va 'struct (anonymous struct at temp.c:4:5)':'struct (anonymous at temp.c:4:5)'
*/
std::regex eFieldDeclaration(
    "[^\\w<]*"                       /*| |-              */
    "FieldDecl"                      /*FieldDecl         */
    "\\s"                            /*                  */
    "0x[\\da-f]{6,11}"               /*0x247143d8308     */
    "\\s"                            /*                  */
    "<[^>]*>"                        /*<line:6:5>        */
    "\\s"                            /*                  */
    "(?:col:\\d+|line:\\d+:\\d+)"    /*col:5             */
    "(\\sreferenced)?"               /* referenced       */
    "\\s"                            /*                  */
    "(\\w+)"                         /*                  */
    "\\s"                            /*                  */
    "'([^']+)'"                      /*'struct (anonymous at temp.c:6:5)'                  */
);

std::regex eFieldDeclarationImplicit(
    /*
    | |-FieldDecl 0x247143d8308 <line:6:5> col:5 implicit referenced 'struct (anonymous at temp.c:6:5)'

    */
    "[^\\w<]*"                       /*| |-                              */
    "FieldDecl+"                     /*FieldDecl                         */
    "\\s"                            /*                                  */
    "0x[\\da-f]{6,11}"               /*0x247143d8308                     */
    "\\s"                            /*                                  */
    "<[^>]*>"                        /*<line:6:5>                        */
    "\\s"                            /*                                  */
    "(?:col:\\d+|line:\\d+:\\d+)"    /*col:5                             */
    "\\simplicit"                    /* implicit                         */
    "(\\sreferenced)?"               /* referenced                       */ /*capt 1*/
    "\\s"                            /*                                  */
    "'([^']+)'"                      /*'struct (anonymous at temp.c:6:5)'*/ /*capt 2*/
);


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
    "(union|struct|)?"      /*struct OR union OR anonymous struct*/
    "\\s?"                                  /* */
    "([\\w\\d]+|\\([^\\)]+\\))?"            /*abc OR (anonymous struct at temp.c:3:1)*/
    "\\s?"                                  /* */
    "(\\**)"                                /***/ //an asterix *
    "("                                     /**/  // open capturing group
    "(?:\\(.+\\))?"                         /*abc*/
    ")"                                     /**/  // close capturing group
    "\\[?((?:\\d+)?)\\]?"                   /*[5]*/
);

/*
Matches the C built-in types
*/
std::regex eBuiltinTypes(
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
);


