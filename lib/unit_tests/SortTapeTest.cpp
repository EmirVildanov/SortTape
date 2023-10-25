#include <fstream>
#include <string>
#include "gtest/gtest.h"
#include "tape.h"

/// Base directory for input files. Note: tests_run file should be executed from /lib directory.
const std::string BASE_SAMPLES_DIRECTORY = "./unit_tests/samples/";
const std::string INPUT_FILE_NAME = "input.txt";
const std::string OUTPUT_FILE_NAME = "output.txt";

const int DEFAULT_AVAILABLE_MEMORY = 500;

/// Helper function to get the location of tests input/output files.
std::string get_test_location_for_N(int N) {
	return BASE_SAMPLES_DIRECTORY + std::to_string(N) + "_elements/";
}

class SortTapeFixture : public ::testing::Test {
public:
    void sort_for_given_input_output_paths(int available_memory, std::string input_path, std::string output_path) {
	    std::ifstream input_file(input_path);
	    if (!input_file) {
		    throw std::invalid_argument("Unable to open test input file:" + input_path);
	    }
	    std::ofstream output_file(output_path);
	    if (!output_file) {
		    throw std::invalid_argument("Unable to open test output file:" + output_path);
	    }

	    auto tape = Tape();

	    tape.sort(available_memory, input_file, output_file);
    }

    void test_sorted_sequence(int available_memory_bytes, int N) {
	    std::string input_file_name = get_test_location_for_N(N) + INPUT_FILE_NAME;
	    std::string output_file_name = get_test_location_for_N(N) + OUTPUT_FILE_NAME;
	    sort_for_given_input_output_paths(available_memory_bytes, input_file_name, output_file_name);

	    std::ifstream sorted_file(output_file_name);
	    if (!sorted_file) {
		    throw std::invalid_argument("Unable to open test output file for reading:" + output_file_name);
	    }

	    int current_sorted_number;
	    for (int i = 0; i < N; i++) {
		    sorted_file >> current_sorted_number;

		    EXPECT_EQ(i, current_sorted_number);
	    }
    }
};

TEST_F(SortTapeFixture, test_empty_input_throws) {
	std::string folder_name = "empty_input/";
	std::string input_file_name = BASE_SAMPLES_DIRECTORY + folder_name + INPUT_FILE_NAME;
	std::string output_file_name = BASE_SAMPLES_DIRECTORY + folder_name + OUTPUT_FILE_NAME;
	EXPECT_THROW(sort_for_given_input_output_paths(DEFAULT_AVAILABLE_MEMORY, input_file_name, output_file_name), EndOfInputFileException);
}

TEST_F(SortTapeFixture, test_no_input_numbers_throws) {
	std::string folder_name = "no_input_numbers/";
	std::string input_file_name = BASE_SAMPLES_DIRECTORY + folder_name + INPUT_FILE_NAME;
	std::string output_file_name = BASE_SAMPLES_DIRECTORY + folder_name + OUTPUT_FILE_NAME;
	EXPECT_THROW(sort_for_given_input_output_paths(DEFAULT_AVAILABLE_MEMORY, input_file_name, output_file_name), EndOfInputFileException);
}

TEST_F(SortTapeFixture, test_small_available_memory_throws) {
	int elements_number = ELEMENT_SIZE_IN_BYTES * ELEMENT_SIZE_IN_BYTES;
	int available_memory = ELEMENT_SIZE_IN_BYTES * ELEMENT_SIZE_IN_BYTES - 1;
	EXPECT_THROW(test_sorted_sequence(available_memory, elements_number), AvailableMemoryNotEnoughException);
}

TEST_F(SortTapeFixture, test_with_negative_numbers) {
	std::string folder_name = "with_negative_numbers/";
	std::string input_file_name = BASE_SAMPLES_DIRECTORY + folder_name + INPUT_FILE_NAME;
	std::string output_file_name = BASE_SAMPLES_DIRECTORY + folder_name + OUTPUT_FILE_NAME;
	sort_for_given_input_output_paths(DEFAULT_AVAILABLE_MEMORY, input_file_name, output_file_name);

	std::ifstream sorted_file(output_file_name);
	if (!sorted_file) {
		throw std::invalid_argument("Unable to open test output file for reading:" + output_file_name);
	}

	int size = 7;
	int expected_numbers[] = { -100, -42, -5, 0, 7, 12, 64};
	int current_sorted_number;
	for (int i = 0; i < size; i++) {
		sorted_file >> current_sorted_number;
		EXPECT_EQ(expected_numbers[i], current_sorted_number);
	}
}

TEST_F(SortTapeFixture, test_0_numbers) {
	test_sorted_sequence(DEFAULT_AVAILABLE_MEMORY, 10);
}

TEST_F(SortTapeFixture, test_10_numbers) {
	test_sorted_sequence(DEFAULT_AVAILABLE_MEMORY, 10);
}

TEST_F(SortTapeFixture, test_1000_numbers) {
	test_sorted_sequence(DEFAULT_AVAILABLE_MEMORY, 1000);
}

TEST_F(SortTapeFixture, test_5000_numbers) {
	test_sorted_sequence(DEFAULT_AVAILABLE_MEMORY, 5000);
}

TEST_F(SortTapeFixture, test_10000_numbers) {
	test_sorted_sequence(DEFAULT_AVAILABLE_MEMORY, 10000);
}