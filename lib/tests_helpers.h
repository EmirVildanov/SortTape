#pragma once

#include <string>
#include <fstream>

// Helper function to generate input tape with N numbers into the given file.
void generate_random_shuffled_array(int N, std::ofstream &output_file);