 #include "utils.h"

/**
 * Send a message to the server. This includes the length of the message and the message itself
 * @param socketServer Socket descriptor
 * @param message Message to be sent
 */
void sendMessageToServer (int socketServer, char* message);

/**
 * Receive a message from the server. This includes the length of the message and the message itself
 * @param socketServer Socket descriptor
 * @param message Message to be received
 */
void receiveMessageFromServer (int socketServer, char* message);

/**
 * Receive a board from the server.
 * @param socketServer Socket descriptor
 * @param board Board of the game
 */
void receiveBoard (int socketServer, tBoard board);

/**
 * Receive a code from the server.
 * @param socketServer Socket descriptor
 * @return Code
 */
unsigned int receiveCode (int socketServer);

/**
 * Reads a move entered by the player
 * @return A number between [0-6] that represents the column where the chip is going to be inserted
 */
unsigned int readMove ();

/**
 * Send a move to the server.
 * @param socketServer Socket descriptor
 * @param move A number between [0-6] that represents the column where the chip is going to be inserted
 */
void sendMoveToServer (int socketServer, unsigned int move);
