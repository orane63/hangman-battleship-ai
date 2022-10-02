# hangman-battleship-ai

This repository includes two programs that try to play optimally with a player.

1. The Hangman AI plays the game of Hangman by trying to guess a player's phrase in as few moves as possible. For each un-guessed word in the phrase, it calculates the likelihood that a character in a dictionary will be the player's phrase. It bases this off of the frequency of letters in the dictionary and the restrictions on the phrase given by previously guessed characters. It also has the ability to completely guess a user's phrase when it is confident. Moreover, it accounts for the fact that the user can tell at most one lie about a character being in a phrase or not being in a phrase.
2. The Battleship AI tries to guess the positions of a user's battleships in as few moves as possible. In summary, it attempts to calculate the likelihood each square is part of a ship. It does this by looking at the squares adjacent to that square and whether or not they are already occupied by a ship. It then simply chooses the square with the maximum likelihood to attack.
