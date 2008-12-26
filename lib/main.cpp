#include <QTextStream>
#include <QDebug>
#include "DotLib.h"

int main(int argc, char *argv[])
{
    DotLib l;
	if (!l.read(argv[1])) {
		qDebug() << QString("Error: Cant load dot lib file %1").arg(argv[1]);
	};
	l.write("/tmp/something.lib");
}

