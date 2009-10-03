#include <QTextStream>
#include <QDebug>
#include "CwCommands.h"

using namespace Clackwise;

int main(int argc, char *argv[])
{
    CwLib* dl = cw_read_lib(QString(argv[1]));
    dl = cw_get_libs(dl->name(), QRegExp::FixedString).at(0);
    CwLibGroup *inv1x = dl->subgroupsByName("cell", "inv1x", QRegExp::FixedString).at(0);
    qDebug() << "name:" << inv1x->name();
    qDebug() << "fullname:" << inv1x->fullName();
    cw_write_lib(dl, "/tmp/out.lib");
}

