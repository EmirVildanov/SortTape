#pragma once

#include <optional>
#include <string>

/// Exception to throw in case unexpected end of file met.
class EndOfInputFileException : public std::exception {
public:
    explicit EndOfInputFileException(const std::string& message);
    const char *what();
private:
    std::string message_;
};

/// Exception to throw in case there is no available memory to implement external sort.
class AvailableMemoryNotEnoughException : public std::exception {
public:
    const char *what();
};

/// Size of each element on an input tape. Used to calculate the size input tape takes.
const int ELEMENT_SIZE_IN_BYTES = 4;

class Tape {
public:
    Tape();

    /// Sort integer values from an input file into an output file.
    void sort(int, std::ifstream &, std::ofstream &);

    /**
     * Update latency fields from config file.
     * Note: all the values are passed in seconds.
     */
    void read_config(std::ifstream &);

private:
    /// Latency of read/write of element from a tape.
    int read_write_latency_;
    /// Latency of rewinding the tape to the desired location.
    int rewind_latency_;
    /// Latency of shifting tape head on one position.
    int shift_latency_;

    /// Helper function tot retrieve integer number from an input tape. Returns None in case EOF is met.
    std::optional<int> get_value_from_tape_or_error(std::ifstream &input_file) const;

    /// Write number to the output file.
    void write_value_to_the_tape(std::ofstream &output_file, int number) const;
};