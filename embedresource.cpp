#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include "getopt.h"
#include "embededresource.h"

void show_help(std::ostream& ostream)
{
	ostream
		<< "embed-resource (ver. " << PROJECT_VERSION << ")"<< std::endl
		<< "    Creates {output_file_name} (which will be .hpp type file) from the contents of {input_file_name}" << std::endl
		<< "USAGE:"<< std::endl
		<< "    embed-resource[.exe] -i {input_file_name} -o {output_file_name}" << std::endl 		
		<< "EXAMPLE:" << std::endl
		<< "    embed-resource.exe -i foo.txt -o foo.hpp" << std::endl;
}

int main(int argc, char** argv)
{	

	int c;
	std::string input_file;
	std::string output_file;
	while ((c = getopt(argc, argv, "vhi:o:")) != -1)
	{
		switch (c)
		{
		case 'o':
			output_file = optarg;
			break;
		case 'i':
			input_file = optarg;
			break;
		case 'h':
			show_help(std::cout);
			return EXIT_SUCCESS;
		case 'v':
			std::cout << "embed-resource version " << PROJECT_VERSION << std::endl;
			return EXIT_SUCCESS;
		case '?':
			if (optopt == 'o')
			{
				fprintf(stderr, "Option -%c requires an argument.\n", optopt);
			}

			else if (isprint(optopt))
			{
				fprintf(stderr, "Unknown option `-%c'.\n", optopt);
			}
			else
			{
				fprintf(stderr,
					"Unknown option character `\\x%x'.\n",
					optopt);
			}

			return EXIT_FAILURE;
		default:
			abort();
		}
	}

	if (input_file.empty())
	{
		std::cerr << "ERROR: input file is not specified" << std::endl;
		show_help(std::cerr);
		return EXIT_FAILURE;
	}

	if (output_file.empty())
	{
		std::cerr << "ERROR: output file is not specified" << std::endl;
		show_help(std::cerr);
		return EXIT_FAILURE;
	}

	std::string sym(input_file);
	std::replace(sym.begin(), sym.end(), '.', '_');
	std::replace(sym.begin(), sym.end(), '-', '_');
	std::replace(sym.begin(), sym.end(), '/', '_');
	std::replace(sym.begin(), sym.end(), '\\', '_');

	std::ifstream ifs;
	ifs.open(input_file);

	std::ofstream ofs;
	ofs.open(output_file);

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
