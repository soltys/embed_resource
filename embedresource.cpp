#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <embededresource.h>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
void show_help(std::ostream& ostream, po::options_description desc)
{
	ostream
		<< "embed-resource (ver. " << PROJECT_VERSION << ")" << std::endl
		<< "    Creates {output_file_name} (which will be .hpp type file) from the contents of {input_file_name}" << std::endl
		<< desc << std::endl
		<< "EXAMPLE:" << std::endl
		<< "    embed-resource.exe -i foo.txt -o foo.hpp" << std::endl;
}

int main(int argc, char** argv)
{

	po::options_description desc("embed-resource[.exe] usage");
	desc.add_options()
		("input,i", po::value<std::string>(), "input file")
		("output,o", po::value<std::string>(), "output file")
		("version,v", "shows version of the application")
		("help,h", "shows help")
		;

	po::variables_map vm;
	store(po::parse_command_line(argc, argv, desc), vm);
	notify(vm);

	if(!vm["help"].empty())
	{
		show_help(std::cout, desc);
		return EXIT_SUCCESS;
	}

	if(!vm["version"].empty())
	{
		std::cout << "embed-resource version " << PROJECT_VERSION << std::endl;
		return EXIT_SUCCESS;
	}

	std::string input_file;
	std::string output_file;

	if (vm["input"].empty())
	{
		std::cerr << "ERROR: input file is not specified" << std::endl;
		show_help(std::cerr, desc);
		return EXIT_FAILURE;
	}
	input_file = vm["input"].as<std::string>();

	if (vm["output"].empty())
	{
		std::cerr << "ERROR: output file is not specified" << std::endl;
		show_help(std::cerr, desc);
		return EXIT_FAILURE;
	}
	output_file = vm["output"].as<std::string>();

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
	ofs << "" << std::endl;
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

	ofs << std::endl << "};" << std::endl;
	ofs << "const std::size_t _resource_" << sym << "_len = sizeof(_resource_" << sym << ");";

	ofs.close();
	ifs.close();

	return EXIT_SUCCESS;
}
