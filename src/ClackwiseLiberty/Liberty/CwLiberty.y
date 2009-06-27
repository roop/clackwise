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
#include <QPair>
#include <QStringList>
#include <QVariantList>
#include "Liberty/CwLibGroup.h"
#include <stdio.h>

using namespace Clackwise;

#define YYSTYPE QVariant
#define YYDEBUG 1

int CwLibertylex();  
int CwLibertyparse(QVariant& ret);
extern int CwLibertydebug;
extern FILE* CwLibertyin;
extern int CwLibertylineno;
QString libertyFilename = "";
static bool parseErrorRecoveryTriggered;

CwLibGroup* parseLiberty(const QString &filename) {
    CwLibertydebug = 0;

    libertyFilename = filename;
    parseErrorRecoveryTriggered = false;

    CwLibertyin = fopen(filename.toAscii().data(), "r");
    if (CwLibertyin == NULL) {
	  printf("Error: Can't open lib file %s for reading\n", qPrintable(filename));
	  fflush(stdout);
      return NULL;
    }
    QVariant result;
    // cwlib_yyparse(p);
    // if (p.type() == typeid(CwLibGroup*))
    //     return any_cast<CwLibGroup*>(p);
    // else
    //     return NULL;
	if (CwLibertyparse(result) != 0 || parseErrorRecoveryTriggered) {
		return NULL;
	}
    fclose(CwLibertyin);

    libertyFilename = "";
    parseErrorRecoveryTriggered = false;

	return result.value<CwLibGroup*>();
}

void CwLibertyerror(QVariant& ret, const char *str) {
    parseErrorRecoveryTriggered = true;
	printf("Error: %s at line %d of library file %s\n", str, CwLibertylineno, qPrintable(libertyFilename));
	fflush(stdout);
}
 
  
%}

%token KEYWORD INTEGER REAL QUOTED_STRING ANY_WORD
%error-verbose
%parse-param {QVariant &ret}  // type of argument to libertyparse
%%

/*
simple_attribute:
         variable  ':'   value    ';'
         {
           ret = "<<" + $1.toString() + " = " + $3.toString() + ">>";
		   qDebug() << "-- simple_attribute" << $1 << $3;
         }

variable:
        KEYWORD ;

value:
        INTEGER | REAL | KEYWORD | QUOTED_STRING | ANY_WORD ;
*/

everything :         // of Type CwLibGroup*
         libgroups
         {
			ret = 0;
			if ($$.type() == QVariant::List &&
			    !$$.toList().isEmpty() &&
				$$.toList().first().value<CwLibGroup*>() != NULL) {
			    ret = $1.toList().first();
			}
         };

libgroups :           // of type QList<CwLibGroup*>
         libgrouphead libgrouptail
         {
		 	$$ = $1;
			if ($$.type() == QVariant::List &&
			    !$$.toList().isEmpty() &&
				$$.toList().first().value<CwLibGroup*>() != NULL) {
				foreach(QVariant v, $$.toList()) {
					CwLibGroup *lg = v.value<CwLibGroup*>();
					if (lg == NULL) {
						continue;
					}
					QVariantList statements = $2.value<QVariantList>();
					for (int i = 0; i < statements.count(); i++) {
						QVariant statement = statements.at(i);
						if (statement.value<CwLibGroup*>() != 0) { // lib group
							lg->addSubgroup(statement.value<CwLibGroup*>());
						} else if (statement.toList().size() > 2) { // attribute
							QVariantList attrStatement = statement.toList();
							QString variableName = attrStatement.at(1).toString();
							if (attrStatement.at(0).toString() == "simple_attribute") {
								lg->setSimpleLibAttribute(variableName, attrStatement.at(2).toString());
							} else if (attrStatement.at(0).toString() == "complex_attribute") {
								lg->setComplexLibAttribute(variableName, attrStatement.at(2).toStringList());
							} else if (attrStatement.at(0).toString() == "define") {
								lg->setLibDefine(variableName, attrStatement.at(2).toStringList());
							}
						}
					}
				}
			}
         }

libgrouphead :       // of type QList<CwLibGroup*>
	     variable '(' value_list ')'
         {
			QVariantList list;
			if ($3.toStringList().size() == 0) {
				list << QVariant::fromValue(new CwLibGroup($1.toString()));
			} else {
				list << QVariant::fromValue(new CwLibGroup($1.toString(), $3.toStringList().join(",")));
			}
			$$ = list;
         }
	   | variable '(' space_separated_value_list ')'
         {
			QVariantList list;
			if ($3.toStringList().size() == 0) {
				list << QVariant::fromValue(new CwLibGroup($1.toString()));
			} else {
				foreach(QString name, $3.toStringList()) {
					list << QVariant::fromValue(new CwLibGroup($1.toString(), name));
				}
			}
			$$ = list;
         }
       | variable '(' error ')'
         {
			QVariantList list;
			list << QVariant::fromValue(new CwLibGroup($1.toString()));
			$$ = list;
         }
       | error '(' value_list ')'
         {
			QVariantList list;
			list << QVariant::fromValue(new CwLibGroup());
			$$ = list;
         }
       | error '(' error ')'
         {
			QVariantList list;
			list << QVariant::fromValue(new CwLibGroup());
			$$ = list;
         };

