#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

// Creates a file name
char *createFileName(char *fileName, char *fName, int num) {
    // Appends number to the end of the original file name
    strcpy(fName, fileName);
    char end[30] = ".";
    char fNum[30];
    // If num > 1 and num < 9 then append num
    if (num > 0 && num < 10) {
        sprintf(fNum, "%d", num);
    // Append res to file name if num is < 1 or > 8
    } else {
        sprintf(fNum, "%s", "res");
    }
    
    // Concatenate strings together to create correct file name
    strcat(end, fNum);
    strcat(fName, end);
    return fName;
}

// Splits a byte (character) into 8 bits
void splitByte(unsigned char b, int bits[8]) {
    // Assigns bits in correct order (7 -> 0)
    for (int i = 7; i >= 0; i--) {
        bits[i] = b & 1;
        b = b >> 1;
    }
}

// Get the parity of the array of 8 bits
int getParity(int bits[]) {
    // Gets odd or even parity
    int count = 0;
    // For each bit
    for (int i = 0; i < 8; i++) {
        // Add to count if the bit is 1
        if (bits[i] == 1) { count++; }
    }
    // Returns the parity (0 - Even or 1 - Odd)
    return count % 2;
}

// Writes the current 8 bits to the files
void writeBitsToFiles(FILE *files[], int bits[]) {
    // Writes 8 bits to their respective files
    for (int i = 0; i < 8; i++) {
        // Writes each bit to their respective file
        fprintf(files[i+1], "%d", bits[i]);
    }
    // Writes parity to the res file
    fprintf(files[9], "%d", getParity(bits));
}

// Closes start to end files
void closeFiles(FILE *files[], int start, int end) {
    for (int i = start; i <= end; i++) {
        fclose(files[i]);
    }
}

// Converts binary characters into integers
int convertCharToBit(char c) {
    if (c == '0') { return 0; }
    else if (c == '1') { return 1; } 
    else { 
        printf("Bad Character\n"); 
        return -1; 
    }
}

// Rebuilds the missing file (files[fileNum])
void rebuildFile(FILE *files[], int fileNum) {
    // If parity file was deleted
    if (fileNum == 9) return;
    // Counter for bits
    int count = 0;
    // Character and int variables for parity
    char currentParity;
    // Character for bit
    int currentBit;
    // int for bit to rewrite to the deleted file
    int bit;

    // Reads each character at a time while there are more characters in the res file
    while ((currentParity = fgetc(files[9])) != EOF) {
        // Goes through all numbered files, and then checks parity and writes into deleted file
        for (int i = 1; i <= 9; i++) {
            // If the file already exists
            if (i != fileNum && i < 9) {
                // Get the next int, and add it to the current count
                currentBit = convertCharToBit(fgetc(files[i]));
                count += currentBit;
            }
            // Write into the file that has been deleted
            else if (i == 9) {
                // If parity does not match count
                if (count % 2 != convertCharToBit(currentParity)) {
                    bit = 1;
                } else {
                    bit = 0;
                }
                // Add bit to the file
                fprintf(files[fileNum], "%d", bit);
            }
            
        }
        // Reset count
        count = 0;
    }
}

// Creates a character from 8 bits
unsigned char rebuildChar(int bits[]) {
    // return ret;
    char c = 0;  // Start with an empty byte (0)

    // Iterate through the bits array
    for (int i = 0; i < 8; i++) {
        // Shift the bit to the correct position and add it to the result
        c |= (bits[i] << (7 - i));
    }
    return c;
}

