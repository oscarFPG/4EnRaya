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

/**
 * Accept a player
 * 
 * @param socketft Socket descriptor
 * @param playerAddress Address of the incoming player
 * @return Player socket descriptor
*/
int acceptPlayer(int socketfd, struct sockaddr_in* playerAddress);

/**
 * Select a random player between both sockets
 * 
 * @param playerSocket1 Player1 socket
 * @param playerSocket2 Player2 socket
 * @return Random player socket from both
*/
tPlayer selectRandomPlayer(int playerSocket1, int playerSocket2);

/**
 * Sends code, message and board to each player based on if they have to make a move or wait
 * 
 * @param turnPlayerSocket Players socket that moves
 * @param turnPlayerchip Players chip that moves
 * @param waitPlayerSocket Players socket that waits
 * @param waitPlayerChip Players chip that moves
 * @param message Buffer to send the messages
 * @param board Board to send to both players
*/
void turnAction(int turnPlayerSocket, char turnPlayerChip, int waitPlayerSocket, char waitPlayerChip, tString* message, tBoard board);

/**
 * Function to play a game from a thread
 * 
 * @param gameInfo Reference to threadArgs struct
*/
void* playGame(void* gameInfo);