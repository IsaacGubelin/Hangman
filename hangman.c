#include "hangman.h"
#include <ctype.h> // For validating characters
#include <stdio.h>
#include <stdlib.h> // Clearing screen
#include <string.h> // strlen
#include <unistd.h> // Sleep function

#define MAX_WORD_SIZE 100
#define ALPHABET_SIZE 26
#define WELCOME_DURATION 1
#define MAX_ATTEMPTS 7

// Package of variables needed for a game of hangman
typedef struct {
    char hostWord[MAX_WORD_SIZE];
    char guesserWord[MAX_WORD_SIZE];
    char usedLetters[ALPHABET_SIZE];
    uint8_t totalBlanks;
    uint32_t guessesLeft;



} hangman_t;

// Displays a welcome message for a few seconds
void WelcomeScreen() {
    printf("Welcome to Hangman!\n");
    sleep(WELCOME_DURATION);
    
} 


// This function verifies user input for the word to be guessed by the players.
// It is used in the SetWordVariables function. MODIFIES PARAMETER WORD.
// Returns false if punctuation, numbers, or non-aplha/space char is detected.
// CAPITALIZES ALL LETTERS
bool CapAndValidate(char userWord[]) {
    
    // Advance through whole input word until null terminator is reached
    // All characters must be letters or spaces
    char currChar;
    for (uint8_t index = 0; userWord[index] != '\0'; index++) { //FIXME: Change conditional to strlen function for clarity
        
        currChar = userWord[index];
        if (isalpha(currChar) || isspace(currChar)) { // VALID
            userWord[index] = toupper(currChar);
        }
        else {                                       // INVALID
            printf("Invalid char found\n");
            return false;
        }

    }
    return true;
}

// Helper function for collecting user input for word to be guessed.
// Also fills totalBlanks variable with correct number of letters.
void SetWordVariables(hangman_t* hm) {

    // Takes a word or phrase from host and makes a copy

    // First attempt to receive word from host
    fgets(hm->hostWord, MAX_WORD_SIZE, stdin);
    // If invalid, keep prompting for valid entry.
    if (!CapAndValidate(hm->hostWord)) {
        do {  // Host will be repeatedly asked for word until validity is confirmed
            printf("Only letters and spaces are valid. Please enter again.\n");
            fgets(hm->hostWord, MAX_WORD_SIZE, stdin);  // Grab input from host
        } while (!CapAndValidate(hm->hostWord)); // If entry is invalid, repeat
    }

    strcpy(hm->guesserWord, hm->hostWord);

    // Change all the letters in the copy string to an underscore '_'
    for (uint8_t i = 0; i < strlen(hm->guesserWord); i++) {
        
        // Don't convert the spaces to underscores
        if (!isspace(hm->guesserWord[i])) {
            hm->guesserWord[i] = '_';
            hm->totalBlanks += 1;
        }
    }
    
    // Clear the screen so the other players don't see the word!
    #ifdef _WIN32
        system("cls"); // For Windows
    #else
        system("clear"); // For Linux/macOS
    #endif

}

// Initiate the variables for the given hangman game
void HangmanInit(hangman_t *hm) {
    hm->totalBlanks = 0;
    hm->guessesLeft = MAX_ATTEMPTS;
    for (uint8_t i = 0; i < ALPHABET_SIZE; i++) {
        hm->usedLetters[i] = ' ';
    }
}

// Helper function that checks if guessed letter is valid
// Returns false if char is not a letter or if it was already guessed
bool IsValidLetter(char letter, hangman_t* hm) {
    if (!isalpha(letter))   // Check that char is an alphabet character
        return false;
    for (uint8_t i = 0; i < ALPHABET_SIZE; i++) {
        if (hm->usedLetters[i] == letter)   // Check all letters previously guessed
            return false;   // Return false if letter was already used.
    }
    return true; // If these tests pass, return true.
}


// Helper function to check if hangman word still has blanks left
bool IsWordGuessed(hangman_t *hm) {
    for (uint8_t index = 0; hm->guesserWord[index] != '\0'; index++) { //FIXME: Change to strlen
        if (hm->guesserWord[index] == '_') // If there are still blanks
            return false;                   // Then word isn't guessed.
    }
    return true; // If no blanks are found, then word has been solved.
}