// Creates the res file based on the 9 files
void createRes(FILE *files[]) {
    // Read each bit
    int bits[8];
    // Character for bit
    int currentBit;

    // Reads each character at a time while there are more characters in the first file
    while (fgetc(files[1]) != EOF) {
        // Goes back in the first file to prevent desync
        fseek(files[1], -1, SEEK_CUR);
        // Goes through all numbered files, and then checks parity and writes into deleted file
        for (int i = 1; i <= 9; i++) {
            // If the file already exists
            if (i < 9) {
                // Get the next int, and add it to the current count
                currentBit = convertCharToBit(fgetc(files[i]));
                // printf("Bit: %d\n", currentBit);
                bits[i-1] = currentBit;
            }
            // Write into the file that has been deleted
            else if (i == 9) {
                // Write the character to the res file
                char c =  rebuildChar(bits);
                // printf(" %c", c);
                fprintf(files[0], "%c", c);
            }
            
        }
    }
    // printf("\n");
}

// Get input function that cleans trailing newline characters
void get_input(char *string, size_t size) {
	fgets(string, size, stdin);
	string[strcspn(string, "\n\r")] = 0;
}

// Main operation 
void run(FILE *input, char *fileName) {
    // Get name of file trimmed
    char *fileNameTrimmed = strtok(fileName, ".");
    // Creates variable to hold individual file names
    char *fName = (char *)malloc(strlen(fileNameTrimmed) + 5);

    // Create the 9 files
    FILE *files[10];

    // Creates 9 write files (1-9)
    for (int i = 0; i <= 9; i++) {
        fName = createFileName(fileNameTrimmed, fName, i);

        if (i != 0) {
            files[i] = fopen(fName, "w");
        }
    }

    // Create array for bits
    int bits[8];
    // Current character variable
    unsigned char currentChar;
    // Int value to check for EOF
    int charCheck;

    // Reads each character at a time while there are more characters in the file
    while ((charCheck = fgetc(input)) != EOF) {
        currentChar = (unsigned char) charCheck;
        // Split byte into bits
        splitByte(currentChar, bits);
        // Write bits to all of the files
        writeBitsToFiles(files, bits);
    }
    // Close input file
    fclose(input);

    // Close the files for now
    closeFiles(files, 1, 9);
 
    // Input to check for 
    char in[100];
    
    // Prompt user to delete a file
    while (strcasecmp(in, "y") != 0) {
        // If the input was not y
        printf("Delete one bit file, enter 'y' when done: ");
        // Get input from user and remove newline character
		get_input(in, sizeof(in));
    }

    // fileNum variable to check which file was deleted
    int fileNum = 0;

    // Go through 8 files to see if one has been deleted
    for (int i = 1; i <= 9; i++) {
        // Gets the file name (1-8)
        fName = createFileName(fileNameTrimmed, fName, i);
        // If a file does not exist
        if (access(fName, F_OK) == -1) {
            // If a file has already been deleted send an error
            if (fileNum != 0) {
                printf("More than 1 file deleted.");
                return;
            }
            // Set fileNum to i
            fileNum = i;
        }
    }
    
    // Open the files that still exist, and create/open the file the has been deleted
    for (int i = 1; i <= 9; i++) {
        fName = createFileName(fileNameTrimmed, fName, i);
        // If its the file that has been deleted
        if (i == fileNum) {
            // Create append file
            files[i] = fopen(fName, "w");
        }
        // If the file still exists
        else {
            // Read from the file
            files[i] = fopen(fName, "rb");
        }
    }

    // Rebuilds the missing file
    rebuildFile(files, fileNum);

    // Close files to reset
    closeFiles(files, 1, 9);

    // Create res file with original info
    fName = createFileName(fileNameTrimmed, fName, 0);
    files[0] = fopen(fName, "w");

    // Reopens the files to create res files
    for (int i = 1; i <= 9; i++) {
        fName = createFileName(fileNameTrimmed, fName, i);
        // Read from the file
        files[i] = fopen(fName, "rb");
    }
    
    // Creates the res file
    createRes(files);

    // Close the files and free memory
    closeFiles(files, 0, 9);
    free(fName);
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        perror("Invalid number of arguments");
        return 1;
    }

    // Open input file
    FILE *input;
    input = fopen(argv[1], "rb");

    // Send an error if failing to open the file
    if (input == NULL) { 
        perror ("Input Error"); 
        return 1; 
    } 

    // Run program
    run(input, argv[1]);

    return 0;
}