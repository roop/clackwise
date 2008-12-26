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

#include <iostream>
#include <vector>
#include <tr1/unordered_map>
#include <boost/any.hpp>
#include "LibData.h"

using namespace std;
using namespace std::tr1;
using namespace boost;

// Lib Attribute

void LibAttribute::Write(ostream& out) const {
    out << name << " : ";
    WriteLibAttributeValue(out, value);
    out << ";" << endl;
};

LibAttribute::~LibAttribute() {
    if (value.type() == typeid(vector<string>*)) {
        delete any_cast<vector<string>*>(value);
    }
};

// Define statements

LibDefine::LibDefine(const string& d, const vector<string>* v) {
    if (d == "define") {
        if (v->size() < 3) {
            type = LibDefine::Invalid;
        } else {
            name = v->at(0);
            group = v->at(1);
            type = getAttrType(v->at(2));
        }
    } else if (d == "define_group") {
          if (v->size() < 2) {
            type = LibDefine::Invalid;
        } else {
            name = v->at(0);
            group = v->at(1);
            type = LibDefine::Group;
        }
    }
};

LibDefine::Type LibDefine::getAttrType(const string& s) {
    if (s == "boolean") {
        return LibDefine::Boolean;
    } else if (s == "string") {
        return LibDefine::String;
    } else if (s == "integer") {
        return LibDefine::Integer;
    } else if (s == "float") {
        return LibDefine::Float;
    } else {
        return LibDefine::Invalid;
    }
};

void LibDefine::Write(ostream& out) const {
    if (type == LibDefine::Invalid) {
      out << "/* Malformed define statement was skipped */" << endl;
    } else if (type == LibDefine::Group) {
      out << "define_group(" << name << ", " << group << ");" << endl;
    } else {
      out << "define(" << name << ", " << group << ", ";
      switch(type) {
        case LibDefine::Boolean: out << "boolean"; break;
        case LibDefine::String:  out << "string"; break;
        case LibDefine::Integer: out << "integer"; break;
        case LibDefine::Float:   out << "float"; break;
        default:                 out << "string"; break;
      }
      out << ");" << endl;
    }
};

// Lib Group

void LibGroup::setAttribute(const string& name, const any& value) {
  // first, look up in our attributes_ hash
  unordered_map<string, int>::iterator where = attributes_.find(name);
  if (where != attributes_.end()) {
    // if found, index into statements_ array using the result of the hashlookup
    (any_cast<LibAttribute*>(statements_->at(where->second)))->value = value;
  } else {
    // if not found, add an attribute to the statements_ array
    statements_->push_back(new LibAttribute(name, value));
    // and store the index in our attributes_ hash
    attributes_[name] = statements_->size() - 1;
  }
};

any LibGroup::attribute(const string& name) const {
    return any_cast<LibAttribute*>(statements_->at(attributes_.find(name)->second))->value;
};

void LibGroup::addLibGroup(LibGroup *g) {
    statements_->push_back(g);
};

void LibGroup::addLibAttribute(LibAttribute *a) {
    statements_->push_back(a);
    attributes_[a->name] = statements_->size() - 1;
};

void LibGroup::Write(ostream& out, const string& prefix) const {
    out << prefix << groupName() << "(" << name() << ")" << " {" << endl;
    for ( vector<any>::iterator it = statements_->begin();
          it != statements_->end(); ++it ) {
        if (it->type() == typeid(LibAttribute*)) {
          LibAttribute* attr = any_cast<LibAttribute*>(*it);
          if (attr) {
              out << prefix << "    ";
              attr->Write(out);
          }
        } else if (it->type() == typeid(LibDefine*)) {
          LibDefine* define = any_cast<LibDefine*>(*it);
          if (define) {
              out << prefix << "    ";
              define->Write(out);
          }
        } else if (it->type() == typeid(LibGroup*)) {
          LibGroup* group = any_cast<LibGroup*>(*it);
          if (group)
              group->Write(out, prefix + "    ");
        } else {
          out << prefix << "/* <unknown_obj> */";
        }
    }
    out << prefix << "}" << endl;
}

LibGroup::~LibGroup() {
    for ( vector<any>::iterator it = statements_->begin();
          it != statements_->end(); ++it ) {
        if (it->type() == typeid(LibAttribute*)) {
          LibAttribute* attr = any_cast<LibAttribute*>(*it);
          delete attr;
        } else if (it->type() == typeid(LibGroup*)) {
          LibGroup* group = any_cast<LibGroup*>(*it);
          delete group;
        }
    }
    delete statements_;
};

// Function

void WriteLibAttributeValue(ostream& out, const any& value) {
    if (value.type() == typeid(string)) {
        out << any_cast<string>(value);
    } else if (value.type() == typeid(vector<string>*)) {
        out << " (";
        vector<string>* l = any_cast<vector<string>*>(value);
        for ( vector<string>::iterator it = l->begin();
              it != l->end(); ++it ) {
            if (it > l->begin()) out << ", ";
            out << *it;
        }
        out << ")";
    } else {
        cout << "<unknown_obj>";
    }
}

// Sample calls

/*
int main() {
    LibGroup* l = new LibGroup("library", "18nm.db");
    l->setAttribute("technology", new vector<any>(1, string("cmos")));
    l->addLibAttribute(new LibAttribute("delay_model", string("table_lookup")));
    l->setAttribute("library_features", new vector<any>(1, string("report_delay_calculation")));
    l->setAttribute("revision", string("\"1.0\""));
    l->setAttribute("nom_process", double(1.500000));
    LibGroup* op;
    l->addLibGroup(op = new LibGroup("operating_conditions", "\"OPCOND_-1\""));
    op->setAttribute("process", string("1.500000"));
    op->setAttribute("voltage", string("4.750000"));
    op->setAttribute("temparature", string("70.000000"));
    l->setAttribute("default_operating_conditions", string("\"OPCOND_-1\""));
    l->Write(cout);
    op->setStatements(new vector<any>(1, new LibAttribute("process", string("1.500000"))));
    l->Write(cout);
    delete l;
};
*/

/*
set a [get_libs A.db]
    = return LIB.FindLibGroup("A.db");
set andgate [get_lib_pins A.db/AN210/A*]
    = LIB.FindLibGroup("A.db").FindLibGroup("cell").Find("AN210").FindLibGroup("pin").Find("*");
get_attribute $andgate direction
    = andgate[0].FindAttribute("direction").value

lib = [
   {
     name => "A.db",



*/
