#include "game.h"

/**
 * Send a message to the player. This includes the length of the message and the message itself
 * @param socketClient Socket descriptor
 * @param message Message to be sent
 */
void sendMessageToPlayer (int socketClient, char* message);

/**
 * Receive a message from the player. This includes the length of the message and the message itself
 * @param socketClient Socket descriptor
 * @param message Message to be received
 */
void receiveMessageFromPlayer (int socketClient, char* message);

/**
 * Send a code to the player.
 * @param socketClient Socket descriptor
 * @param code Code to be send
 */
void sendCodeToClient (int socketClient, unsigned int code);

/**
 * Send a board to the player.
 * @param socketClient Socket descriptor
 * @param board Board of the game
 */
void sendBoardToClient (int socketClient, tBoard board);

/**
 * Receive a move from the player.
 * @param socketClient Socket descriptor
 * @return Move performed by the player
 */
unsigned int receiveMoveFromPlayer (int socketClient);

/**
 * Get the associated socket to player
 *
 * @param player Current player
 * @param player1socket Socket that connects with player 1
 * @param player1socket Socket that connects with player 2
 * @return Associated socket to player
 */
int getSocketPlayer (tPlayer player, int player1socket, int player2socket);

/**
 * Switch player
 *
 * @param currentPlayer Current player
 * @return Rival player
 */
tPlayer switchPlayer (tPlayer currentPlayer);

