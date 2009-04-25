#include <QTextStream>
#include <QDebug>
#include "Commands.h"

using namespace Clackwise;

int main(int argc, char *argv[])
{
    DotLib* dl = read_lib(argv[1]);
    dl = get_lib(dl->name().toAscii().data());
    write_lib(dl, "/tmp/out.lib");
}

