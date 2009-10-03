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

#include <QTextStream>
#include <QDebug>
#include "CwCommands.h"
#include "CwLib.h"
#include "CwTroves.h"

namespace Clackwise {

CwLib* cw_read_lib(const QString &filename) {
    CwLib* dotlib = new CwLib();
    dotlib->read(filename);
    CwTroves::instance()->currentLibTrove()->store(dotlib->name(), dotlib);
    return dotlib;
}

CwLib* cw_create_lib(const QString &name) {
    if (name.isEmpty()) {
        return 0;
    }
    CwLib* dotlib = new CwLib(name);
    CwTroves::instance()->currentLibTrove()->store(dotlib->name(), dotlib);
    return dotlib;
}

CwLib* cw_clone_lib(CwLib* src, const QString &name) {
    if (name.isEmpty()) {
        return 0;
    }
    CwLib* dotlib = src->clone();
    dotlib->setName(name);
    CwTroves::instance()->currentLibTrove()->store(dotlib->name(), dotlib);
    return dotlib;
}

Clackwise::CwLibGroup* cw_create_lib_group(CwLibGroup* parent,
                        const QString& type, const QString &name) {
    CwLibGroup *lg = new CwLibGroup(type, name);
    parent->addSubgroup(lg);
    return lg;
}

QList<CwLib*> cw_get_libs(const QString &pattern, int patternSyntax) {
    QList<CwLib*> l = CwTroves::instance()->currentLibTrove()->retrieve(pattern, QRegExp::PatternSyntax(patternSyntax));
    return l;
}

void cw_write_lib(CwLib *dotlib, const QString &filename) {
	dotlib->write(filename);
}

QString object_to_string(CwLibGroup *libgroup) {
    return libgroup->fullName();
}

QList<QString> cw_all_troves() {
    return CwTroves::instance()->availableTroveNames();
}

QString cw_current_trove() {
    return CwTroves::instance()->currentTroveName();
}

void cw_set_current_trove(const QString &name) {
    CwTroves::instance()->setCurrentTroveName(name);
}

}
