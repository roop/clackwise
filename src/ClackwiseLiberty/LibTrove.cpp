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

#include "LibTrove.h"

namespace Clackwise {

LibTrove::LibTrove(const QString &troveName)
    : m_troveName(troveName) {
}

void LibTrove::store(const QString &name, DotLib *dotlib) {
	remove(name);
    m_data[name] = dotlib;
}

DotLib* LibTrove::retrieve(const QString &name) const {
    if (m_data.contains(name))
        return m_data.value(name);
    return 0;
}

void LibTrove::remove(const QString &name) {
    if (m_data.contains(name)) {
        delete m_data.value(name);
        m_data.remove(name);
	}
}

void LibTrove::clear() {
    QHashIterator<QString, DotLib*> i(m_data);
    while (i.hasNext()) {
        i.next();
        delete i.value();
    }
	m_data.clear();
}

LibTrove::~LibTrove() {
	clear();
}


}
