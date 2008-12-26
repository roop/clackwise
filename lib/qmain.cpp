#include <QTextStream>
#include <QDebug>
#include "LibGroup.h"
#include "liberty.h"

int main(int argc, char *argv[]) {
    LibGroup library;
    library.setType("library");
    library.setName("BU110");
    library.setSimpleAttribute("simple", "mary");
    library.setSimpleAttribute("simon", "sez");
    library.setComplexAttribute("complex", "\"3+7i\"");
    library.setComplexAttribute("lakshmi_complex", "cross", "cut", "road");
    library.setComplexAttribute("complex", "\"4+2i\"");
    library.setMultivaluedAttribute("define", "prickiness", "pin", "float");
    library.setMultivaluedAttribute("define", "is_port", "pin", "boolean");
    library.setMultivaluedAttribute("define", "is_hierarchical", "cell", "boolean");
	LibGroup buf("cell", "bu110");
	buf.setSimpleAttribute("area", "5");
    buf.setComplexAttribute("complex", "\"5+27i\"");
	LibGroup a("pin", "a");
	a.setSimpleAttribute("direction", "in");
	LibGroup y("pin", "y");
	y.setSimpleAttribute("direction", "out");
	buf.insertSubgroup(buf.subgroupsCount(), &a);
	buf.insertSubgroup(buf.subgroupsCount(), &y);
	library.insertSubgroup(library.subgroupsCount(), &buf);;

	QTextStream qout(stdout);
	// qout << library.toText();
	LibGroup* lg = parseLib(argv[1]);
	if (lg) {
		qDebug() << lg->name() << " " << lg->type();
		qout << lg->toText();
	}
}

