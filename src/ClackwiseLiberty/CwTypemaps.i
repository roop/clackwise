/*

Copyright (c) 2009 Roopesh Chander <roop@forwardbias.in>

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
#include <QString>
#include <QList>
%}

#ifdef SWIGTCL
    %typemap(in) QString {
        int strLength;
        char *str = Tcl_GetStringFromObj($input, &strLength);
        $1 = QString(str);
    }
    %typemap(in) QString *, QString & {
        int strLength;
        char *str = Tcl_GetStringFromObj($input, &strLength);
        $1 = new QString(str);
    }
    %typemap(freearg) QString *, QString & {
        delete $1;
    }
    %typemap(out) QString {
        Tcl_SetResult(interp, ((char *) ($1).toAscii().data()), TCL_VOLATILE);
    }
    %typemap(typecheck) QString = char *;

    %typemap(out) QList<Clackwise::CwLib*> {
        Tcl_Obj *listObj = Tcl_NewListObj(0, 0);
        QList<Clackwise::CwLib*> l = QList<Clackwise::CwLib*>($1);
        for (int i = 0; i < l.size(); i++) {
            Tcl_Obj *elemObj = SWIG_NewInstanceObj(SWIG_as_voidptr(l.at(i)), $descriptor(Clackwise::CwLib*),0);
            Tcl_ListObjAppendElement(interp, listObj, elemObj);
        }
        Tcl_SetObjResult(interp, listObj);
    }
    %typemap(out) QList<Clackwise::CwLibGroup*> {
        Tcl_Obj *listObj = Tcl_NewListObj(0, 0);
        QList<Clackwise::CwLibGroup*> l = QList<Clackwise::CwLibGroup*>($1);
        for (int i = 0; i < l.size(); i++) {
            Tcl_Obj *elemObj = SWIG_NewInstanceObj(SWIG_as_voidptr(l.at(i)), $descriptor(Clackwise::CwLibGroup*),0);
            Tcl_ListObjAppendElement(interp, listObj, elemObj);
        }
        Tcl_SetObjResult(interp, listObj);
    }
#endif


