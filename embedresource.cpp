#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cerr << "USAGE: " << argv[0] << " {sym} {rsrc}" << std::endl << std::endl
                          << "  Creates {sym}.cpp from the contents of {rsrc}"  << std::endl;
        return EXIT_FAILURE;
    }

    std::string sym(argv[2]);
    std::replace(sym.begin(), sym.end(), '.', '_');
    std::replace(sym.begin(), sym.end(), '-', '_');
    std::replace(sym.begin(), sym.end(), '/', '_');
    std::replace(sym.begin(), sym.end(), '\\', '_');

    std::ifstream ifs;
    ifs.open(argv[2]);

    std::ofstream ofs;
    ofs.open(argv[1]);

    ofs << "#pragma once" << std::endl;
    ofs << "#include \"Resource.h\"" << std::endl;
    ofs << "#include <cstddef>" << std::endl;
    ofs << "const char _resource_" << sym << "[] = {" << std::endl;

    size_t lineCount = 0;
    while (true)
    {
        char c;
        ifs.get(c);
        if (ifs.eof())
            break;

        ofs << "0x" << std::hex << (c & 0xff) << ", ";
        if (++lineCount == 20) {
            ofs << std::endl;
            lineCount = 0;
        }
    }

    ofs << "};" << std::endl;
    ofs << "const std::size_t _resource_" << sym << "_len = sizeof(_resource_" << sym << ");";

    ofs.close();
    ifs.close();

    return EXIT_SUCCESS;
}
