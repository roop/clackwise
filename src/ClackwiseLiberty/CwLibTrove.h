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

#include <QString>
#include <QHash>

#ifndef __LIBTROVE_H
#define __LIBTROVE_H

#include "CwLib.h"

namespace Clackwise {

class CwLibTrove {
public:
	CwLibTrove(const QString &troveName);
	~CwLibTrove();
	void store(const QString &name, CwLib *dotlib);
	CwLib* retrieve(const QString &name) const;
	void remove(const QString &name);
	void clear();

private:
	QString const m_troveName;
	QHash<QString, CwLib*> m_data;
};

}
#endif
