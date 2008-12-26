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
#include "DotLib.h"
#include "LibGroup.h"
#include "liberty.h"

DotLib::DotLib(const QString& filename)
{
	if (!filename.isNull()) {
		read(filename);
	}
}

DotLib::~DotLib()
{
}

DotLib::DotLib(const DotLib &other)
        : LibGroup(other)
{
}

DotLib& DotLib::operator=(const DotLib & other)
{
	LibGroup::operator=(other);
}

bool DotLib::read(const QString& filename)
{
    LibGroup* lg = parseLiberty(filename);
    if (lg == NULL) {
        return false;
    }
    *this = * ((DotLib*) lg);
    delete lg;
    return true;
}

bool DotLib::write(const QString& filename)
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
