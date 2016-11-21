#include "gencc.h"
#include "helper.h"

#include <iostream>

int main(int argc, char* argv[])
{
    HelperImpl helper;
    Gencc gencc(&helper);

    std::vector<std::string> params;
    for (int i = 0; i < argc; ++i) {
        params.emplace_back(argv[i]);
    }

    try {
        return gencc.init(params);
    } catch (const std::exception& ex) {
        std::cout << "ERROR: " << ex.what() << '\n';
        return -1;
    }
}
