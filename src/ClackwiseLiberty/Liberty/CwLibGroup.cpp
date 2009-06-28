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
    QHash<QString, QVariant> m_libAttributes;

	// store lib defines
    QMultiHash<QString, QStringList> m_libDefines;

	// store user attributes
    QHash<QString, QString> m_userAttributes;

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

void CwLibGroup::setSimpleLibAttribute(QString name, QString value)
{
    copyOnWrite();
	d->m_libAttributes.insert(name, value);
}

void CwLibGroup::setUserAttribute(QString name, QString value)
{
    copyOnWrite();
	d->m_userAttributes.insert(name, value);
}

void CwLibGroup::setComplexLibAttribute(QString name, QStringList value)
{
    copyOnWrite();
	d->m_libAttributes.insert(name, value);
}

void CwLibGroup::setLibDefine(QString name, QStringList value)
{
    copyOnWrite();
	d->m_libDefines.insertMulti(name, value);
}

void CwLibGroup::removeLibAttribute(QString name)
{
    copyOnWrite();
	d->m_libAttributes.remove(name);
}

void CwLibGroup::removeUserAttribute(QString name)
{
    copyOnWrite();
	d->m_userAttributes.remove(name);
}

void CwLibGroup::removeLibDefine(const QString &name, const QStringList &value)
{
    copyOnWrite();
	d->m_libDefines.remove(name, value);
}

bool CwLibGroup::hasLibAttribute(QString name) const
{
	return d->m_libAttributes.contains(name);
}

bool CwLibGroup::hasUserAttribute(QString name) const {
	return d->m_userAttributes.contains(name);
}

bool CwLibGroup::hasLibDefine(QString name) const {
	return d->m_libDefines.contains(name);
}

QStringList CwLibGroup::libAttributeValue(QString name) const
{
	if (d->m_libAttributes.contains(name)) {
		return d->m_libAttributes.value(name).toStringList();
	}
	return QStringList();
}

QString CwLibGroup::userAttributeValue(QString name) const
{
	return d->m_userAttributes.value(name);
}

CwLibGroup::LibAttributeType CwLibGroup::libAttributeType(QString name) const {
	if (d->m_libAttributes.contains(name)) {
		switch (d->m_libAttributes.value(name).type()) {
			case QVariant::StringList:
				return ComplexLibAttribute;
			case QVariant::String:
				return SimpleLibAttribute;
			default:
				return UnknownLibAttribute;
		}
	}
	return UnknownLibAttribute;
}

QList<QStringList> CwLibGroup::libDefineValues(QString name) const
{
	return d->m_libDefines.values(name);
}

void CwLibGroup::clearLibAttributes()
{
    copyOnWrite();
	d->m_libAttributes.clear();
}

void CwLibGroup::clearLibDefines()
{
    copyOnWrite();
	d->m_libDefines.clear();
}

QStringList CwLibGroup::libAttributes() const
{
    return d->m_libAttributes.keys();
}

QStringList CwLibGroup::userAttributes() const
{
    return d->m_userAttributes.keys();
}

QStringList CwLibGroup::libDefines() const
{
    return d->m_libDefines.uniqueKeys();
}

int CwLibGroup::subgroupsCount() const
{
    return d->m_subgroups.size();
}

QString CwLibGroup::toText(const QString &prefix) const
{
    QString outStr("");
    outStr += prefix + type() +  "(" + name() + ") {\n";
    QStringList defines = libDefines();
    foreach (QString defineCmd, defines) {
        foreach (QStringList valueList, libDefineValues(defineCmd)) {
			outStr += prefix + "  " + defineCmd + "(" + valueList.join(", ") + ");\n";
		}
	}
    QStringList attributes = libAttributes();
    foreach (QString attr, attributes) {
		QVariant val = libAttributeValue(attr);
		if (libAttributeType(attr) == SimpleLibAttribute) {
			outStr += prefix + "  " + attr + " : " + val.toString() + ";\n";
		} else if (libAttributeType(attr) == ComplexLibAttribute) {
			outStr += prefix + "  " + attr + "(" + val.toStringList().join(", ") + ");\n";
		} else {
			Q_ASSERT(0);
		}
	}
    for (int i = 0; i < subgroupsCount(); i++) {
        outStr += subgroupAt(i)->toText(prefix + "  ");
    }
    outStr += prefix + "}\n";
    return outStr;
}

}

