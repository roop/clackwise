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

#include "LibGroup.h"

LibGroup::LibGroup(QString type, QString name)
    : m_type(type), m_name(name) {
}

LibGroup::~LibGroup() {
}

void LibGroup::setType(QString type) {
    m_type = type;
}

QString LibGroup::type() const {
    return m_type;
}

void LibGroup::setName(QString name) {
    m_name = name;
}

QString LibGroup::name() const {
    return m_name;
}

void LibGroup::insertSubgroup(int position, LibGroup *lg) {
    m_subgroups.insert(position, lg);
}

void LibGroup::replaceSubgroup(int position, LibGroup *lg) {
    m_subgroups[position] = lg;
}

void LibGroup::removeSubgroupAt(int position) {
    m_subgroups.removeAt(position);
}

void LibGroup::clearSubgroups() {
    m_subgroups.clear();
}

const LibGroup* LibGroup::subgroupAt(int position) const {
    return m_subgroups.at(position);
}

void LibGroup::setSimpleAttribute(QString name, QString value) {
    m_attributes.replace(name, value);
}

void LibGroup::setComplexAttribute(QString name, QStringList value) {
    m_attributes.replace(name, value);
}

void LibGroup::setComplexAttribute(QString name, QString value1,
                                                 QString value2,
                                                 QString value3,
                                                 QString value4,
                                                 QString value5) {
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

void LibGroup::setMultivaluedAttribute(QString name, QStringList value) {
    m_attributes.insert(name, value);
}

void LibGroup::setMultivaluedAttribute(QString name, QString value1,
                                                     QString value2,
                                                     QString value3,
                                                     QString value4,
                                                     QString value5) {
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
void LibGroup::removeAttribute(QString name) {
    m_attributes.remove(name);
}

void LibGroup::removeAttribute(QString name, QVariant value) {
    m_attributes.remove(name, value);
}

QVariant LibGroup::attributeValue(QString name) const {
    QVariantList values = m_attributes.values(name);
    if (values.size() == 1) {
        return values.at(0);
    }
    return values;
}

void LibGroup::clearAttributes() {
    m_attributes.clear();
}

QStringList LibGroup::attributes() const {
    return m_attributes.uniqueKeys();
}

int LibGroup::subgroupsCount() const {
    return m_subgroups.size();
}

int LibGroup::attributesCount() const {
    return m_attributes.size();
}

QString LibGroup::toText(const QString &prefix) const {
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

