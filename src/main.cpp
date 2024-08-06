#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

int main(int argc, char** argv) {
	if (argc != 3 || std::strcmp(argv[1], "-c") != 0) {
		std::cout << "Usage: \n $ " << argv[0] << " -c INPUT_FILE" << std::endl;
		return 1;
	}

	try {
		char* filename = argv[2];
		std::ifstream file(filename);
		if (!file.is_open()) {
			std::cout << "Error opening file " << filename << std::endl;
			return 1;
		}

		size_t count = 0;
		while (EOF != file.get())
			count++;

		if (file.eof()) {
			std::cout << count << " " << filename << std::endl;
		} else if (file.fail()) {
			std::cerr << "failed to read file " << filename << std::endl;
			return 1;
		}

		file.close();
	} catch (const std::bad_alloc& e) {
		std::cout << "Memory allocation failed: " << e.what() << std::endl;
        	std::cout << "The input might be too large to process." << std::endl;
        	return 1;
	} catch (const std::exception& e) {
		std::cout << "An unexpected error occurred: " << e.what() << std::endl;
        	return 1;
    	}

	return 0;
}
