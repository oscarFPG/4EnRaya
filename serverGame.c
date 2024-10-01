#include "serverGame.h"
#include <pthread.h>

const int MAX_MATCHES = 10;

void sendMessageToPlayer(int socketClient, char* message){

	// Enviamos el tamaño del mensaje
	int size = strlen(message);
	send(socketClient, &size, sizeof(size), 0);

	// Enviamos el mensaje
	int msgLength = send(socketClient, message, size, 0);

	// Check the number of bytes sent
	if (msgLength < 0)
		showError("ERROR while writing to the socket");
}

void receiveMessageFromPlayer(int socketClient, char* message){

	// Recibimos el tamaño del mensaje
	int size;
	recv(socketClient, &size, sizeof(size), 0);

	// Recibimos el mensaje
	int msgLength = recv(socketClient, message, size, 0);

	// Check read bytes
	if (msgLength < 0)
		showError("ERROR while reading from socket");
}

void sendCodeToClient(int socketClient, unsigned int code){

	int msgLength = send(socketClient, &code, sizeof(code), 0);
	
	// Check the number of bytes sent
	if (msgLength < 0)
		showError("ERROR while writing to the socket");
}

void sendBoardToClient(int socketClient, tBoard board){

	// Enviamos el tablero
	int msgLength = send(socketClient, board, BOARD_HEIGHT * BOARD_WIDTH, 0);

	// Check the number of bytes sent
	if (msgLength < 0)
		showError("ERROR while writing to the socket");
}

unsigned int receiveMoveFromPlayer(int socketClient){

	unsigned int move;
	int msgLength = recv(socketClient, &move, sizeof(move), 0);

	// Check read bytes
	if (msgLength < 0)
		showError("ERROR while reading from socket");

	return move;
}

int getSocketPlayer(tPlayer player, int player1socket, int player2socket){

	int socket;
	if (player == player1)
		socket = player1socket;
	else
		socket = player2socket;

	return socket;
}

tPlayer switchPlayer(tPlayer currentPlayer){

	tPlayer nextPlayer;
	if (currentPlayer == player1)
		nextPlayer = player2;
	else
		nextPlayer = player1;

	return nextPlayer;
}

// --------------------------------------------------------------------------------------------------------------------- //
int acceptPlayer(int socketfd, struct sockaddr_in* playerAddress){

	// Listen
	listen(socketfd, 10);

	// Get length of client structure
	int clientLength = sizeof(playerAddress);

	// Accept!
	int socketPlayer = accept(socketfd, playerAddress, &clientLength);

	// Check accept result
	if (socketPlayer < 0)
		showError("ERROR while accepting");

	return socketPlayer;
}

tPlayer selectRandomPlayer(int playerSocket1, int playerSocket2){
	
	int number = rand() % 11;
	return number % 2 == 0 ? player1 : player2;
}

void turnAction(int turnPlayerSocket, char turnPlayerChip, int waitPlayerSocket, char waitPlayerChip, tString* message, tBoard board){

	// Send turn code
	sendCodeToClient(turnPlayerSocket, TURN_MOVE);
	sendCodeToClient(waitPlayerSocket, TURN_WAIT);

	// Send turn message
	sprintf(message, "It's your turn. You play with: %c", turnPlayerChip);
	sendMessageToPlayer(turnPlayerSocket, message);
	memset(message, 0, STRING_LENGTH);
	sprintf(message, "Your rival is thinking...please, wait! You play with: %c", waitPlayerChip);
	sendMessageToPlayer(waitPlayerSocket, message);
	memset(message, 0, STRING_LENGTH);

	// Send board state
	printBoard(board, message);
	sendBoardToClient(turnPlayerSocket, board);
	sendBoardToClient(waitPlayerSocket, board);
}

