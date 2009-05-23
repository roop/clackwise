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

    enum AttributeType {
        LibAttribute,
        UserAttribute
    };

    enum LibAttributeValueType {
        Invalid = -1,
        Boolean = 0,
        String,
        Integer,
        Float,
        Group
    };

    CwLibGroup(QString type = "", QString name = "");
    ~CwLibGroup();
    CwLibGroup(const CwLibGroup &other);
    CwLibGroup& operator=(const CwLibGroup &other);

    // type, like bus in bus(w[1])
    void setType(QString type);
    QString type() const;

    // name, like w[1] in bus(w[1])
    void setName(QString name);
    QString name() const;

    void insertSubgroup(int position, CwLibGroup *lg);
    void addSubgroup(CwLibGroup *lg);
    void replaceSubgroup(int position, CwLibGroup *lg);
    void removeSubgroupAt(int position);
    const CwLibGroup* subgroupAt(int position) const;
    void clearSubgroups();

    // set a simple attribute
    // any existing attribute of the same name will be replaced
    void setSimpleAttribute(AttributeType type, QString name, QString value);

    // set a complex attribute
    // any existing attribute of the same name will be replaced
    void setComplexAttribute(AttributeType type, QString name, QStringList value);
    void setComplexAttribute(AttributeType type, QString name, QString value1,
                             QString value2 = QString(),
                             QString value3 = QString(),
                             QString value4 = QString(),
                             QString value5 = QString());

    // set a multivalued complex attribute
    // all existing attributes of the same name will be kept
    // this is to support attributes like define, define_group,
    // which can occur multiple times legally
    void setMultivaluedAttribute(AttributeType type, QString name, QStringList value);
    void setMultivaluedAttribute(AttributeType type, QString name, QString value1,
                                 QString value2 = QString(),
                                 QString value3 = QString(),
                                 QString value4 = QString(),
                                 QString value5 = QString());

    // remove an attribute
    void removeAttribute(AttributeType type, QString name);

    // remove a particular value of an attribute
    // removes the attribute only if the value is as specified
    // this is more relevant for multivalued attributes
    void removeAttribute(AttributeType type, QString name, QVariant value);

    // get the value of an attribute
    // the attribute type can be derived from the return type
    // QString => simple attribute
    // QList<QString> => complex attribute
    // QList<QList<QString> > => multivalued complex attribute
    QVariant attributeValue(AttributeType type, QString name) const;

    // remove all attributes
    void clearAttributes(AttributeType type);

    // return a list of all attributes
    QStringList attributes(AttributeType type) const;

    int subgroupsCount() const;
    int attributesCount(AttributeType type) const;

    QString toText(const QString& prefix = QString("")) const;

protected:
    void ref();
    void deref();
    void copyOnWrite();

    class Private;
    Private *d;
};

}

#ifndef SWIG
Q_DECLARE_METATYPE(Clackwise::CwLibGroup*)
#endif

#endif
