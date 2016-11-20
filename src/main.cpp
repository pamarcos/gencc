#include "gencc.h"

int main(int argc, char* argv[])
{
    GenccOptions options;
    Helper helper;
    Gencc gencc(&options, &helper);
    return gencc.init(argc, argv);
}
