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
    "(col:(\\d+))|(line:(\\d+):(\\d+))|(([\\w\\d\\\\/:\\.]+\\.\\w+):(\\d+):(\\d+))"
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
    /*   */ "[\\w\\d\\\\/:\\.]*\\.\\w[\\w\\d]*:\\d+:\\d+)"
    /*G  */ "(?:,\\s)?"
    /*H 3*/ "(col:\\d+|"
    /*   */ "line:\\d+:\\d+|"
    /*   */ "[\\w\\d\\\\/:\\.]*\\.\\w[\\w\\d]*:\\d+:\\d+)?"
    /*I  */ ">\\s"
    /*J 4*/ "(col:\\d+|"
    /*   */ "line:\\d+:\\d+|"
    /*   */ "[\\w\\d\\\\/:\\.]*\\.\\w[\\w\\d]*:\\d+:\\d+|)?"
    /*K  */ ".*"
);

std::regex eCatchGlobals(
    "([^\\w<]*)([\\w<]+).*"
);

std::regex eCatchGlobalName(
    R"###([^\w<]*[\w<]+\s0x[\da-f]{6,7}\s<[^>]*>\s(?:col:\d+|line:\d+:\d+)(?:\sused)?\s(\w+)\s'([^']+)')###");

std::regex eIntegralType(
    R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s'([^']+)'\s(\d+))###");

std::regex eVarDecl(
    R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s(?:col:\d+|line:\d+:\d+)(?:\sused)?\s(\w+)\s'([^']+)')###");

/* Matches something as
|-ParmVarDecl 0x2e6392983e0 <col:8, col:12> col:12 used a 'int'
Captures:
1 name (optional)
2 type
*/
std::regex eParmVarDecl(
    R"###([^\w]*ParmVarDecl\s0x[\da-f]{6,11}\s<[^>]*>\s(?:col:\d+|line:\d+:\d+)(?:\sused)?(?:\s([\w\d]+))?\s'([^']+)')###");

/* Matches something as
|-FunctionDecl 0x1b5f74af000 <line:27:1, line:30:1> line:27:7 used f6 'int (*(long))(long)'
Captures:
1 name 
2 type
*/
std::regex eFunctionDecl(
    R"###([^\w]*FunctionDecl\s0x[\da-f]{6,11}(?:\sprev\s0x[\da-f]{6,11})?\s<[^>]*>\s(?:col:\d+|line:\d+:\d+)(?:\sused)?(?:\s([\w\d]+))?\s'([^']+)')###");

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

/*
    | `-DeclRefExpr 0x269eb41f840 <col:12> 'int' EnumConstant 0x269eb381c80 'IDLE' 'int'
*/
std::regex eDeclRefExprEnum(
    "[^\\w<]*"             /*    | |   `-                                                                     */
    "[\\w<]+"              /*DeclRefExpr                                                                      */
    "\\s"                  /*                                                                                 */
    "0x[\\da-f]{6,11}"     /*0x23a31f28510                                                                    */
    "\\s"                  /*                                                                                 */
    "<[^>]*>"              /*<col:3>                                                                          */
    "\\s"                  /*                                                                                 */
    "'([^']+)'"            /*'int'              1                                                             */
    "\\sEnumConstant"      /* EnumConstant                                                                    */
    "\\s"                  /*                                                                                 */
    "(0x[\\da-f]{6,11})"   /*0x269eb381c80      2                                                             */
    "\\s"                  /*                                                                                 */
    "'([\\w\\d]+)'"        /*'IDLE'             3                                                             */
    "\\s"                  /*                                                                                 */
    "'([\\w\\d]+)'"        /*'int'              4                                                             */
);

/*
DeclRefExpr of a Function !!!
|   `-DeclRefExpr 0x1b5f74af608 <col:12> 'int (*(long))(long)' Function 0x1b5f74af000 'f6' 'int (*(long))(long)'

*/
std::regex eDeclRefExprFunction(
    "[^\\w<]*"             /*    | |   `-                                                                     */
    "[\\w<]+"              /*DeclRefExpr                                                                      */
    "\\s"                  /*                                                                                 */
    "0x[\\da-f]{6,11}"     /*0x23a31f28510                                                                    */
    "\\s"                  /*                                                                                 */
    "<[^>]*>"              /*<col:3>                                                                          */
    "\\s"                  /*                                                                                 */
    "'([^']+)'"            /*'int'              1                                                             */
    "\\sFunction"          /* Function                                                                        */
    "\\s"                  /*                                                                                 */
    "(0x[\\da-f]{6,11})"   /*0x269eb381c80      2                                                             */
    "\\s"                  /*                                                                                 */
    "'([\\w\\d]+)'"        /*'f6'               3                                                             */
    "\\s"                  /*                                                                                 */
    "'([^']+)'"            /*'int'              4                                                             */
);


/*
    |   `-DeclRefExpr 0x24f1aec4030 <col:12> 'int' lvalue ParmVar 0x24f1aec3e08 'a' 'int'
*/
std::regex eDeclRefExprParmVar(
    R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s'([^']+)'(?::'[^']+')\slvalue\sParmVar\s(0x[\da-f]{6,11})\s'([\w\d]+)'\s'([\w\d]+)')###"
);

