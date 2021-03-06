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

#ifndef __TROVES_H
#define __TROVES_H

#include "CwLibTrove.h"

namespace Clackwise {

class CwTroves {
public:
    static CwTroves* instance();
    static void destroy();
    CwLibTrove* libTrove(const QString &name);
    void setLibTrove(const QString &name, CwLibTrove* trove);
    void removeLibTrove(const QString &name);
    void clear();
    void setCurrentTroveName(const QString& name);
    QString currentTroveName() const;
    QList<QString> availableTroveNames() const;
    CwLibTrove* currentLibTrove();

private:
    static CwTroves* s_instance;
    CwTroves();
    ~CwTroves();
    QHash<QString, CwLibTrove*> m_libTroves;
    // QHash<QString, CwLibTrove*> m_modelTroves; // for future use
    QString m_currentTroveName;
};

}
#endif
