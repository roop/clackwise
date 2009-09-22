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

#include <QFile>
#include "CwLib.h"
#include "Liberty/CwLibGroup.h"
#include "Liberty/CwLiberty.h"

namespace Clackwise {

CwLib::CwLib()
{
}

CwLib::CwLib(const QString& name)
{
    CwLibGroup::setName(name);
}

CwLib::~CwLib()
{
}

CwLib::CwLib(const CwLib &other)
        : CwLibGroup(other)
{
}

CwLib& CwLib::operator=(const CwLib & other)
{
	CwLibGroup::operator=(other);
	return *this;
}

CwLib* CwLib::clone() const {
    return (CwLib*) CwLibGroup::clone();
}

bool CwLib::read(const QString& filename)
{
    CwLibGroup* lg = parseLiberty(filename);
    if (lg == NULL) {
        return false;
    }
    *this = * ((CwLib*) lg);
    delete lg;
    return true;
}

bool CwLib::write(const QString& filename)
{
	QFile dotlib(filename);
	if (!dotlib.open(QIODevice::WriteOnly)) {
		return false;
	}
	if (!d) {
		return false;
	}
	dotlib.write(toText().toAscii().data());
	return true;
}

}
