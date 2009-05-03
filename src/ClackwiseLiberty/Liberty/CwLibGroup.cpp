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

#include <QStringList>
#include <QDebug>

#include "Liberty/CwLibGroup.h"
#include "Liberty/CwLiberty.h"

namespace Clackwise {

class CwLibGroup::Private
{
public:
    Private(QString type, QString name)
            : m_type(type), m_name(name), refCount(1) {
    }
    ~Private() {
    };

    QString m_type;
    QString m_name;

    // store simple and complex attributes
    // if the value is a list, it's a complex attribute. (like capacitive_load_unit(1,pf); )
    // else a simple attribute. (like voltage_unit: 1mV; )
    QMultiMap<QString, QVariant> m_attributes;

    // store subgroups
    QList<CwLibGroup*> m_subgroups;

    // reference count for implicit sharing
    int refCount;
};

CwLibGroup::CwLibGroup(QString type, QString name)
        : d(new Private(type, name))
{
}

CwLibGroup::CwLibGroup(const CwLibGroup &other)
        : d(other.d)
{
    ref();
}

CwLibGroup& CwLibGroup::operator=(const CwLibGroup & other)
{
    if (this != &other) {
        deref();
        d = other.d;
        ref();
    }
    return *this;
}

CwLibGroup::~CwLibGroup()
{
    deref();
}

void CwLibGroup::ref()
{
    Q_ASSERT(d);
    Q_ASSERT(d->refCount);
    d->refCount++;
}

void CwLibGroup::deref()
{
    Q_ASSERT(d);
    Q_ASSERT(d->refCount);
    d->refCount--;
    if (d->refCount == 0) {
        delete d;
        d = 0;
    }
}

void CwLibGroup::copyOnWrite()
{
    if (d->refCount > 1) {
        deref();
        Private *new_d = new Private(type(), name());
        new_d->m_attributes = d->m_attributes;
        new_d->m_subgroups = d->m_subgroups;
        d = new_d;
    }
}

void CwLibGroup::setType(QString type)
{
    copyOnWrite();
    d->m_type = type;
}

QString CwLibGroup::type() const
{
    return d->m_type;
}

void CwLibGroup::setName(QString name)
{
    copyOnWrite();
    d->m_name = name;
}

QString CwLibGroup::name() const
{
    return d->m_name;
}

void CwLibGroup::insertSubgroup(int position, CwLibGroup *lg)
{
    copyOnWrite();
    d->m_subgroups.insert(position, lg);
}

void CwLibGroup::addSubgroup(CwLibGroup *lg)
{
    insertSubgroup(subgroupsCount(), lg);
}

void CwLibGroup::replaceSubgroup(int position, CwLibGroup *lg)
{
    copyOnWrite();
    d->m_subgroups[position] = lg;
}

void CwLibGroup::removeSubgroupAt(int position)
{
    copyOnWrite();
    d->m_subgroups.removeAt(position);
}

void CwLibGroup::clearSubgroups()
{
    copyOnWrite();
    d->m_subgroups.clear();
}

const CwLibGroup* CwLibGroup::subgroupAt(int position) const
{
    return d->m_subgroups.at(position);
}

void CwLibGroup::setSimpleAttribute(QString name, QString value)
{
    copyOnWrite();
    d->m_attributes.replace(name, value);
}

void CwLibGroup::setComplexAttribute(QString name, QStringList value)
{
    copyOnWrite();
    d->m_attributes.replace(name, value);
}

void CwLibGroup::setComplexAttribute(QString name, QString value1,
                                   QString value2,
                                   QString value3,
                                   QString value4,
                                   QString value5)
{
    QStringList valueList;
    valueList.append(value1);
    if (!value2.isNull())
        valueList.append(value2);
    if (!value3.isNull())
        valueList.append(value3);
    if (!value4.isNull())
        valueList.append(value4);
    if (!value5.isNull())
        valueList.append(value5);
    setComplexAttribute(name, valueList);
}

void CwLibGroup::setMultivaluedAttribute(QString name, QStringList value)
{
    copyOnWrite();
    d->m_attributes.insert(name, value);
}

void CwLibGroup::setMultivaluedAttribute(QString name, QString value1,
                                       QString value2,
                                       QString value3,
                                       QString value4,
                                       QString value5)
{
    QStringList valueList;
    valueList.append(value1);
    if (!value2.isNull())
        valueList.append(value2);
    if (!value3.isNull())
        valueList.append(value3);
    if (!value4.isNull())
        valueList.append(value4);
    if (!value5.isNull())
        valueList.append(value5);
    setMultivaluedAttribute(name, valueList);
}

void CwLibGroup::removeAttribute(QString name)
{
    copyOnWrite();
    d->m_attributes.remove(name);
}

void CwLibGroup::removeAttribute(QString name, QVariant value)
{
    copyOnWrite();
    d->m_attributes.remove(name, value);
}

QVariant CwLibGroup::attributeValue(QString name) const
{
    QVariantList values = d->m_attributes.values(name);
    if (values.size() == 1) {
        return values.at(0);
    }
    return values;
}

void CwLibGroup::clearAttributes()
{
    copyOnWrite();
    d->m_attributes.clear();
}

QStringList CwLibGroup::attributes() const
{
    return d->m_attributes.uniqueKeys();
}

int CwLibGroup::subgroupsCount() const
{
    return d->m_subgroups.size();
}

int CwLibGroup::attributesCount() const
{
    return d->m_attributes.size();
}

QString CwLibGroup::toText(const QString &prefix) const
{
    QString outStr("");
    outStr += prefix + type() +  "(" + name() + ") {\n";
    QStringList attrs = attributes();
    for (int i = 0; i < attrs.size(); i++) {
        QVariant val = attributeValue(attrs[i]);
        QVariantList multiVals;
        switch (val.type()) {
        case QMetaType::QString: // simple attribute
            outStr += prefix + "  " + attrs[i] + " : " + val.toString() + ";\n";
            break;
        case QMetaType::QStringList: // complex attribute
            outStr += prefix + "  " + attrs[i] + "(" + val.toStringList().join(", ") + ");\n";
            break;
        case QMetaType::QVariantList: // multivalued complex attribute
            multiVals = val.toList();
            for (int j = 0; j < multiVals.size(); j++) {
                outStr += prefix + "  " + attrs[i] + "(" + multiVals.at(j).toStringList().join(", ") + ");\n";
            }
            break;
        default:
            break;
        }
    }
    for (int i = 0; i < subgroupsCount(); i++) {
        outStr += subgroupAt(i)->toText(prefix + "  ");
    }
    outStr += prefix + "}\n";
    return outStr;
}

}

