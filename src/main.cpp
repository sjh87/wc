#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <locale>
#include <codecvt>

#define C_FLAG 0b1000
#define L_FLAG 0b0100
#define M_FLAG 0b0010
#define W_FLAG 0b0001

#define OUTPUT_COLUMN_WIDTH 6

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
	
	for (const char& c : line) {
		if (std::isspace(c)) {
			inWord = false;
		} else if (!inWord) {
			inWord = true;
			count++;
		}
	}

	return count;
}

std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
size_t countChars(std::string &line) {
    std::u32string u32Str = conv.from_bytes(line);

	return u32Str.length();
}

struct Counts {
	size_t c; // bytes
	size_t l; // lines
	size_t m; // characters, including multi-byte characters
	size_t w; // words
};

std::stringstream outputLineStream(const Counts &counts, const u_int8_t &flags) {
	std::stringstream outputLine;

	if (L_FLAG & flags) {
		outputLine << std::setw(OUTPUT_COLUMN_WIDTH) << counts.l << " ";
	}

	if (W_FLAG & flags) {
		outputLine << std::setw(OUTPUT_COLUMN_WIDTH) << counts.w << " ";
	}

	if (M_FLAG & flags) {
		outputLine << std::setw(OUTPUT_COLUMN_WIDTH) << counts.m << " ";
	}

	if (C_FLAG & flags) {
		outputLine << std::setw(OUTPUT_COLUMN_WIDTH) << counts.c << " ";
	}

	return outputLine;
}

std::stringstream outputLineStream(const std::string &filename, const Counts &counts, const u_int8_t &flags) {
	std::stringstream outputLine = outputLineStream(counts, flags);

	outputLine << std::setw(OUTPUT_COLUMN_WIDTH) << filename;

	return outputLine;
}

int main(int argc, char** argv) {
	try {
		u_int8_t flags = collectFlags(argc, argv);
		std::set<std::string> filenames;
		collectFilenames(argc, argv, filenames);

		if (filenames.empty()) {
			Counts counts{};
			std::string line;

			while (std::getline(std::cin, line)) {
				counts.c += line.length() + 1; // add one for newline/carriage return char
				counts.l++;
				counts.m += countChars(line) + 1;
				counts.w += countWords(line);
			}

			if (std::cin.eof()) { // OS signals an EOF when the pipe is empty
				if (!line.empty()) {
					counts.c -= 1;
					counts.m -= 1;
				}

			} else if (std::cin.bad()) {
				std::cout << "could not read data from stdin" << std::endl;

				return 1;
			}

			std::cout << outputLineStream(counts, flags).str() << std::endl;
			return 0;
		}

		std::map<std::string, Counts> countsPerFile;
		Counts totalCounts{};

		for (const auto &filename : filenames) {
			std::ifstream file(filename);

			if (!file.is_open()) {
				std::cout << "failed to open file \"" << filename << "\"" << std::endl;
				return 1;
			}

			Counts counts{};

			std::string line;
			while (std::getline(file, line)) {
				counts.c += line.length() + 1; // add one for newline/carriage return char
				counts.l++;
				counts.m += countChars(line) + 1;
				counts.w += countWords(line);
			}

			if (file.eof()) {
				if (!line.empty()) {
					counts.c -= 1;
					counts.m -= 1;
				}

				totalCounts.c += counts.c;
				totalCounts.l += counts.l;
				totalCounts.m += counts.m;
				totalCounts.w += counts.w;
				countsPerFile.insert({ filename, counts });
				continue;
			} else if (file.bad()) {
				std::cout << "could not read data in " << filename << std::endl;

				return 1;
			}
		}

		for (const auto& [filename, counts] : countsPerFile)
			std::cout << outputLineStream(filename, counts, flags).str() << std::endl;
		

		if (countsPerFile.size() > 1)
			std::cout << outputLineStream("total", totalCounts, flags).str() << std::endl;

	} catch (const BadFlagException& e) {
		std::cout << e.what() << std::endl;

		return 1;
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