// Function for getting a guessed letter and checking if it's in the word
// Decrements guesses left variable in hangman struct if guess is not found
// Alters guesser word variable by changing blanks to guessed letter if found
void EvaluateGuess(hangman_t* hm) {

    static uint8_t guessIndex = 0; // Used for storing guesses in array
    bool correctGuess = false; // Flag for when a guess is found in word
    uint8_t numFound = 0; // Gives count of occurrences of the letter guessed


    char letter;
    printf("Type a letter to guess!\n");
    letter = getchar(); // Get a letter from guessers
    while (getchar() != '\n'); // Ignore the newline made from enter button

    // If the letter is invalid, keep prompting for new letter
    if (!IsValidLetter(letter, hm)) {
        do {    // This loop keeps going until a valid letter is entered
            printf("Invalid guess! Enter another letter.\n");
            letter = getchar(); // User tries again to input valid letter
            while (getchar() != '\n'); // ignore newline from enter button

        } while (!IsValidLetter(letter, hm)); // If invalid, repeat loop
    }

    letter = toupper(letter);           // Capitalize the letter
    printf("You entered: %c\n", letter); // Display the guessed letter
    hm->usedLetters[guessIndex] = letter; // Store guessed letter
    guessIndex++;                       // Advance index for next guess

    // Search through host's word to see if letter is in it
    // When a match is found, reveal it by changing the blank to the letter
    for (uint8_t i = 0; i < strlen(hm->hostWord); i++) {
        if (hm->hostWord[i] == letter) { // if letter is found
            correctGuess = true; // Raise the flag to signal the guess is correct
            numFound++; // Increase occurrence count
            hm->guesserWord[i] = letter; // Reveal the letter
        }
    }

    // Give results of guess
    if (numFound > 1) { // If guessed letter was found multiple times
        printf("Very good! There are %d %c's.\n", numFound, letter);
    } else if (numFound == 1) { // If guessed letter is found only once
        printf("There is one %c.\n", letter);
    } else { // Incorrect guess / No matches found7
        printf("Sorry, no %c's were found.\n", letter);
        hm->guessesLeft--; // Incorrect guess, decrement variable for tries left
    }

}



// HANGMAN GAMEPLAY FUNCTION
// Collects user input for a word/phrase and conducts a single full game of Hangman.
void PlayHangman() {

    // userWord holds the word being guessed. guessedSoFar is what is displayed to guessers.
    hangman_t hmGame;

    HangmanInit(&hmGame);

    WelcomeScreen();
   printf("Sizeof hmGame vars: %d %d %d\n", strlen(hmGame.guesserWord), strlen(hmGame.hostWord), strlen(hmGame.usedLetters)); //FIXME:

    // Have host secretly enter word for other players to guess
    printf("Host, please type in a word or phrase less than %d characters long.\n", MAX_WORD_SIZE);
   printf("Sizeof hmGame vars: %d %d %d\n", strlen(hmGame.guesserWord), strlen(hmGame.hostWord), strlen(hmGame.usedLetters)); //FIXME:

    // Load both the word and underscore arrays, get number of blanks
    SetWordVariables(&hmGame);
    
    // Game begins
    printf("There are %d blanks.\n", hmGame.totalBlanks);

    printf("Sizeof hmGame vars: %d %d %d\n", strlen(hmGame.guesserWord), strlen(hmGame.hostWord), strlen(hmGame.usedLetters)); //FIXME:

    //FIXME: debug print
    printf("Debugging:\n%s\n%s\n", hmGame.guesserWord, hmGame.hostWord);

    // Main loop of game that continues until guesses are used up
    // Players win if they guess word before running out of tries
    while (hmGame.guessesLeft > 0 && !IsWordGuessed(&hmGame)) {
         printf("Sizeof hmGame vars: %ld %ld %ld\n", strlen(hmGame.guesserWord), strlen(hmGame.hostWord), strlen(hmGame.usedLetters)); //FIXME:
        EvaluateGuess(&hmGame); // This function handles letter guesses
        printf("%s\nstrlen: %ld\n", hmGame.usedLetters, strlen(hmGame.usedLetters));
        
        
        // printf("\n");
        printf("%s\n", hmGame.guesserWord);
    }


}

void TestFunc() {
    


}