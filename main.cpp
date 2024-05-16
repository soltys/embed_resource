#include <internal/config.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <iterator>
#include "args.hxx"

typedef unsigned char byte;

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
	bool contentAsComent;

	args::ArgumentParser parser(PROJECT_DESCRIPTION);
	args::HelpFlag help_flag(parser, "help", "Display this help menu", {'h', "help"});
	args::Flag version_flag(parser, "version", "shows version and exit", {'v', "version"});
	args::Flag contentAsComent_flag(parser, "comments", "Inserts original content of the file into output as multi-line comments", {'c', "comments"});
	args::ValueFlag<std::string> input_file_flag(parser, "input", "input file to be processed", {'i', "input"});
	args::ValueFlag<std::string> output_file_flag(parser, "output", "output file name", {'o', "output"});

	try
	{
		parser.ParseCLI(argc, argv);
	}
	catch (args::Help)
	{
		std::cout << parser;
		return EXIT_SUCCESS;
	}
	catch (args::ParseError e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return EXIT_FAILURE;
	}
	catch (args::ValidationError e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return EXIT_FAILURE;
	}

	if (version_flag)
	{
		std::cout << "embed-resource version " << PROJECT_VERSION << std::endl;
		return EXIT_SUCCESS;
	}

	if (!input_file_flag)
	{
		std::cerr << "ERROR: input file is not specified" << std::endl;
		std::cerr << parser;
		return EXIT_FAILURE;
	}
	input_file = args::get(input_file_flag);

	if (!output_file_flag)
	{
		std::cerr << "ERROR: output file is not specified" << std::endl;
		std::cerr << parser;
		return EXIT_FAILURE;
	}
	output_file = args::get(output_file_flag);

	contentAsComent = args::get(contentAsComent_flag);
	
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
		if (lineCount++ == 25)
		{
			ofs << std::endl;
			lineCount = 1;
		}
		ofs << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << (input_file_content[i] & 0xff) << ", ";
	}

	ofs << std::endl
		<< "};" << std::endl;
	ofs << "const std::size_t _resource_" << sym << "_len = sizeof(_resource_" << sym << ");";

	ofs.close();

	return EXIT_SUCCESS;
}
