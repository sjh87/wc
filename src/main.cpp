#include <array>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <stdexcept>

#define C_FLAG 0b1000
#define L_FLAG 0b0100
#define M_FLAG 0b0010
#define W_FLAG 0b0001

class BadFlagException : public std::exception {
	std::string message;

public:
	BadFlagException(const std::string& message): message(message) {}
	virtual ~BadFlagException() = default;

	const char* what() const noexcept override {
		return message.c_str();
	}
};

u_int8_t collectFlags(int argc, char** argv) {
	u_int8_t flags{0};

	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			for (int j = 0; argv[i][j]; j++) {
				switch (argv[i][j]) {
					case 'c':
						flags |= C_FLAG;
						break;
					case 'l':
						flags |= L_FLAG;
						break;
					case 'm':
						flags |= M_FLAG;
						break;
					case 'w':
						flags |= W_FLAG;
						break;
					case '-':
						if (j == 0) {
							break;
						}
						
						throw BadFlagException("\"-\" is not a valid flag");
					default:
						std::stringstream what;
						what << "flag " << argv[i][j] << " is not recognized";

						throw BadFlagException(what.str());
				}
			}
		}
	}

	return flags ? flags : C_FLAG | L_FLAG | W_FLAG;
}

void collectFilenames(int argc, char** argv, std::set<std::string>& filenames) {
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			filenames.insert(std::string(argv[i]));
		}
	}
}

size_t countWords(std::string& line) {
	size_t count{0};
	bool inWord{false};
	
	for (char& c : line) {
		if (std::isspace(c)) {
			inWord = false;
		} else if (!inWord) {
			inWord = true;
			count++;
		}
	}

	return count;
}

struct Counts {
	size_t c;
	size_t l;
	size_t m;
	size_t w;
};

int main(int argc, char** argv) {
	try {
		u_int8_t flags = collectFlags(argc, argv);
		std::set<std::string> filenames;
		collectFilenames(argc, argv, filenames);

		std::map<std::string, Counts> countsPerFile;

		for (const auto &filename : filenames) {
			std::ifstream file(filename);
			Counts counts{};

			if (!file.is_open()) {
				std::cout << "failed to open file \"" << filename << "\"" << std::endl;
				return 1;
			}

			std::string line;
			while (std::getline(file, line)) {
				counts.l++;
				counts.w += countWords(line);
			}

			if (file.bad()) {
				if (file.eof()) {
					counts.c = file.gcount();
					countsPerFile.insert({ filename, counts });
					continue;
				}

				std::cout << "could not read data in " << filename << std::endl;

				return 1;
			}
		}

		for (const auto& [filename, counts] : countsPerFile) {
			std::stringstream outputLine;

			if (L_FLAG && flags) {
				outputLine << counts.l << " ";
			}

			if (W_FLAG && flags) {
				outputLine << counts.w << " ";
			}

			if (C_FLAG && flags) {
				outputLine << counts.c << " ";
			}

			if (M_FLAG && flags) {
				outputLine << counts.m << " ";
			}

			outputLine << filename;
			std::cout << outputLine.str();
		}

	} catch (const BadFlagException& e) {
        	std::cout << e.what() << std::endl;
        	return 1;
		std::cout << "Memory allocation failed: " << e.what() << std::endl;
        	std::cout << "The input might be too large to process." << std::endl;
        	return 1;
	} catch (const std::exception& e) {
		std::cout << "An unexpected error occurred: " << e.what() << std::endl;
        	return 1;
    	}

	return 0;
}