libgrouptail :       // of type QVariantList
         '{' libstatements '}' 
         {
           $$ = $2;
         }
       | '{' libstatements '}' ';'
         {
           $$ = $2;
         }
         ;

libstatements :      // of type QVariantList
         {
           $$ = QVariantList();
         }
       | libstatements simple_attribute
         {
		 	$$ = $1.toList() << $2;
         }
       | libstatements complex_attribute
         {
		 	$$ = $1.toList() << $2;
         }
       | libstatements libgroups
         {
			$$ = $1.toList() << $2.toList();
         };

simple_attribute :   // of type QVariantList
         variable   ':'   value_expr
         {
           $$ = QVariantList() << "simple_attribute" << $1.toString() << $3.toString();
         }
       | variable   ':'   value_expr    ';'
         {
           $$ = QVariantList() << "simple_attribute" << $1.toString() << $3.toString();
         }
       | variable ':' error
         {
           $$ = QVariantList() << "simple_attribute" << $1.toString();
         }
       | error ':' value
         {
           $$ = QVariantList() << "simple_attribute" << QString();
         }
       | error ':' error
         {
           $$ = QVariantList() << "simple_attribute" << QString();
         };

complex_attribute :  // of type QVariantList
         variable   '('   value_list   ')'
         {
		   	if (QRegExp("define(_group|_cell_area)?").exactMatch($1.toString())) {
				$$ = QVariantList() << "define" << $1.toString() << $3.toStringList();
				QString attrType = $3.toStringList().last();
				if ($3.toStringList().size() != 3 ||
					!QRegExp("(boolean|string|integer|real)").exactMatch(attrType)) {
					$$ = QVariantList() << "define" << $1.toString();
					printf("Error: define or define_group statement incorrect at line %d"
					       " of library file %s . The statement was skipped.\n",
						   CwLibertylineno, qPrintable(libertyFilename));
					fflush(stdout);
				}
			} else {
		 		$$ = QVariantList() << "complex_attribute" << $1.toString() << $3.toStringList();
			}
         }
       | variable   '('   value_list   ')'   ';'
         {
		   	if (QRegExp("define(_group|_cell_area)?").exactMatch($1.toString())) {
				$$ = QVariantList() << "define" << $1.toString() << $3.toStringList();
				QString attrType = $3.toStringList().last();
				if ($3.toStringList().size() != 3 ||
					!QRegExp("(boolean|string|integer|real)").exactMatch(attrType)) {
					$$ = QVariantList() << "define" << $1.toString();
					printf("Error: define or define_group statement incorrect at line %d"
					       " of library file %s . The statement was skipped.\n",
						   CwLibertylineno, qPrintable(libertyFilename));
					fflush(stdout);
				}
			} else {
		 		$$ = QVariantList() << "complex_attribute" << $1.toString() << $3.toStringList();
			}
         }
       | variable '(' error ')'
         {
           $$ = QVariantList() << "complex_attribute" << $1.toString();
         }
       | error '(' value_list ')'
         {
           $$ = QVariantList() << "complex_attribute" << QString();
         }
       | error '(' error ')'
         {
           $$ = QVariantList() << "complex_attribute" << QString();
         };

value_list :         // of type QStringList
         {
           $$ = QStringList();
         }
       | value
         {
		   $$ = QStringList($1.toString());
         } 
       | value_list ',' value 
         {
		   $$ = $1.toStringList() << $3.toString();
         };

space_separated_value_list :     // of type QStringList
         value
         {
		   $$ = QStringList($1.toString());
         } 
       | space_separated_value_list value 
         {
		   $$ = $1.toStringList() << $2.toString();
         };

value_expr :               // of type QString
      value
      {
        $$ = $1.toString();
      }
    | value '+' value_expr
      {
        $$ = $1.toString() + " + " + $3.toString();
      }
    | value '-' value_expr
      {
        $$ = $1.toString() + " - " + $3.toString();
      }
    | value '*' value_expr
      {
        $$ = $1.toString() + " * " + $3.toString();
      }
    | value '/' value_expr
      {
        $$ = $1.toString() + " * " + $3.toString();
      }

variable :           // of type QString
        KEYWORD ;

value :              // of type QString
        INTEGER | REAL | KEYWORD | QUOTED_STRING | ANY_WORD ;

// TODO: %destructor{ delete $$ }
