/////////////////////////
// HANGMAN GAME HANDLING
// 
//
/////////////////////////


#ifndef HANGMAN_H
#define HANGMAN_H


#include <stdbool.h>
#include <stdint.h>



void PlayHangman(); // Top level function for playing hangman game
bool CapAndValidate(char userWord[]);
void TestFunc(); // Dummy function to test private functions in .c file


#endif // HANGMAN_H