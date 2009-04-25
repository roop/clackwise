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

#include "Troves.h"
#include "LibTrove.h"

Troves* Troves::s_instance = 0;

Troves* Troves::instance() {
    if (s_instance == 0)
        s_instance = new Troves();
    return s_instance;
}

void Troves::destroy() {
    delete s_instance;
    s_instance = 0;
}

LibTrove* Troves::libTrove(const QString &name) {
    if (!m_libTroves.contains(name))
        setLibTrove(name, new LibTrove(name));
    return m_libTroves.value(name);
}

void Troves::setLibTrove(const QString &name, LibTrove* trove) {
    removeLibTrove(name);
    m_libTroves[name] = trove;
}

void Troves::removeLibTrove(const QString &name) {
    if (m_libTroves.contains(name)) {
        delete m_libTroves.value(name);
        m_libTroves.remove(name);
	}
}

void Troves::clear() {
    QHashIterator<QString, LibTrove*> il(m_libTroves);
    while (il.hasNext()) {
        il.next();
        delete il.value();
    }
	m_libTroves.clear();
}

Troves::Troves()
    : m_currentTroveName(QString("lib")) {
    setLibTrove(currentTroveName(), new LibTrove(currentTroveName()));
}

Troves::~Troves() {
    clear();
}

void Troves::setCurrentTroveName(const QString& name) {
    m_currentTroveName = name;
}

QString Troves::currentTroveName() const {
    return m_currentTroveName;
}

LibTrove* Troves::currentLibTrove() {
    return libTrove(currentTroveName());
}

QList<QString> Troves::availableTroveNames() const {
    return m_libTroves.keys();
}

