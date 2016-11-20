#include "gencc.h"
#include "helper.h"

#include <iostream>

int main(int argc, char* argv[])
{
    GenccOptions options;
    Helper helper;
    Gencc gencc(&options, &helper);

    try {
        return gencc.init(argc, argv);
    } catch (const std::exception& ex) {
        std::cout << "ERROR: " << ex.what() << '\n';
        return -1;
    }
}
