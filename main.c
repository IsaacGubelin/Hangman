#include <stdio.h>
#include <string.h>
#include "hangman.h"

bool IsWordGuessedForSure(char word[]) {
    for (uint8_t index = 0; word[index] != '\0'; index++) {
        if (word[index] == '_') // If there are still blanks
            return false;                   // Then word isn't guessed.
    }
    return true; // If no blanks are found, then word has been solved.
}

int main() {


	PlayHangman();


	

	return 0;
}
