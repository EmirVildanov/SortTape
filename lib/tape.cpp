#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <thread>
#include "tape.h"
#include "json/json.h"

EndOfInputFileException::EndOfInputFileException(const std::string& message) : message_(message) {}

const char *EndOfInputFileException::what() {
	return message_.c_str();
}

const char *AvailableMemoryNotEnoughException::what() {
	return "Available memory is not enough to implement external sort";
}

/// Helper function to get location of temporary sorted blocks.
std::string get_tmp_file_name(int index) {
	return "/tmp/sorted_number_block_" + std::to_string(index);
}

/// Helper structure representing pointer to the sorted block (tmp file).
struct PointerValueInfo {
    /// Opened file stream.
    std::ifstream *file_pointer;
    /// Last number read from the file.
    int pointer_value;
    /// Flag is true in case there are no more values in the sorted block.
    bool finished;
};

/// Helper structure needed for merge of K sorted blocks.
struct PointersSortMinPair {
    /// Index of a sorted block which `pointer_value` is considered to be minimal.
    int index;
    /// Corresponding `pointer_value` of a sorted block.
    int value;
};

const std::string READ_WRITE_LATENCY_KEY = std::string("read_write_latency");
const int READ_WRITE_LATENCY_DEFAULT_VALUE = 0;
const std::string REWIND_LATENCY_KEY = std::string("rewind_latency");
const int REWIND_LATENCY_DEFAULT_VALUE = 0;
const std::string SHIFT_LATENCY_KEY = std::string("shift_latency");
const int SHIFT_LATENCY_DEFAULT_VALUE = 0;

Tape::Tape() : read_write_latency_(READ_WRITE_LATENCY_DEFAULT_VALUE),
               rewind_latency_(REWIND_LATENCY_DEFAULT_VALUE),
               shift_latency_(SHIFT_LATENCY_DEFAULT_VALUE) {}

void Tape::read_config(std::ifstream &config_file) {
	Json::Value root;
	config_file >> root;
	int config_read_write_latency = root
		.get(READ_WRITE_LATENCY_KEY, READ_WRITE_LATENCY_DEFAULT_VALUE)
		.asInt();
	int config_rewind_latency = root
		.get(REWIND_LATENCY_KEY, REWIND_LATENCY_DEFAULT_VALUE)
		.asInt();
	int config_shift_latency = root
		.get(SHIFT_LATENCY_KEY, SHIFT_LATENCY_DEFAULT_VALUE)
		.asInt();
	read_write_latency_ = config_read_write_latency;
	rewind_latency_ = config_rewind_latency;
	shift_latency_ = config_shift_latency;
}

std::optional<int> Tape::get_value_from_tape_or_error(std::ifstream &input_file) const {
	if (input_file.eof()) {
		return std::nullopt;
	}
	int value;
	std::this_thread::sleep_for(std::chrono::seconds(read_write_latency_));
	input_file >> value;
	std::this_thread::sleep_for(std::chrono::seconds(shift_latency_));
	return {value};
}

void Tape::write_value_to_the_tape(std::ofstream &output_file, int number) const {
	std::this_thread::sleep_for(std::chrono::seconds(read_write_latency_));
	output_file << number;
	std::this_thread::sleep_for(std::chrono::seconds(shift_latency_));
}

