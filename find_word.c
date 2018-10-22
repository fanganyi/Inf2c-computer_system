// ==========================================================================
// Word Finder
// ==========================================================================
// Print all words in a sentence

// Inf2C-CS Coursework 1. Task A 
// PROVIDED file, to be used as model for writing MIPS code.

// Instructor: Boris Grot
// TA: Priyank Faldu
// 10 Oct 2017

//---------------------------------------------------------------------------
// C definitions for SPIM system calls
//---------------------------------------------------------------------------
#include <stdio.h>

void read_string(char* s, int size) { fgets(s, size, stdin); }

void print_char(char c)    { printf("%c", c); }   
void print_int(int num)    { printf("%d", num); }
void print_string(const char* s) { printf("%s", s); }

#define false 0
#define true 1

// Maximum characters in an input sentence excluding terminating null character
#define MAX_SENTENCE_LENGTH 1000

// Maximum characters in a word excluding terminating null character
#define MAX_WORD_LENGTH 50

// Global variables
// +1 to store terminating null character
char input_sentence[MAX_SENTENCE_LENGTH+1];
char word[MAX_WORD_LENGTH+1];

void read_input(const char* inp) {
    print_string("\nEnter input: ");
    read_string(input_sentence, MAX_SENTENCE_LENGTH+1);
}

void output(const char* out) {
    print_string(out);
    print_string("\n");
}

// returns true if an input character is a valid word character
// returns false if an input character is any punctuation mark (including hyphen)
int is_valid_character(char ch) {
    if ( ch >= 'a' && ch <= 'z' ) {
        return true;
    } else if ( ch >= 'A' && ch <= 'Z' ) {
        return true;
    } else {
        return false;
    }
}

// returns true only if an input character is hyphen
int is_hyphen(char ch) {
    if ( ch == '-' ) {
        return true;
    } else {
        return false;
    }
}

// Global index that points to the next character to be processed in "input_sentence"
// Retains its value across function calls
int input_index = 0;

// Marks if the sentence is processed
int end_of_sentence = false;

// This function processes character array pointed to by "inp".
// It keeps processing "inp" till 
//   either it finds a valid word OR
//   it finds an end of a sentence
//
// If a valid word is found, it is stored at "w" and the function returns true.
// If no word is found and the end of sentence is encountered, it returns false.
int process_input(char* inp, char* w) {

    char cur_char = '\0';
    int is_valid_ch = false;

    // Indicates how many elements in "w" contains valid word characters
    int char_index = -1;

    while( end_of_sentence == false ) {
        // This loop runs until end of an input sentence is encountered or a valid word is extracted
        cur_char = inp[input_index];
        input_index++;

        // Check if it is a valid character
        is_valid_ch = is_valid_character(cur_char);

        if ( is_valid_ch ) {
            w[++char_index] = cur_char;
        } else {
            if ( cur_char == '\n' || cur_char == '\0' ) {
                // Indicates an end of an input sentence
                end_of_sentence = true;
            }
            if ( char_index >= 0 ) {
                // w has accumulated some valid characters. Thus, punctuation mark indicates a possible end of a word
                if ( is_hyphen(cur_char) == true && is_valid_character(inp[input_index]) ) {
                    // check if the next character is also a valid character to detect hyphenated word.
                    w[++char_index] = cur_char;
                    continue;
                }
                // w has accumulated some valid characters. Thus, punctuation mark indicates an end of a word
                char_index++;
                w[char_index] = '\0';
                return true;
            }
            // skip the punctuation mark
            w[0] = '\0';
            char_index = -1;
        }
    }
    return false;
}


int main() {

    int word_found = false;

    read_input(input_sentence);

    print_string("\noutput:\n");

    do {

        word_found = process_input(input_sentence, word);

        if ( word_found == true ) {
            output(word);
        }

    } while ( word_found == true );

    return 0;
}

//---------------------------------------------------------------------------
// End of file
//---------------------------------------------------------------------------

