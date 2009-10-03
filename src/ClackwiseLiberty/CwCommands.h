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

#include <QRegExp>

#ifndef __COMMANDS_H
#define __COMMANDS_H

#include "CwLib.h"

namespace Clackwise {

// Lib commands
Clackwise::CwLib* cw_read_lib(const QString &filename);
Clackwise::CwLib* cw_create_lib(const QString &name);
Clackwise::CwLib* cw_clone_lib(Clackwise::CwLib* src, const QString &name);
Clackwise::CwLibGroup* cw_create_lib_group(Clackwise::CwLibGroup* parent,
                        const QString& type, const QString &name);
QList<Clackwise::CwLib*> cw_get_libs(const QString &pattern, int patternSyntax);
void cw_write_lib(Clackwise::CwLib *dotlib, const QString &filename);

// General
QString object_to_string(Clackwise::CwLibGroup *libgroup);

// Trove commands
QList<QString> cw_all_troves();
QString cw_current_trove();
void cw_set_current_trove(const QString &name);

}
#endif
