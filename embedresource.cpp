#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <embededresource.h>
#include <boost/program_options.hpp>
#include <vector>

typedef char byte;
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
		("comment,c", "embeded file content as multiline c comment")
		("version,v", "shows version of the application")
		("help,h", "shows help")
		;

	po::variables_map vm;
	store(po::parse_command_line(argc, argv, desc), vm);
	notify(vm);

	if (!vm["help"].empty())
	{
		show_help(std::cout, desc);
		return EXIT_SUCCESS;
	}

	if (!vm["version"].empty())
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

	bool showContentAsComment = false;
	if (!vm["comment"].empty())
	{
		showContentAsComment = true;
	}

	std::string sym(input_file);
	std::replace(sym.begin(), sym.end(), '.', '_');
	std::replace(sym.begin(), sym.end(), '-', '_');
	std::replace(sym.begin(), sym.end(), '/', '_');
	std::replace(sym.begin(), sym.end(), '\\', '_');

	std::ifstream ifs;
	ifs.open(input_file, std::ios::in | std::ios::binary);
	ifs.unsetf(std::ios::skipws);

	std::ofstream ofs;
	ofs.open(output_file, std::ios::out | std::ios::binary);

	ofs << "#pragma once" << std::endl;
	ofs << "#include \"Resource.h\"" << std::endl;
	ofs << "" << std::endl;

	// get its size:
	std::streampos input_size;
	ifs.seekg(0, std::ios::end);
	input_size = ifs.tellg();
	ifs.seekg(0, std::ios::beg);

	auto buffer = std::vector<byte>();
	buffer.reserve(input_size);
	std::copy(std::istream_iterator<byte>(ifs), std::istream_iterator<byte>(), std::back_inserter(buffer));	
	ifs.close();

	if (showContentAsComment)
	{
		ofs << "// Content of input file " << input_file << std::endl;
		ofs << "/***" << std::endl;
		std::copy(buffer.begin(), buffer.end(), std::ostream_iterator< byte >(ofs));
		ofs << std::endl 
			<< "***/" << std::endl;
	}
	ofs << "const char _resource_" << sym << "[] = {" << std::endl;

	size_t lineCount = 0;

	for (int i = 0; i < buffer.size(); i++) {
		if (++lineCount == 20) {
			ofs << std::endl;
			lineCount = 0;
		}
		ofs << "0x" << std::hex << (buffer[i] & 0xff) << ", ";
	}

	ofs << std::endl << "};" << std::endl;
	ofs << "const std::size_t _resource_" << sym << "_len = sizeof(_resource_" << sym << ");";

	ofs.close();

	return EXIT_SUCCESS;
}
