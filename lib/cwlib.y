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
#include <stdio.h>
#include <iostream>
#include <vector>
#include <tr1/unordered_map>
#include <boost/functional/hash.hpp>
#include <boost/any.hpp>
#include "LibData.h"
#define YYSTYPE any

using namespace std;
using namespace boost;

extern "C"
{
        int cwlib_yyparse(any& ret);
        int cwlib_yywrap()
        {
                return 1;
        }

}
int cwlib_yylex(void);  

extern int cwlib_yydebug;
extern FILE* cwlib_yyin;
extern int cwlib_yylineno;
string cwlib_libFilename = "";
bool cwlib_parseError;

LibGroup * parseLib(string libFile) {
 //   cwlib_yydebug = 1;

    cwlib_yyin = fopen( libFile.c_str(), "r" );
    if (cwlib_yyin == NULL) {
      cout << "Error: Can't open lib file " << libFile << " for reading" << endl;
      return NULL;
    }
    cwlib_libFilename = libFile;
    cwlib_parseError = false;
    any p;
    // cwlib_yyparse(p);
    // if (p.type() == typeid(LibGroup*))
    //     return any_cast<LibGroup*>(p);
    // else
    //     return NULL;
    if (cwlib_yyparse(p) == 0 && !cwlib_parseError && p.type() == typeid(LibGroup*))
        return any_cast<LibGroup*>(p);
    else
        return NULL;
    fclose(cwlib_yyin);
}

void cwlib_yyerror(any& ret, const char *str) {
    cwlib_parseError = true;
    printf("Error: %s at line %d of library file %s\n", str, cwlib_yylineno, any_cast<string>(cwlib_libFilename).c_str());
}
 
  
%}

%token KEYWORD INTEGER REAL QUOTED_STRING ANY_WORD
%error-verbose
%parse-param {any& ret}  // argument to yyparse
%%

everything :         // of Type LibGroup*
         libgroup
         {
           ret = $1;
         };

libgroup :           // of type LibGroup*
         libgrouphead libgrouptail
         {
           if (any_cast<LibGroup*>($1))
             any_cast<LibGroup*>($1)->setStatements( any_cast<vector<any>*>($2) );
           $$ = $1;
         }

libgrouphead :       // of type LibGroup*
         variable '(' value_list ')'
         {
           switch(any_cast<vector<any>*>($3)->size()) {
               case 0:  // value_list is empty
                   $$ = new LibGroup(any_cast<string>($1), string(""));
                   break;
               case 1:  // value list has one element
                   $$ = new LibGroup(any_cast<string>($1),
                             any_cast<string>((any_cast<vector<any>*>($3))->at(0)) );
                   break;
               default: // value list has >1 element. bad.
                   $$ = new LibGroup(any_cast<string>($1),
                             any_cast<string>((any_cast<vector<any>*>($3))->at(0)) );
                   cout << "Warning: Expected zero or one but got more as name for "
                        << any_cast<string>($1) << " at line " << cwlib_yylineno
                        << " of library file " << cwlib_libFilename
                        << ". Using the first value ("
                        << any_cast<string>((any_cast<vector<any>*>($3))->at(0))
                        << ") as the name." << endl;
           }
         }
       | variable '(' error ')'
         {
           $$ = (LibGroup*) NULL;
         }
       | error '(' value_list ')'
         {
           $$ = (LibGroup*) NULL;
         }
       | error '(' error ')'
         {
           $$ = (LibGroup*) NULL;
         };

libgrouptail :       // of type vector<any>*
         '{' libstatements '}' 
         {
           $$ = $2;
         }
       | '{' libstatements '}' ';'
         {
           $$ = $2;
         }
         ;

libstatements :      // of type vector<any>*
         {
           $$ = new vector<any>(0);
         }
       | libstatements simple_attribute
         {
           any_cast<vector<any>*>($1)->push_back( any_cast<LibAttribute*>($2) );
           $$ = $1;
         }
       | libstatements complex_attribute
         {
           any_cast<vector<any>*>($1)->push_back( any_cast<LibAttribute*>($2) );
           $$ = $1;
         }
       | libstatements libgroup
         {
           any_cast<vector<any>*>($1)->push_back( any_cast<LibGroup*>($2) );
           $$ = $1;
         };

simple_attribute :   // of type LibAttribute*
         variable   ':'   value    
         {
           $$ = new LibAttribute(any_cast<string>($1), any_cast<string>($3));
         }
       | variable   ':'   value    ';'
         {
           $$ = new LibAttribute(any_cast<string>($1), any_cast<string>($3));
         }
       | variable ':' error
         {
           $$ = (LibAttribute*) NULL;
         }
       | error ':' value
         {
           $$ = (LibAttribute*) NULL;
         }
       | error ':' error
         {
           $$ = (LibAttribute*) NULL;
         };

complex_attribute :  // of type LibAttribute*
         variable   '('   value_list   ')'
         {
           $$ = new LibAttribute(any_cast<string>($1), any_cast<vector<any>*>($3));
         }
       | variable   '('   value_list   ')'   ';'
         {
           $$ = new LibAttribute(any_cast<string>($1), any_cast<vector<any>*>($3));
         }
       | variable '(' error ')'
         {
           $$ = (LibAttribute*) NULL;
         }
       | error '(' value_list ')'
         {
           $$ = (LibAttribute*) NULL;
         }
       | error '(' error ')'
         {
           $$ = (LibAttribute*) NULL;
         };

variable :           // of type string
        KEYWORD ;
value :              // of type string
        INTEGER | REAL | KEYWORD | QUOTED_STRING | ANY_WORD ;

value_list :         // of type vector<any>*
         {
           $$ = new vector<any>(0);
         }
       | value
         {
           $$ = new vector<any>(1, any_cast<string>($1));
         } 
       | value_list ',' value 
         {
           any_cast<vector<any>*>($1)->push_back( any_cast<string>($3) );
           $$ = $1
         };

// TODO: %destructor{ delete $$ }
