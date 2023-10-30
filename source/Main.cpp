#include <iostream>
#include <fstream>
#include <string>

#define DEFAULT_CHUNK_SIZE 134217728
#define DEFAULT_CHUNK_PREFIX "chunk_"

int Split(std::string filename, unsigned int chunkSize);
int Merge(std::string filename, unsigned int count);

int main() {
	for (;;) {
		std::string input;
		std::cout << "> ";
		std::getline(std::cin, input);

		std::string args[3];
		unsigned int na = 0;
		for (unsigned int i = 0; i <= input.size(); i++) {
			//if (input[i] == '\0')
				//break;
			if (input[i] == ' ') {
				i++;
				na++;
				if (na > 2)
					break;
			}
			args[na] += input[i];
		}

		if (args[0] == "split") {
			if (args[1] == "")
				std::cout << "not enough arguments!\n";
			if (args[2] == "")
				Split(args[1], DEFAULT_CHUNK_SIZE);
			else
				Split(args[1], stoi(args[2]));
		}

		if (args[0] == "merge") {
			if (args[1] == "")
				std::cout << "not enough arguments!\n";
			else if (args[2] == "")
				std::cout << "not enough arguments!\n";
			else
				Merge(args[1], stoi(args[2]));
		}
	}

	system("pause");
	return 0;
}


int Split(std::string filename, unsigned int chunkSize) {
	// Open input file with a name stored in filename
	std::ifstream inputfile;
	inputfile.open(filename, std::ios::binary | std::ios::in | std::ios::out);
	if (!inputfile.is_open()) {
		std::cout << "Cannot open file!\n";
		return -1;
	}

	// Get size of input file
	inputfile.seekg(0, std::ios::end);
	unsigned long long size = inputfile.tellg();
	inputfile.seekg(0, std::ios::beg);

	// calculate the number of chunks to split
	unsigned int n = (size / chunkSize) + 1;
	std::cout << "Splitting " << filename << " of size " << size << " bytes into " << n << " chunks of maximum " << chunkSize << " bytes each.\n";

	// Set up buffer for read data
	char* buffer = new char[chunkSize];
	std::string ofilename;
	unsigned int rwsize;
	for (unsigned int i = 0; i < n; i++) {
		if ((size - (i * chunkSize)) < chunkSize)
			rwsize = size - (i * chunkSize);
		else
			rwsize = chunkSize;

		inputfile.seekg(i * chunkSize, std::ios::beg);
		inputfile.read(buffer, rwsize);

		ofilename = DEFAULT_CHUNK_PREFIX + std::to_string(i) + "_" + filename;
		std::ofstream of(ofilename.c_str());
		of.close();

		std::fstream outfile(ofilename.c_str(), std::ios::binary | std::ios::in | std::ios::out);

		outfile.write(buffer, rwsize);
		outfile.close();

		std::cout << "Created chunk: " << i << " | filename: " << ofilename << " | size: " << rwsize << " | bytes\n";
	}
	delete[] buffer;
	inputfile.close();

	return 0;
}

int Merge(std::string filename, unsigned int count) {
	std::ofstream of(filename.c_str());
	of.close();

	std::fstream outputfile;
	outputfile.open(filename.c_str(), std::ios::binary | std::ios::out | std::ios::in);
	for (unsigned int i = 0; i <= (count - 1); i++) {
		std::ifstream infile;
		infile.open((DEFAULT_CHUNK_PREFIX + std::to_string(i) + "_" + filename), std::ios::binary);

		infile.seekg(0, std::ios::end);
		unsigned long long infileSize = infile.tellg();
		infile.seekg(0, std::ios::beg);

		std::cout << "Merging " << infileSize << " bytes into " << filename << " from " << (DEFAULT_CHUNK_PREFIX + std::to_string(i) + "_" + filename) << "\n";

		char* buffer = new char[infileSize];
		infile.read(buffer, infileSize);

		outputfile.seekg(0, std::ios::end);
		outputfile.write(buffer, infileSize);

		delete[] buffer;
		infile.close();
	}
	outputfile.close();
	return 0;
}