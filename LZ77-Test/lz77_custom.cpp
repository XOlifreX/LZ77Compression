#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>


#define LOOKUPSIZE 21
#define SEARCHSIZE 20

// Obviously not final. The offset & length variables are generally placed in one single byte divide in 2 parts that each define one of the two variables
// For practical and educational purposes, we use a struct with those variables.
struct Pointer {
	char offset;
	char len;
	char character; // char is only saved when no match is found for anything on the search field. We just take the first char of the lookup field.
};

// std::string input = "ABCCBACBBABCABCBCAABACABBBBABCAB";
char input[1000 * 1000];
int inputLength = 0;
Pointer encoded[1000 * 1000];
int pointerCounter = 0;
char decoded[1000 * 1000];


void EncodeCustom(void) {

	char lookupSize = LOOKUPSIZE;
	char searchSize = 0;
	int searchPos = 0;
	int lookupPos = 0;

	for (int i = 0; i < inputLength;  i+=0) {

		Pointer biggest = { 0,0 };

		// Loop over de search field. If it did not find any match, place a 0 reference item in the encoded array.
		// Loop over de search field and try every possible size
		// int maxSizeLookup = -1 * ((lookupPos - (searchSize + i)) - LOOKUPSIZE);
		// int maxSizeSearch = searchSize;

		char variableLookup[LOOKUPSIZE];
		for (int temp = 0; temp < LOOKUPSIZE; temp++)
			variableLookup[temp] = 0;
		for (char sizes = 1; sizes < SEARCHSIZE + 1; sizes++) {
			
			//Do we go OoB? In this case it means reaching the end of the input, so we could be reading OoB memory.
			if (i + sizes > inputLength)
				break;

			char indexPos = 0;
			// Loop over the lookupfield and pickup the first field of its size.
			for (int l = lookupPos; l < lookupPos + sizes; l++) {
				variableLookup[indexPos] = input[l];
				indexPos++;
			}

			char variableSearch[SEARCHSIZE];
			for (int temp = 0; temp < SEARCHSIZE; temp++)
				variableSearch[temp] = 0;
			// Loop over the search positions for finding a match with the selected value of the lookup field
			// It will loop over each position by one with each size.
			for (int k = searchPos; k < (SEARCHSIZE + searchPos); k++) {

				// Do we go OoB? Yes? Break out of the for-loop
				if (k + sizes > searchPos + searchSize)
					break;

				char indexPos = 0;
				// Loop over the searchfield and pickup the first field of its size.
				for (int l = (0 + k); l < (0 + k) + sizes; l++) {
					variableSearch[indexPos] = input[l];
					indexPos++;
				}

				// check if content is equal
				int counter = 0;
				for (int index = 0; index < sizes; index++) {
					if (variableSearch[index] == variableLookup[index])
						counter++;
				}

				// Chech if the found variables equal to each other
				if (counter == sizes) {
					// Yes? Is it the biggest found match, though?
					if (biggest.len < counter) {
						biggest = { (char)(lookupPos - k) ,(char)(counter) };
					}
				}

			}
		}

		// If the length of the biggest match is 0? No match has been found, so the first char of the lookup field is taken
		if (biggest.len == 0) 
			biggest.character = input[lookupPos];

		encoded[pointerCounter] = biggest;
		pointerCounter++;

		char toRaiseSize = biggest.len != 0 ? biggest.len : 1;
		i += toRaiseSize;
		lookupPos += toRaiseSize;

		for (int temp = 0; temp < toRaiseSize; temp++) {
			if (searchSize < SEARCHSIZE)
				searchSize++;
			else
				searchPos++;
		}

	}

	// print the encoded array out on the screen to see the contents.
	std::ofstream encodedFile("../LZ77-Test/testdocEncoded.txt");
	encodedFile.clear();
	for (int i = 0; i < pointerCounter; i++) {

		if (encoded[i].len == 0) {
			std::cout << "Char: " << encoded[i].character << "" << std::endl;
			encodedFile << encoded[i].character << ", " << std::endl;
		}
		else {
			std::cout << "Offset: " << (int)(encoded[i].offset) << " - Length: " << (int)(encoded[i].len) << "" << std::endl;
			encodedFile << "Offset: " << (int)(encoded[i].offset) << " - Length: " << (int)(encoded[i].len) << "" << std::endl;
		}

	}

	encodedFile.close();

}

void DecodeCustom(void) {

	std::cout << "\nDecoding: \n" << std::endl;
	int decodedIndex = 0;

	for (int i = 0; i < pointerCounter; i++) {

		Pointer currentCheckingPointer = encoded[i];

		if (currentCheckingPointer.len == 0) {
			decoded[decodedIndex] = currentCheckingPointer.character;
			decodedIndex++;
		}
		else {

			int toUseIndex = decodedIndex - currentCheckingPointer.offset;
			for (int j = 0; j < currentCheckingPointer.len; j++) {

				decoded[decodedIndex] = decoded[toUseIndex + j];
				decodedIndex++;

			}


		}

	}

	decoded[decodedIndex] = '\0';
	decodedIndex++;


	for (int i = 0; i < decodedIndex; i++)
		std::cout << decoded[i];

	std::cout << std::endl;
	std::string decodedInput(decoded);

	std::ofstream decodedFile("../LZ77-Test/testdocDecoded.txt");
	decodedFile.clear();
	decodedFile << decodedInput;
	decodedFile.close();

	std::cout << "\nAre they the same?" << std::endl;
	if (strcmp(decodedInput.c_str(), input) == 0)
		std::cout << "YES!!" << std::endl;
	else
		std::cout << "No..." << std::endl;

}

void readFile() {

	std::string line;
	std::ifstream theFile("../LZ77-Test/testdoc.txt");
	int index = 0;

	if (theFile.is_open()) {
		while (std::getline(theFile, line)) {
			for (int i = 0; i < line.length(); i++) {
				input[index] = line[i];
				index++;
			}
		}
	}

	std::cout << input << std::endl;

	inputLength = index; // No + 1, cause of the \0 character at the end.

}


int main() {

	readFile();

	EncodeCustom();
	DecodeCustom();

	std::cin.get();

}