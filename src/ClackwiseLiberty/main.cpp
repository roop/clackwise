#include <QTextStream>
#include <QDebug>
#include "CwCommands.h"

using namespace Clackwise;

int main(int argc, char *argv[])
{
    CwLib* dl = read_lib(QString(argv[1]));
    dl = cw_get_lib(dl->name());
    write_lib(dl, "/tmp/out.lib");
}

