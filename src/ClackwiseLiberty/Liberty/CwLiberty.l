/*

Copyright (c) 2008, 2009 Roopesh Chander <roop@forwardbias.in>

This file is part of Clackwise. <http://clackwise.googlecode.com>

Clackwise is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 of the License, or
(at your option) any later version.

Clackwise is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
version 2.1 along with Clackwise.  If not, see <http://www.gnu.org/licenses/>.

*/

%{
#include <QDebug>
#include <QVariant>

#define YYSTYPE QVariant
#include "CwLiberty_yacc.h"

extern "C" int CwLibertywrap() { return 1; }

%}
%option yylineno
%%
\/\*([^\*]*|\*[^\/]*)\*\/      ; /*ignore comments*/
\\[ \t]*\n                     ;
                               /* ignore continued-on-next-line-slashes */
[a-zA-Z_][a-zA-Z0-9_]*                               CwLibertylval = QString(yytext); return KEYWORD;
(\+|-)?[0-9]+                                        CwLibertylval = QString(yytext); return INTEGER;
(\+|-)?[0-9\.]+(e(\+|-)?[0-9]+)?                     CwLibertylval = QString(yytext); return REAL;
\"[^\"]*\"                                           CwLibertylval = QString(yytext); return QUOTED_STRING;
[a-zA-Z_0-9\\.\~\!\@\#\$\%\^\&\*\=]+[\(\[][0-9]+[\]\)]   CwLibertylval = QString(yytext); return ANY_WORD;
[a-zA-Z_0-9\\.\~\!\@\#\$\%\^\&\*\=]+                     CwLibertylval = QString(yytext); return ANY_WORD;
\n                             ;
                               /* ignore newline */
[ \t]+                         ; /* ignore whitespace */
.                              return (char) yytext[0]; 
%%

// \;                             ; /* ignore ';' itself (see comments at the end) */

// ignoring ';' because whitespace instead of ';' as a separator is acceptible
// and is sufficient to separate the tokens


