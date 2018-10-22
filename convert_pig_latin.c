// ==========================================================================
// PigLatin Converter
// ==========================================================================
// Convert all words in a sentence using PigLatin rules 

// Inf2C-CS Coursework 1. Task B 
// PROVIDED file, to be used to complete the task in C and as a model for writing MIPS code.

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
char output_sentence[(MAX_SENTENCE_LENGTH*3)+1];

void read_input(const char* inp) {
    print_string("Enter input: ");
    read_string(input_sentence, MAX_SENTENCE_LENGTH+1);
}

void output(const char* out) {
    print_string(out);
    print_string("\n");
}

// Do not modify anything above
//
//
// Define your global variables here
//
//
// Write your own functions here
//
//
int endword(char ch){
   if ( ch >= 'a' && ch <= 'z' ) {
        return false;
    } 
   if ( ch >= 'A' && ch <= 'Z' ) {
        return false;
    } 
   if(ch=='-'){
        return false;
    }
   return true; 
    
}

int isvowel(char ch){             //whether ch is vowel
   if(ch=='a'||ch=='e'||ch=='i'||ch=='o'||ch=='u'||ch=='A'||ch=='E'||ch=='I'||ch=='O'||ch=='U'){
        return true;
     }
      return false;
 }

int iscap(char ch){                    //whether it’s capital
     if ( ch >= 'A' && ch <= 'Z' ) {
        return true;
    }
     return false;
}
char cap(char ch){       //capitalise ch
    char out;
    if(iscap(ch)){
     out=ch;
        return out;
   }
     out= ch-32;
    return out;
}
char lower(char ch){       //lower case ch
    char out;
    if(iscap(ch)){
     out= ch+32;
        return out;
   }
     out= ch;
    return out;
}
void process_input(char* inp, char* out) {
    int i=0;  //index of inp
    int o=-1;  //index of out
    int fc=0; //index of first char
    int fv=0;  //index of first vowel
    
    while(inp[i]!='\0'&&inp[i]!='\n'){ //this loop will end when sentence is end
        
    while(!endword(inp[i]) && !isvowel(inp[i])){   // stop at find first vowel or first non alpabet char
        i++;
    }
        fv=i;
    if(isvowel(inp[i])){             //if this word contains vowel
        
        i++;
        if(iscap(inp[fc])){              //add first vowel to out, check whether use capital
            out[++o]=cap(inp[fv]);
        }else{
        out[++o]=lower(inp[fv]);
        }
        while(!endword(inp[i])){     //concate rest chars from first vowel to word end
          out[++o]=inp[i];
          i++;
        }
    }
        
        if(iscap(inp[i-1])){
        while(fc<fv){     //if this word dont have a vowel,add this word to out. if has a vowel,add chars before first vowel. check whether add capital or not
            out[++o]=cap(inp[fc]);
            fc++;
        }
        }else{
            while(fc<fv){
            out[++o]=lower(inp[fc]);
            fc++;
        }
        }
    
    if(iscap(inp[i-1])){         //add 'ay' at end of a word, check whether use capital or not
        out[++o]='A';
        out[++o]='Y';
    }else{
        out[++o]='a';
        out[++o]='y';
    }
    while(endword(inp[i])&&inp[i]!='\0'&&inp[i]!='\n'){
        out[++o]=inp[i];
        i++;
    }
    fc=i;                       //reset index of first char in a word
    }
}
//
// Do not modify anything below


int main() {

    read_input(input_sentence);

    print_string("\noutput:\n");

    output_sentence[0] = '\0';
    process_input(input_sentence, output_sentence);
  
    output(output_sentence);

    return 0;
}

//---------------------------------------------------------------------------
// End of file
//---------------------------------------------------------------------------

