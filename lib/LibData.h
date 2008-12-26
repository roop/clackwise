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

#ifndef LIBDATA_H

#define LIBDATA_H

#include <iostream>
#include <vector>
#include <tr1/unordered_map>
#include <boost/any.hpp>

using namespace std;
using namespace std::tr1;
using namespace boost;

struct LibAttribute {
    string name;
    any value;
    // value can be any of: 
    //   string        - like  time_unit : "1ns" ;
    //   vector<string>*  - like  line (26, 0, 100, 0);
    LibAttribute(const string& n, const any& v): name(n), value(v) {};
    ~LibAttribute();
    void Write(ostream& out) const;
};
void WriteLibAttributeValue(ostream& out, const any& value);

struct LibDefine {
    enum Type {
      Invalid = -1,
      Boolean = 0,
      String,
      Integer,
      Float,
      Group
    };
    string name;
    string group;
    Type type;
    static Type getAttrType(const string& s);
    LibDefine(const string& n, const string& g, const Type t):
                name(n), group(g), type(t) {};
    // for define(a, b, c), call Define("define", ["a", "b", "c"])
    // for define_group(a, b), call Define("define_group", ["a", "b"])
    LibDefine(const string& d, const vector<string>* v);
    ~LibDefine() {};
    void Write(ostream& out) const;
};


class LibGroup {
private:
    string groupName_;
    string name_;
    // common list to store elements of LibAttribute*, LibGroup* and LibDefine*
    vector<any>* statements_;
    // attributes_[name] = index into the statements_ vector (for fast lookup of attributes)
    unordered_map<string, int> attributes_;
public:
    LibGroup(string g=string(""), string n=string("")): groupName_(g), name_(n) {
        statements_ = new vector<any>(0);
    };
    ~LibGroup();
    void setGroupName(const string& g) { groupName_ = g; }
    string groupName() const { return groupName_; }
    void setName(const string& n) { name_ = n; }
    string name() const { return name_; }
    void setAttribute(const string& name, const any& value);
    any attribute (const string& name) const;
    void addLibGroup(LibGroup *g); // add a sub-group to this libgroup
    void addLibAttribute(LibAttribute *a); // add a sub-group to this libgroup
    void setStatements(vector<any>* statements) { delete statements_; statements_ = statements; };
    vector<any>* statements() const { return statements_; };
    void Write(ostream& out, const string& prefix="") const;
};

#endif

