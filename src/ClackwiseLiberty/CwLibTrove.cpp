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

#include "CwLibTrove.h"

namespace Clackwise {

CwLibTrove::CwLibTrove(const QString &troveName)
    : m_troveName(troveName) {
}

void CwLibTrove::store(const QString &name, CwLib *dotlib) {
	remove(name);
    m_data[name] = dotlib;
}

CwLib* CwLibTrove::retrieve(const QString &name) const {
    if (m_data.contains(name))
        return m_data.value(name);
    return 0;
}


QList<CwLib*> CwLibTrove::retrieve(const QString &pattern, QRegExp::PatternSyntax patternSyntax) const {
    QList<CwLib*> ret;
    QRegExp regexp(pattern, Qt::CaseSensitive, patternSyntax);
    QHash<QString, CwLib*>::const_iterator i = m_data.constBegin();
    while (i != m_data.constEnd()) {
        if (regexp.exactMatch(i.key())) {
            ret << i.value();
        }
        ++i;
    }
    return ret;
}

void CwLibTrove::remove(const QString &name) {
    if (m_data.contains(name)) {
        delete m_data.value(name);
        m_data.remove(name);
	}
}

void CwLibTrove::clear() {
    QHashIterator<QString, CwLib*> i(m_data);
    while (i.hasNext()) {
        i.next();
        delete i.value();
    }
	m_data.clear();
}

CwLibTrove::~CwLibTrove() {
	clear();
}


}
