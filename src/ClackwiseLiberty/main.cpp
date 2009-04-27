#include <QTextStream>
#include <QDebug>
#include "Commands.h"

using namespace Clackwise;

int main(int argc, char *argv[])
{
    Lib* dl = read_lib(QString(argv[1]));
    dl = get_lib(dl->name());
    write_lib(dl, "/tmp/out.lib");
}

