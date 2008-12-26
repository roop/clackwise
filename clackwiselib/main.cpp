#include <QTextStream>
#include <QDebug>
#include "Commands.h"

int main(int argc, char *argv[])
{
    DotLib* dl = _read_lib(argv[1], "lib");
    dl = _get_lib(dl->name().toAscii().data(), "lib");
    _write_lib(dl, "/tmp/out.lib");
    _write_lib(dl->name().toAscii().data(), "lib", "/tmp/out2.lib");
}

