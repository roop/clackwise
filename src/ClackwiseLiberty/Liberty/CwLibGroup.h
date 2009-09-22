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
#include <QVariant>
#include <QList>
#include <QTextStream>
#include <QStringList>

#ifndef LIBGROUP_H
#define LIBGROUP_H

namespace Clackwise {

class CwLibGroup
{
public:

    enum LibAttributeType {
        UnknownLibAttribute = -1,
        SimpleLibAttribute = 0,
        ComplexLibAttribute
    };

    enum LibAttributeValueType {
        Invalid = -1,
        Boolean = 0,
        String,
        Integer,
        Float,
        Group
    };

    CwLibGroup(const QString& type = "", const QString& name = "");
    ~CwLibGroup();
    CwLibGroup(const CwLibGroup &other);
    CwLibGroup& operator=(const CwLibGroup &other);
    CwLibGroup* clone() const;

    QList<CwLibGroup*> parents() const;

    // type, like bus in bus(w[1])
    void setType(const QString& type);
    QString type() const;

    // name, like w[1] in bus(w[1])
    void setName(const QString& name);
    QString name() const;
    QString fullName() const;

    void insertSubgroup(int position, CwLibGroup *lg);
    void addSubgroup(CwLibGroup *lg);
    void replaceSubgroup(int position, CwLibGroup *lg);
    void removeSubgroupAt(int position);
    const CwLibGroup* subgroupAt(int position) const;
    QList<CwLibGroup*> subgroupsByName(const QString &type, const QString &pattern, int patternSyntax) const;
    QList<CwLibGroup*> subgroupsBySequence(const QString &typeSequence, const QString &pattern, int patternSyntax) const;
    void clearSubgroups();

    // set a simple attribute
    // any existing attribute of the same name will be replaced
    void setSimpleLibAttribute(const QString& name, const QString& value);
    void setUserAttribute(const QString& name, const QString& value);

    // set a complex attribute
    // any existing attribute of the same name will be replaced
    void setComplexLibAttribute(const QString& name, const QStringList& value);

    // set a multivalued complex attribute
    // all existing attributes of the same name will be kept
    // this is to support attributes like define, define_group,
    // which can occur multiple times legally
    void setLibDefine(const QString& name, const QStringList& value);

    // remove an attribute
    void removeLibAttribute(const QString& name);
    void removeUserAttribute(const QString& name);

    // remove a define or define_group special complex attribute
    void removeLibDefine(const QString &name, const QStringList &value);

	// check for an attribute by name
	bool hasLibAttribute(const QString& name) const;
	bool hasUserAttribute(const QString& name) const;
	bool hasLibDefine(const QString& name) const;

    // get the value of an attribute
    QStringList libAttributeValue(const QString& name) const;
    QString userAttributeValue(const QString& name) const;

    // query whether it's a simple attribute, or a complex attribute
	// note that even if libAttributeValue() returns a list with a single item,
	// it might be a complex attribute
    LibAttributeType libAttributeType(const QString& name) const;

    // get the value of defines
	QList<QStringList> libDefineValues(const QString& name) const;

    // remove all attributes
    void clearLibAttributes();
    void clearLibDefines();

    // return a list of all attributes
    QStringList libAttributes() const;
    QStringList userAttributes() const;
    QStringList libDefines() const;

    int subgroupsCount() const;

    QString toText(const QString& prefix = QString("")) const;

protected:
    void ref();
    void deref();
    void copyOnWrite();
    bool addParent(CwLibGroup *parent);
    bool removeParent(CwLibGroup *parent);

    class Private;
    Private *d;
};

}

#ifndef SWIG
Q_DECLARE_METATYPE(Clackwise::CwLibGroup*)
#endif

#endif
