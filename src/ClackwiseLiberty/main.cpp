#include <QTextStream>
#include <QDebug>
#include "Commands.h"

int main(int argc, char *argv[])
{
    DotLib* dl = read_lib(argv[1], "lib");
    dl = get_lib(dl->name().toAscii().data(), "lib");
    write_lib(dl, "/tmp/out.lib");
    write_lib(dl->name().toAscii().data(), "lib", "/tmp/out2.lib");
}

