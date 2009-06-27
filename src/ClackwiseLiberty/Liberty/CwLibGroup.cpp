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
    QMultiMap<QString, QVariant> m_libAttributes;
    QMultiMap<QString, QVariant> m_userAttributes;

    // store subgroups
    QList<CwLibGroup*> m_subgroups;

    // the same private object can be used in multiple parents, hence a list of parents
    QList<CwLibGroup*> m_parents;

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

QList<CwLibGroup*> CwLibGroup::parents() const {
    return d->m_parents;
}

bool CwLibGroup::addParent(CwLibGroup *parent) {
    if (!d->m_parents.contains(parent)) {
        d->m_parents.append(parent);
        return true;
    }
    return false;
}

bool CwLibGroup::removeParent(CwLibGroup *parent) {
    return d->m_parents.removeOne(parent);
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
    foreach(CwLibGroup *g, d->m_subgroups) {
        g->addParent(this);
    }
}

void CwLibGroup::deref()
{
    Q_ASSERT(d);
    Q_ASSERT(d->refCount);
    d->refCount--;
    foreach(CwLibGroup *g, d->m_subgroups) {
        g->removeParent(this);
    }
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
        new_d->m_libAttributes = d->m_libAttributes;
        new_d->m_userAttributes = d->m_userAttributes;
        new_d->m_subgroups = d->m_subgroups;
        new_d->m_parents << this;
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

QString CwLibGroup::fullName() const {
    if (parents().count() > 0) {
        if (!name().isEmpty()) {
            return parents().at(0)->fullName() + "/" + name();
        }
    }
    return name();
}

void CwLibGroup::insertSubgroup(int position, CwLibGroup *lg)
{
    copyOnWrite();
    d->m_subgroups.insert(position, lg);
    lg->addParent(this);
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

QList<CwLibGroup*> CwLibGroup::subgroupsByName(const QString &type, const QString &pattern, int patternSyntax) const {
    QList<CwLibGroup*> ret;
    QRegExp regexp(pattern, Qt::CaseSensitive, QRegExp::PatternSyntax(patternSyntax));
    foreach(CwLibGroup *subgroup, d->m_subgroups) {
        if (subgroup->type() == type && regexp.exactMatch(subgroup->name())) {
            ret << subgroup;
        }
    }
    return ret;
}

QList<CwLibGroup*> CwLibGroup::subgroupsBySequence(const QString &typeSequence, const QString &pattern, int patternSyntax) const {
    int t = typeSequence.indexOf('/');
    QString typeThis = typeSequence.mid(0, t);
    QString typeMore = (t >= 0)? typeSequence.mid(t + 1) : "";
    int p = pattern.indexOf('/');
    QString patternThis = pattern.mid(0, p);
    QString patternMore = (p >= 0)? pattern.mid(p + 1) : "";
    QList<CwLibGroup*> subgroups = subgroupsByName(typeThis, patternThis, patternSyntax);
    if (typeMore.isEmpty()) {
        return subgroups;
    }
    QList<CwLibGroup*> ret;
    foreach (CwLibGroup* g, subgroups) {
        ret << g->subgroupsBySequence(typeMore, patternMore, patternSyntax);
    }
    return ret;
}

void CwLibGroup::setSimpleAttribute(AttributeCategory category, QString name, QString value)
{
    copyOnWrite();
    switch (category) {
        case LibAttribute:
            d->m_libAttributes.replace(name, value);
            break;
        case UserAttribute:
            d->m_userAttributes.replace(name, value);
            break;
        default:
            Q_ASSERT(0);
    };
}

void CwLibGroup::setComplexAttribute(AttributeCategory category, QString name, QStringList value)
{
    copyOnWrite();
    switch (category) {
        case LibAttribute:
            d->m_libAttributes.replace(name, value);
            break;
        case UserAttribute:
            d->m_userAttributes.replace(name, value);
            break;
        default:
            Q_ASSERT(0);
    };
}

void CwLibGroup::setComplexAttribute(AttributeCategory category, QString name, QString value1,
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
    setComplexAttribute(category, name, valueList);
}

void CwLibGroup::setMultivaluedAttribute(AttributeCategory category, QString name, QStringList value)
{
    copyOnWrite();
    switch (category) {
        case LibAttribute:
            d->m_libAttributes.insert(name, value);
            break;
        case UserAttribute:
            d->m_userAttributes.insert(name, value);
            break;
        default:
            Q_ASSERT(0);
    };
}

void CwLibGroup::setMultivaluedAttribute(AttributeCategory category, QString name, QString value1,
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
    setMultivaluedAttribute(category, name, valueList);
}

void CwLibGroup::removeAttribute(AttributeCategory category, QString name)
{
    copyOnWrite();
    switch (category) {
        case LibAttribute:
            d->m_libAttributes.remove(name);
            break;
        case UserAttribute:
            d->m_userAttributes.remove(name);
            break;
        default:
            Q_ASSERT(0);
    };
}

void CwLibGroup::removeAttribute(AttributeCategory category, QString name, QVariant value)
{
    copyOnWrite();
    switch (category) {
        case LibAttribute:
            d->m_libAttributes.remove(name, value);
            break;
        case UserAttribute:
            d->m_userAttributes.remove(name, value);
            break;
        default:
            Q_ASSERT(0);
    };
}

bool CwLibGroup::hasAttribute(AttributeCategory category, QString name) const
{
    switch (category) {
        case LibAttribute:
            return d->m_libAttributes.contains(name);
        case UserAttribute:
            return d->m_userAttributes.contains(name);
        default:
            Q_ASSERT(0);
    };
	return false;
}

QVariant CwLibGroup::attributeValue(AttributeCategory category, QString name) const
{
    QVariantList values;
    switch (category) {
        case LibAttribute:
            values = d->m_libAttributes.values(name);
            break;
        case UserAttribute:
            values = d->m_userAttributes.values(name);
            break;
        default:
            Q_ASSERT(0);
    };
    if (values.size() == 1) {
        return values.at(0);
    }
    return values;
}

void CwLibGroup::clearAttributes(AttributeCategory category)
{
    copyOnWrite();
    switch (category) {
        case LibAttribute:
            d->m_libAttributes.clear();
            break;
        case UserAttribute:
            d->m_userAttributes.clear();
            break;
        default:
            Q_ASSERT(0);
    };
}

QStringList CwLibGroup::attributes(AttributeCategory category) const
{
    switch (category) {
        case LibAttribute:
            return d->m_libAttributes.uniqueKeys();
            break;
        case UserAttribute:
            return d->m_userAttributes.uniqueKeys();
            break;
        default:
            Q_ASSERT(0);
    };
    return QStringList();
}

int CwLibGroup::subgroupsCount() const
{
    return d->m_subgroups.size();
}

int CwLibGroup::attributesCount(AttributeCategory category) const
{
    switch (category) {
        case LibAttribute:
            return d->m_libAttributes.size();
            break;
        case UserAttribute:
            return d->m_userAttributes.size();
            break;
        default:
            Q_ASSERT(0);
    };
    return 0;
}

QString CwLibGroup::toText(const QString &prefix) const
{
    QString outStr("");
    outStr += prefix + type() +  "(" + name() + ") {\n";
    QStringList attrs = attributes(LibAttribute);
    for (int i = 0; i < attrs.size(); i++) {
        QVariant val = attributeValue(LibAttribute, attrs[i]);
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

