#include <internal/config.h>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <iterator>
#include "Flags.hpp"

typedef unsigned char byte;
void show_help(char *program_name, std::ostream &ostream, Flags desc)
{
	ostream
		<< "embed-resource (ver. " << PROJECT_VERSION << ")" << std::endl
		<< "    Creates {output_file_name} (which will be .hpp type file) from the contents of {input_file_name}" << std::endl;
	desc.PrintHelp(program_name, ostream);
	ostream << std::endl
			<< "EXAMPLE:" << std::endl
			<< "    embed-resource.exe -i foo.txt -o foo.hpp" << std::endl;
}

std::vector<byte> read_file_into_vector(std::string file_name)
{
	std::ifstream ifs;
	ifs.open(file_name, std::ios::in | std::ios::binary);
	ifs.unsetf(std::ios::skipws);

	// get its size:
	std::streampos input_size;
	ifs.seekg(0, std::ios::end);
	input_size = ifs.tellg();
	ifs.seekg(0, std::ios::beg);

	auto buffer = std::vector<byte>();
	buffer.reserve(input_size);
	std::copy(std::istream_iterator<byte>(ifs), std::istream_iterator<byte>(), std::back_inserter(buffer));
	ifs.close();

	return buffer;
}

int main(int argc, char **argv)
{
	std::string input_file;
	std::string output_file;

	bool version;
	bool help;
	bool contentAsComent;

	Flags flags;

	flags.Var(input_file, 'i', "input", std::string(""), "Input file name");
	flags.Var(output_file, 'o', "output", std::string(""), "Output file name");
	flags.Bool(contentAsComent, 'c', "comments", "Inserts original content of the file into output as multi-line comments");
	flags.Bool(version, 'v', "version", "shows version and exit");
	flags.Bool(help, 'h', "help", "show this help and exit");

	if (!flags.Parse(argc, argv))
	{
		flags.PrintHelp(argv[0]);
		return 1;
	}
	else if (help)
	{
		flags.PrintHelp(argv[0]);
		return 0;
	}

	if (help)
	{
		show_help(argv[0], std::cout, flags);
		return EXIT_SUCCESS;
	}

	if (version)
	{
		std::cout << "embed-resource version " << PROJECT_VERSION << std::endl;
		return EXIT_SUCCESS;
	}

	if (input_file == "")
	{
		std::cerr << "ERROR: input file is not specified" << std::endl;
		show_help(argv[0], std::cerr, flags);
		return EXIT_FAILURE;
	}

	if (output_file == "")
	{
		std::cerr << "ERROR: output file is not specified" << std::endl;
		show_help(argv[0], std::cerr, flags);
		return EXIT_FAILURE;
	}

	std::string sym(input_file);
	std::replace(sym.begin(), sym.end(), '.', '_');
	std::replace(sym.begin(), sym.end(), '-', '_');
	std::replace(sym.begin(), sym.end(), '/', '_');
	std::replace(sym.begin(), sym.end(), '\\', '_');

	auto input_file_content = read_file_into_vector(input_file);

	std::ofstream ofs;
	ofs.open(output_file, std::ios::out | std::ios::binary);

	ofs << "#pragma once" << std::endl;
	ofs << "#include <EmbedResource.hpp>" << std::endl;
	ofs << "" << std::endl;

	if (contentAsComent)
	{
		ofs << "// Content of input file " << input_file << std::endl;
		ofs << "/***" << std::endl;
		std::copy(input_file_content.begin(), input_file_content.end(), std::ostream_iterator<byte>(ofs));
		ofs << std::endl
			<< "***/" << std::endl;
	}
	ofs << "const char _resource_" << sym << "[] = {" << std::endl;

	size_t lineCount = 0;

	for (int i = 0; i < input_file_content.size(); i++)
	{
		if (++lineCount == 20)
		{
			ofs << std::endl;
			lineCount = 0;
		}
		ofs << "0x" << std::hex << (input_file_content[i] & 0xff) << ", ";
	}

	ofs << std::endl
		<< "};" << std::endl;
	ofs << "const std::size_t _resource_" << sym << "_len = sizeof(_resource_" << sym << ");";

	ofs.close();

	return EXIT_SUCCESS;
}
