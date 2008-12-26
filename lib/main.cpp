#include <QTextStream>
#include <QDebug>
#include "LibGroup.h"
#include "liberty.h"

int main(int argc, char *argv[])
{
    LibGroup lg;
	if (!lg.readDotLib(argv[1])) {
		qDebug() << QString("Error: Cant load dot lib file %1").arg(argv[1]);
	};
    qDebug() << lg.toText();
}

