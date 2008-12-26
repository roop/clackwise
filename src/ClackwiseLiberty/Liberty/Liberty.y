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
#include "Liberty/LibGroup.h"
#define YYSTYPE QVariant
#define YYDEBUG 1

int Libertylex();  
int Libertyparse(QVariant& ret);
extern int Libertydebug;
extern FILE* Libertyin;
extern int Libertylineno;
QString libertyFilename = "";
static bool parseErrorRecoveryTriggered;

LibGroup* parseLiberty(const QString &filename) {
    Libertydebug = 0;

    libertyFilename = filename;
    parseErrorRecoveryTriggered = false;

    Libertyin = fopen(filename.toAscii().data(), "r");
    if (Libertyin == NULL) {
      qDebug() << "Error: Can't open lib file " << filename << " for reading" << endl;
      return NULL;
    }
    QVariant result;
    // cwlib_yyparse(p);
    // if (p.type() == typeid(LibGroup*))
    //     return any_cast<LibGroup*>(p);
    // else
    //     return NULL;
	if (Libertyparse(result) != 0 || parseErrorRecoveryTriggered) {
		return NULL;
	}
    fclose(Libertyin);

    libertyFilename = "";
    parseErrorRecoveryTriggered = false;

	return result.value<LibGroup*>();
}

void Libertyerror(QVariant& ret, const char *str) {
    parseErrorRecoveryTriggered = true;
    qDebug() << QString("Error: %1 at line %2 of library file %3\n").arg(str).arg(Libertylineno).arg(libertyFilename);
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

everything :         // of Type LibGroup*
         libgroup
         {
           ret = $1;
         };

libgroup :           // of type LibGroup*
         libgrouphead libgrouptail
         {
		 	$$ = $1;
			if ($$.value<LibGroup*>() != NULL) {
				LibGroup *lg = $$.value<LibGroup*>();
				QVariantList statements = $2.value<QVariantList>();
				for (int i = 0; i < statements.count(); i++) {
					QVariant statement = statements.at(i);
					if (statement.value<LibGroup*>() != 0) { // lib group
						lg->addSubgroup(statement.value<LibGroup*>());
					} else if (statement.toList().size() > 2) { // attribute
						QVariantList attrStatement = statement.toList();
						QString variableName = attrStatement.at(1).toString();
						if (attrStatement.at(0).toString() == "simple_attribute") {
							lg->setSimpleAttribute(variableName, attrStatement.at(2).toString());
						} else if (attrStatement.at(0).toString() == "complex_attribute") {
							lg->setComplexAttribute(variableName, attrStatement.at(2).toStringList());
						} else if (attrStatement.at(0).toString() == "multivalued_attribute") {
							lg->setMultivaluedAttribute(variableName, attrStatement.at(2).toStringList());
						}
					}
				}
			}
         }

libgrouphead :       // of type LibGroup*
	     variable '(' value_list ')'
         {
		 	$$ = QVariant();
			if ($3.toStringList().size() == 0) {
				$$.setValue<LibGroup*>(new LibGroup($1.toString()));
			} else if ($3.toStringList().size() == 1) {
				$$.setValue<LibGroup*>(new LibGroup($1.toString(), $3.toStringList().at(0)));
			} else {
				Libertyerror($$, "Expecting one or zero names for lib group, but got more");
				YYERROR;
			}
         }
       | variable '(' error ')'
         {
		 	$$ = QVariant();
			$$.setValue<LibGroup*>(new LibGroup($1.toString()));
         }
       | error '(' value_list ')'
         {
		 	$$ = QVariant();
			$$.setValue<LibGroup*>(new LibGroup());
         }
       | error '(' error ')'
         {
		 	$$ = QVariant();
			$$.setValue<LibGroup*>(new LibGroup());
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
       | libstatements libgroup
         {
		 	$$ = $1.toList() << $2;
         };

simple_attribute :   // of type QVariantList
         variable   ':'   value    
         {
           $$ = QVariantList() << "simple_attribute" << $1.toString() << $3.toString();
         }
       | variable   ':'   value    ';'
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
				$$ = QVariantList() << "multivalued_attribute" << $1.toString() << $3.toStringList();
				QString attrType = $3.toStringList().last();
				if ($3.toStringList().size() != 3 ||
		    		QRegExp("(boolean|string|integer|float)").exactMatch(attrType)) {
					$$ = QVariantList() << "multivalued_attribute" << $1.toString();
					qDebug() << "Error: define or define_group statement incorrect at line " << Libertylineno
						 	 << " of library file " << libertyFilename << ". Skipped." << endl;
				}
			} else {
		 		$$ = QVariantList() << "complex_attribute" << $1.toString() << $3.toStringList();
			}
         }
       | variable   '('   value_list   ')'   ';'
         {
		   	if (QRegExp("define(_group|_cell_area)?").exactMatch($1.toString())) {
				$$ = QVariantList() << "multivalued_attribute" << $1.toString() << $3.toStringList();
				QString attrType = $3.toStringList().last();
				if ($3.toStringList().size() != 3 ||
		    		QRegExp("(boolean|string|integer|float)").exactMatch(attrType)) {
					$$ = QVariantList() << "multivalued_attribute" << $1.toString();
					qDebug() << "Error: define or define_group statement incorrect at line " << Libertylineno
						 	 << " of library file " << libertyFilename << ". Skipped." << endl;
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

variable :           // of type QString
        KEYWORD ;

value :              // of type QString
        INTEGER | REAL | KEYWORD | QUOTED_STRING | ANY_WORD ;

// TODO: %destructor{ delete $$ }
