#include <iostream>
#include <fstream>
#include "tape.h"
#include "string"

int main(int argc, char *argv[]) {
	if (argc != 4 && argc != 5) {
		std::cerr << "Usage: sort_tape MaxMemory(in Kilobytes) InputFileName OutputFileName [latency_config]." << std::endl;
		return -1;
	}

	try {
		// Transform from Kilobytes into bytes.
		int max_allowed_memory = std::stoi(argv[1]) * 1024;

		std::string input_file_name(argv[2]);
		std::string output_file_name(argv[3]);

		std::ifstream input_file(input_file_name);
		std::ofstream output_file(output_file_name);
		if (!input_file) {
			throw std::invalid_argument("Unable to open input file " + input_file_name);
		} else if (!output_file) {
			throw std::invalid_argument("Unable to open output file " + output_file_name);
		}

		auto tape = Tape();

		if (argc == 5) {
			std::string config_file_name(argv[4]);
			std::ifstream config_file(config_file_name);
			if (!config_file) {
				throw std::invalid_argument("Unable to open config file " + config_file_name);
			}
			tape.read_config(config_file);
		}

		tape.sort(max_allowed_memory, input_file, output_file);
	} catch (std::invalid_argument &e) {
		std::cerr << e.what() << std::endl;
		return -1;
	} catch (EndOfInputFileException &e) {
		std::cerr << e.what() << std::endl;
		return -1;
	}
	return 0;
}
