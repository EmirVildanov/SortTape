#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>

void generate_random_shuffled_array(int N, std::ofstream &output_file) {
	std::vector<int> numbers_sequence;
	numbers_sequence.reserve(N);
	for (int i = 0; i < N; i++) {
		numbers_sequence.push_back(i);
	}
	std::shuffle(std::begin(numbers_sequence), std::end(numbers_sequence), std::mt19937(std::random_device()()));
	output_file << N << std::endl;
	for (int i = 0; i < N; i++) {
		if (i != 0) {
			output_file << " ";
		}
		output_file << numbers_sequence[i];
	}
}
