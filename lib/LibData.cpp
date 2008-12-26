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


void LibAttribute::Write(ostream& out) {
    out << name;
    WriteLibAttributeValue(out, value);
    out << ";" << endl;
};

LibAttribute::~LibAttribute() {
    if (value.type() == typeid(vector<any>*)) {
        delete any_cast<vector<any>*>(value);
    }
};


void LibGroup::setAttribute(string name, any value) {
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

any LibGroup::attribute(string name) {
    return any_cast<LibAttribute*>(statements_->at(attributes_[name]))->value;
};

void LibGroup::addLibGroup(LibGroup *g) {
    statements_->push_back(g);
};

void LibGroup::addLibAttribute(LibAttribute *a) {
    statements_->push_back(a);
    attributes_[a->name] = statements_->size() - 1;
};

void LibGroup::Write(ostream& out, string prefix) {
    out << prefix << groupName() << "(" << name() << ")" << " {" << endl;
    for ( vector<any>::iterator it = statements_->begin();
          it != statements_->end(); ++it ) {
        if (it->type() == typeid(LibAttribute*)) {
          LibAttribute* attr = any_cast<LibAttribute*>(*it);
          if (attr) {
              out << prefix << "    ";
              attr->Write(out);
          }
        } else if (it->type() == typeid(LibGroup*)) {
          LibGroup* group = any_cast<LibGroup*>(*it);
          if (group)
              group->Write(out, prefix + "    ");
        } else {
          out << prefix << "<unknown_obj>";
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

void WriteLibAttributeValue(ostream& out, any value, string prefix) {
    if (value.type() == typeid(int)) {
        cout << prefix << any_cast<int>(value);
    } else if (value.type() == typeid(double)) {
        cout << prefix << any_cast<double>(value);
    } else if (value.type() == typeid(string)) {
        cout << prefix << any_cast<string>(value);
    } else if (value.type() == typeid(vector<any>*)) {
        cout << " (";
        vector<any>* vl = any_cast<vector<any>*>(value);
        for ( vector<any>::iterator it = vl->begin();
              it != vl->end(); ++it ) {
            if (it > vl->begin()) cout << ", ";
            WriteLibAttributeValue(out, *it, "");
        }
        cout << ")";
    } else {
        cout << "<unknown_obj>";
    }
}
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
