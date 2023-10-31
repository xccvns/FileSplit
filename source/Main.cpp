#include <iostream>
#include <fstream>
#include <string>

#define DEFAULT_CHUNK_SIZE 134217728 // roughly ~134Mb
#define DEFAULT_CHUNK_PREFIX "chunk" // example filename oryginal: "example.txt" chunk: "chunk5_example.txt"

int Split(std::string filename, unsigned int chunkSize);
int Merge(std::string filename, unsigned int count);

int main() {
	// First message
	std::cout << "FileSplit program!\nType help for more info.\n";
	// Program loop
	for (;;) {
		std::string input;
		std::cout << "> ";
		std::getline(std::cin, input);

		std::string args[3];
		std::uint8_t numOfArgs = 0;
		for (unsigned int i = 0; i <= input.size(); i++) {
			if (input[i] == '\0')
				break;
			if (input[i] == ' ') {
				i++;
				numOfArgs++;
				if (numOfArgs > 2)
					break;
			}
			args[numOfArgs] += input[i];
		}

		// split command issued
		if (args[0] == "split") {
			if (args[1] == "")
				std::cout << "not enough arguments!\n";
			if (args[2] == "")
				Split(args[1], DEFAULT_CHUNK_SIZE);
			else
				Split(args[1], stoi(args[2]));
		}

		// merge command issued
		if (args[0] == "merge") {
			if (args[1] == "")
				std::cout << "not enough arguments!\n";
			else if (args[2] == "")
				std::cout << "not enough arguments!\n";
			else
				Merge(args[1], stoi(args[2]));
		}

		// help command issued
		if (args[0] == "help") {
			std::cout << "Available commands:\n";
			std::cout << "split <filename> <chunk size>(optional)       - Splits file into chunks\n";
			std::cout << "merge <oryginal filename> <number of chunks>  - Merges chunks into one file\n";
			std::cout << "help                                          - Displays help\n";
			std::cout << "clear                                         - Clears the screen\n";
			std::cout << "exit                                          - Exits the program\n";
		}

		// exit command issued
		if (args[0] == "cls" || args[0] == "clr" || args[0] == "clear") {
			system("cls");
		}

		// exit command issued
		if (args[0] == "exit") {
			break;
		}
	}
	return 0;
}

int Split(std::string filename, unsigned int chunkSize) {
	// Open input file
	std::ifstream inputfile;
	inputfile.open(filename, std::ios::binary | std::ios::in | std::ios::out);
	// Chec if input file was opened correctly
	if (!inputfile.is_open()) {
		std::cout << "Cannot open file!\n";
		return -1;
	}

	// Get size of input file
	inputfile.seekg(0, std::ios::end);
	unsigned long long size = inputfile.tellg();
	inputfile.seekg(0, std::ios::beg);

	// Calculate the number of chunks to split
	unsigned int n = (size / chunkSize) + 1;
	std::cout << "Splitting " << filename << " of size " << size << " bytes into " << n << " chunks of maximum " << chunkSize << " bytes each.\n";

	// Set up buffer for read data
	char* buffer = new char[chunkSize];
	std::string outputFilename;
	unsigned int currentChunkSize;
	// Loop for every chunk calculated
	for (unsigned int i = 0; i < n; i++) {
		// Check if we are on the last chunk file (because last one wont be exactly chunkSize but smaller)
		if ((size - (i * chunkSize)) < chunkSize)
			currentChunkSize = size - (i * chunkSize);
		else
			currentChunkSize = chunkSize;

		// Put getpointer at the start of each chunk's data and read currentChunkSize bytes
		inputfile.seekg(i * chunkSize, std::ios::beg);
		inputfile.read(buffer, currentChunkSize);

		// Create output chunk file
		outputFilename = DEFAULT_CHUNK_PREFIX + std::to_string(i) + "_" + filename;
		std::ofstream of(outputFilename.c_str());
		of.close();

		// Open created chunk file
		std::fstream outfile(outputFilename.c_str(), std::ios::binary | std::ios::in | std::ios::out);

		// Write currentChunkSize bytes of data to the chunk file
		outfile.write(buffer, currentChunkSize);
		outfile.close();

		// Log
		std::cout << "Created chunk: " << i << " | filename: " << outputFilename << " | size: " << currentChunkSize << " | bytes\n";
	}
	// Delete buffer and close input file
	delete[] buffer;
	inputfile.close();
	return 0;
}

int Merge(std::string filename, unsigned int count) {
	// Create output file
	std::ofstream of(filename.c_str());
	of.close();

	// Open created output file
	std::fstream outputfile;
	outputfile.open(filename.c_str(), std::ios::binary | std::ios::out | std::ios::in);

	// Loop for every chunk file
	for (unsigned int i = 0; i <= (count - 1); i++) {
		// Open current chunk file
		std::ifstream infile;
		infile.open((DEFAULT_CHUNK_PREFIX + std::to_string(i) + "_" + filename), std::ios::binary);

		// Read the size of current chunk file
		infile.seekg(0, std::ios::end);
		unsigned long long infileSize = infile.tellg();
		infile.seekg(0, std::ios::beg);

		// Log
		std::cout << "Merging " << infileSize << " bytes into " << filename << " from " << (DEFAULT_CHUNK_PREFIX + std::to_string(i) + "_" + filename) << "\n";

		// Create buffer and read current chunk file
		char* buffer = new char[infileSize];
		infile.read(buffer, infileSize);

		// Write to output file
		outputfile.seekg(0, std::ios::end);
		outputfile.write(buffer, infileSize);

		// Delete buffer and close current chunk file
		delete[] buffer;
		infile.close();
	}

	// Close output file
	outputfile.close();
	return 0;
}