void Tape::sort(int max_memory_size, std::ifstream &input_file, std::ofstream &output_file) {
	if (input_file.peek() == EOF) {
		throw EndOfInputFileException("Input file is empty");
	}
	int number_of_elements = -1;
	std::optional<int> input_number_of_elements = get_value_from_tape_or_error(input_file);
	if (input_number_of_elements) {
		number_of_elements = *input_number_of_elements;
	} else {
		throw EndOfInputFileException("Unable to read number of elements in input file");
	}

	// Calculate memory that input elements take in bytes.
	// Because each element is an integer (2^32).
	int elements_memory_size = number_of_elements * ELEMENT_SIZE_IN_BYTES;
	// Calculate number of tmp blocks we'll need to create in order to sort all the input sequence.
	int number_of_sorted_blocks = ceil(
		static_cast<double>(elements_memory_size) / static_cast<double>(max_memory_size)
	);
	// Calculate number of values one tmp block can store.
	int max_elements_in_block = floor(
		static_cast<double>(max_memory_size) / static_cast<double>(ELEMENT_SIZE_IN_BYTES));
	if (max_elements_in_block == 0) {
		throw AvailableMemoryNotEnoughException();
	}

	// Check that we could sort calculated number of sorted blocks.
	// The logic is that we'd have to keep `max_elements_in_block` pointer values (see above)
	// in the available memory.
	// General condition for memory unavailability:
	// `max_memory_size^2 < number_of_elements * ELEMENT_SIZE_IN_BYTES^2`
	if (number_of_sorted_blocks > max_elements_in_block) {
		throw AvailableMemoryNotEnoughException();
	}


	// Fill sorted tmp blocks and write them on the disk.
	// TODO: implement in-memory sort in case there is just one sorted_block needed.
	int elements_read_counter = 0;
	for (int i = 0; i < number_of_sorted_blocks; i++) {
		std::vector<int> buffer;

		for (int j = 0; j < max_elements_in_block && elements_read_counter < number_of_elements; j++) {
			std::optional<int> current_number = get_value_from_tape_or_error(input_file);
			if (current_number) {
				buffer.push_back(*current_number);
				elements_read_counter++;
			} else {
				throw EndOfInputFileException("Unable to read number from the tape");
			}
		}
		std::sort(buffer.begin(), buffer.end());

		std::ofstream current_tmp_sorted_block(get_tmp_file_name(i));
		if (!current_tmp_sorted_block) {
			throw std::invalid_argument("Unable to generate test file");
		}
		for (size_t j = 0; j < buffer.size(); j++) {
			if (j != 0) {
				current_tmp_sorted_block << " ";
			}
			write_value_to_the_tape(current_tmp_sorted_block, buffer[j]);
		}
	}

	/// Opened sorted lists.
	std::vector<PointerValueInfo> pointers;
	for (int i = 0; i < number_of_sorted_blocks; i++) {
		std::string current_tmp_file_name = get_tmp_file_name(i);
		auto current_tmp_file = new std::ifstream(current_tmp_file_name);
		if (!(*current_tmp_file)) {
			throw std::invalid_argument("Unable to open tmp file with id = " + std::to_string(i));
		}
		std::this_thread::sleep_for(std::chrono::seconds(rewind_latency_));
		std::optional<int> first_number = get_value_from_tape_or_error(*current_tmp_file);
		if (first_number) {
			pointers.push_back({current_tmp_file, *first_number, false});
		} else {
			throw EndOfInputFileException("Unable to read number from sorted tmp block");
		}
	}

	for (int i = 0; i < number_of_elements; i++) {
		if (i != 0) {
			output_file << " ";
		}
		// Pair of index, value.
		std::optional<PointersSortMinPair> minimal_element = std::nullopt;

		for (int j = 0; j < number_of_sorted_blocks; j++) {
			PointerValueInfo current_info = pointers[j];
			if (current_info.finished) {
				continue;
			}
			if (minimal_element) {
				int current_min_value = (*minimal_element).value;
				if (current_info.pointer_value < current_min_value) {
					minimal_element = {PointersSortMinPair{j, current_info.pointer_value}};
				}
			} else {
				minimal_element = {PointersSortMinPair{j, current_info.pointer_value}};
			}
		}
		if (minimal_element) {
			int current_min_value = (*minimal_element).value;
			int min_index = (*minimal_element).index;

			write_value_to_the_tape(output_file, current_min_value);

			std::optional<int> next_value = get_value_from_tape_or_error(
				(*pointers[min_index].file_pointer));
			if (next_value) {
				pointers[min_index].pointer_value = *next_value;
			} else {
				pointers[min_index].finished = true;
			}
		}
	}

	for (auto & info : pointers) {
		if (info.file_pointer != nullptr) {
			delete info.file_pointer;
		}
	}
}