std::regex eImplicitCastExpr(
    R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s'([^']+)'(?::'[^']+')?\s<([^>]*)>)###");

std::regex eBinaryOperator(
    R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s'([^']+)'(?::'[^']+')?\s'([^']+)')###");


/*
| | |   `-CompoundAssignOperator 0x5033c18 <col:37, Sources/MCN1_acmotor.c:350:22> 'C_D_ISR_Status_t':'unsigned char' '|=' ComputeLHSTy='int' ComputeResultTy='int'
Captures:
1. Type
2. Compound operator
*/
std::regex eCompoundAssignOperator(
    R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s'([^']+)'(?::'[^']+')?\s'([^']+)')###"
);

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
    "\\(anonymous\\sunion\\sat\\s"  /*(anonymous union at */
    "([^\\)]+)"                     /*temp.c:4:5*/ /*Source point is always complete with file, line and col info*/
    "\\)"                           /*)*/
);

std::regex eStructTypeAnonymous2(
    "struct\\s\\(anonymous\\sat\\s" /*struct (anonymous at */
    "([^\\)]+)"                     /*temp.c:4:5*/ /*Source point is always complete with file, line and col info*/
    "\\)"                           /*)*/
);

std::regex eUnionTypeAnonymous2(
    "union\\s\\(anonymous\\sat\\s"  /*union (anonymous at */
    "([^\\)]+)"                     /*temp.c:4:5*/ /*Source point is always complete with file, line and col info*/
    "\\)"                           /*)*/
);

std::regex eStructDefinition(
    R"###([^\w<]*[\w<]+\s(0x[\da-f]{6,11})\s<[^>]*>\s(?:col:\d+|line:\d+:\d+|[\\w\\d\\\\/:]+\\.\\w+:\\d+:\\d+)\s(struct|union)\s?(\w+)?\sdefinition)###");

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
    "(?:\\s(\\w+))?"                 /* Filed name       */
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
    | |-MemberExpr 0x26114378700 <col:5, col:8> 'int':'int' lvalue bitfield .a 0x26114378368
A-------B---------CD------------EF-------------GH----IJ----KL-----MN--------OPQR------------
*/
std::regex eMemberExpr(
    "[^\\w<]*"          /* A   Tree lines, connectors between lines inthe ast files           */
    "MemberExpr"        /* B   Match excatly what you see                                     */
    "\\s"               /* C   A space                                                        */
    "0x[\\da-f]{6,11}"  /* D   The hex identifier of the ast node 0x123456                    */
    "\\s"               /* E   A space                                                        */
    "<[^>]*>"           /* F   Source location that is included in angular brackets <>        */
    "\\s"               /* G   A space                                                        */
    "'([^']+)'"         /* H 1 Type which this node return 'int'                              */
    ":?"                /* I   A colon (for user types) 'mytype':'mytype'                     */
    "(?:'(?:[^']+)')?"  /* J   The second 'mytype', see above                                 */
    "\\s"               /* K   A space                                                        */
    "lvalue"            /* L   matches exactly "lvalue"                                       */
    "\\s"               /* M   A space                                                        */
    "(bitfield\\s)?"    /* N 2 bitfield                                                       */
    "(?:\\.|->)"        /* O   matches . or -> for members or pointers to members             */
    "([\\w\\d]+)"       /* P 3 The member name which can have numbers after the first char    */
    "\\s"               /* Q   A space                                                        */
    "0x[\\da-f]{6,11}"  /* R   The hex identifier of the ast member node 0x123456             */
);

/*
EnumDecl Syntax
|-EnumDecl 0x1f18b49f398 <line:15:1, line:19:1> line:15:6 e1
The enum struct can be anonymous: in this case the hexID must be tracked.
|-EnumDecl 0x1f18b49f398 <line:15:1, line:19:1> line:15:6
Can be followed by a typedef

*/
std::regex eEnumDecl(
    R"###([^\w]*EnumDecl\s(0x[\da-f]{6,11})\s<[^>]*>\s(?:col:\d+|line:\d+:\d+|[\w\d\\\/\.:]+\.\w+:\d+:\d+)(?:\s(\w+))?)###");

/*
EnumConstantDecl syntax
referenced is optional
| |-EnumConstantDecl 0x1f18b3127b0 <line:6:5> col:5 referenced IDLE 'int'
*/
std::regex eEnumConstantDecl(
    R"###([^\w]*EnumConstantDecl\s(0x[\da-f]{6,11})\s<[^>]*>\s(?:col:\d+|line:\d+:\d+|[\w\d\\\/\.:]+\.\w+:\d+:\d+)(?:\sreferenced)?\s(\w+)\s'int')###");



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
);



/*
| | |   |-CStyleCastExpr 0x4f6a620 <./Sources/MCN1_RegPI.h:32:44, col:237> 'u16':'unsigned short' <FloatingToIntegral>
Captures: 
1. First type
2. Second type decoded optional
3. Type of cast
*/
std::regex eCStyleCastExprt(
    R"###([^\w]*CStyleCastExpr\s0x[\da-f]{6,11}\s<[^>]*>\s'([^']+)'(?::'([^']+)'|)\s<([^>]+)>)###");