void* playGame(void* gameInfo){

	tThreadArgs* playerInfo = (tThreadArgs *)gameInfo; 

	tPlayer currentPlayer;							/** Current player */
	struct sockaddr_in player1Address;				/** Client address structure for player 1 */
	int socketPlayer1 = playerInfo->socketPlayer1;	/** Socket descriptor for player 1 */
	struct sockaddr_in player2Address;				/** Client address structure for player 2 */
	int socketPlayer2 = playerInfo->socketPlayer2;	/** Socket descriptor for player 2*/
	tString message;								/** Message sent to the players */
	tBoard board;									/** Board of the game */

	// Initialize memory
	initBoard(&board);
	memset(&message, 0, STRING_LENGTH);

	// Send player names
	sendMessageToPlayer(socketPlayer1, &playerInfo->player2Name);
	sendMessageToPlayer(socketPlayer2, &playerInfo->player1Name);

	// Random selection of one player to start playing
	currentPlayer = selectRandomPlayer(socketPlayer1, socketPlayer2);
	if(currentPlayer == player1)
		printf("Turno del jugador1: %s\n", playerInfo->player1Name);
	else
		printf("Turno del jugador2: %s\n", playerInfo->player2Name);

	// Loop to receive game movements from both players until one wins or a draw
	int currentPlayerSocket = getSocketPlayer(currentPlayer, socketPlayer1, socketPlayer2);
	unsigned int move = -1;
	int isWinner = FALSE;
	int boardFull = FALSE;
	int endOfGame = FALSE;

	while(endOfGame == FALSE){

		tMove validMove = fullColumn_move;
		while(validMove == fullColumn_move) {

			// Current player makes a move
			if(currentPlayer == player1)
				turnAction(socketPlayer1, PLAYER_1_CHIP, socketPlayer2, PLAYER_2_CHIP, &message, board);
			else
				turnAction(socketPlayer2, PLAYER_2_CHIP, socketPlayer1, PLAYER_1_CHIP, &message, board);

			// Receive player movement
			move = receiveMoveFromPlayer(currentPlayerSocket);
			validMove = checkMove(board, move);
		}
		
		// ACTUALLY make the move
		insertChip(board, currentPlayer, move);

		// Check if it is a winning move
		isWinner = checkWinner(board, currentPlayer);
		boardFull = isBoardFull(board);
		endOfGame = isWinner || boardFull;

		// Change turn
		if(endOfGame == FALSE){
			currentPlayer = switchPlayer(currentPlayer);
			currentPlayerSocket = getSocketPlayer(currentPlayer, socketPlayer1, socketPlayer2);
		}
	}
	
	// Print last board status
	printBoard(board, &message);

	// Show winner player message to both player
	if(isWinner){	// Last player to move is the winner
		sendCodeToClient(currentPlayerSocket, GAMEOVER_WIN);
		sendMessageToPlayer(currentPlayerSocket, "You win!");
		sendBoardToClient(currentPlayerSocket, board);

		currentPlayer = switchPlayer(currentPlayer);
		currentPlayerSocket = getSocketPlayer(currentPlayer, socketPlayer1, socketPlayer2);

		sendCodeToClient(currentPlayerSocket, GAMEOVER_LOSE);
		sendMessageToPlayer(currentPlayerSocket, "You lose!");
		sendBoardToClient(currentPlayerSocket, board);
	}
	else{
		sendCodeToClient(currentPlayerSocket, GAMEOVER_DRAW);
		sendMessageToPlayer(currentPlayerSocket, "Draw!");
		sendBoardToClient(currentPlayerSocket, board);

		currentPlayer = switchPlayer(currentPlayer);
		currentPlayerSocket = getSocketPlayer(currentPlayer, socketPlayer1, socketPlayer2);

		sendCodeToClient(currentPlayerSocket, GAMEOVER_DRAW);
		sendMessageToPlayer(currentPlayerSocket, "Draw!");
		sendBoardToClient(currentPlayerSocket, board);
	}

	// Close sockets
	shutdown(socketPlayer1, SHUT_RDWR);
	shutdown(socketPlayer2, SHUT_RDWR);
	pthread_exit(NULL);
}
// --------------------------------------------------------------------------------------------------------------------- //

int main(int argc, char *argv[]){

	// Server info
	int socketfd;						/** Socket descriptor */
	unsigned int port;					/** Listening port */
	struct sockaddr_in serverAddress;	/** Server address structure */

	// Players info
	struct sockaddr_in player1Address;	/** Client address structure for player 1 */
	tString player1Name;				/** Name of player 1 */
	int socketPlayer1;					/** Socket descriptor for player 1 */

	struct sockaddr_in player2Address;	/** Client address structure for player 2 */
	tString player2Name;				/** Name of player 2 */
	int socketPlayer2;					/** Socket descriptor for player 2 */

	// Variables for multiple games
	tThreadArgs matches[MAX_MATCHES];	/** Matches to play simultaneously */
	int matchesCounter = 0;				/** Matches counter */
	pthread_t matchThreads[MAX_MATCHES];/** Thread to play the matches */


	// Check arguments
	if (argc != 2) {
		fprintf(stderr,"ERROR wrong number of arguments\n");
		fprintf(stderr,"Usage:\n$>%s port\n", argv[0]);
		exit(1);
	}

	// Init seed
	srand(time(NULL));

	// Create the socket
	socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Check
	if (socketfd < 0)
		showError("ERROR while opening socket");

	// Init server structure
	memset(&serverAddress, 0, sizeof(serverAddress));

	// Get listening port
	port = atoi(argv[1]);

	// Fill server structure
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(port);

	// Bind
	if (bind(socketfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
		showError("ERROR while binding");

	printf("Server started! Waiting for players...\n");
	while(1){

		// Wait for both players
		socketPlayer1 = acceptPlayer(socketfd, &player1Address);
		receiveMessageFromPlayer(socketPlayer1, &player1Name);
		socketPlayer2 = acceptPlayer(socketfd, &player2Address);
		receiveMessageFromPlayer(socketPlayer2, &player2Name);

		// DEBUG: show both players names in the server
		printf("%s vs %s\n", player1Name, player2Name);

		// Store both players sockets
		matches[matchesCounter].socketPlayer1 = socketPlayer1;
		matches[matchesCounter].socketPlayer2 = socketPlayer2;

		// Clear both players buffers
		memset(&matches[matchesCounter].player1Name, 0, STRING_LENGTH);
		memset(&matches[matchesCounter].player2Name, 0, STRING_LENGTH);

		// Store both players names in the buffer
		strncpy(&matches[matchesCounter].player1Name, &player1Name, strlen(player1Name));
		strncpy(&matches[matchesCounter].player2Name, &player2Name, strlen(player2Name));

		// Play match
		pthread_create(&matchThreads[matchesCounter], NULL, (void *)playGame, (void *)&matches[matchesCounter]);
		++matchesCounter;
	}
		
	return 0;
}