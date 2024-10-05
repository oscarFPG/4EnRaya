#include "clientGame.h"

// ------------------------------------------------ Auxiliary Functions ------------------------------------------------ //
void sendMessageToServer (int socketServer, char* message) { 

	// Enviamos el tamaño del mensaje
	int size = strlen(message);
	send(socketServer, &size, sizeof(size), 0);

	// Enviamos el mensaje
	int msgLength = send(socketServer, message, size, 0);

	// Check the number of bytes sent
	if (msgLength < 0)
		showError("ERROR while writing to the socket");
}

void receiveMessageFromServer (int socketServer, char* message){

	// Recibimos el tamaño del mensaje
	int size;
	recv(socketServer, &size, sizeof(size), 0);

	// Recibimos el mensaje
	int msgLength = recv(socketServer, message, size, 0);

	// Check read bytes
	if (msgLength < 0)
		showError("ERROR while reading from socket");
}

void receiveBoard(int socketServer, tBoard board){

	int msgLength = recv(socketServer, board, BOARD_HEIGHT * BOARD_WIDTH, 0);

	// Check read bytes
	if (msgLength < 0)
		showError("ERROR while reading from socket");
}

unsigned int receiveCode (int socketServer){

	int code;
	int msgLength = recv(socketServer, &code, sizeof(code), 0);

	// Check read bytes
	if (msgLength < 0)
		showError("ERROR while reading from socket");

	return code;
}

unsigned int readMove(){

	tString enteredMove;
	unsigned int move;
	unsigned int isRightMove;


	// Init...
	isRightMove = FALSE;
	move = STRING_LENGTH;

	while (!isRightMove){

		printf ("Enter a move [0-6]:");

		// Read move
		fgets (enteredMove, STRING_LENGTH-1, stdin);

		// Remove new-line char
		enteredMove[strlen(enteredMove)-1] = 0;

		// Length of entered move is not correct
		if (strlen(enteredMove) != 1){
			printf ("Entered move is not correct. It must be a number between [0-6]\n");
		}

		// Check if entered move is a number
		else if (isdigit(enteredMove[0])){

			// Convert move to an int
			move =  enteredMove[0] - '0';

			if (move > 6)
				printf ("Entered move is not correct. It must be a number between [0-6]\n");
			else
				isRightMove = TRUE;
		}

		// Entered move is not a number
		else
			printf ("Entered move is not correct. It must be a number between [0-6]\n");
	}

	return move;
}

void sendMoveToServer (int socketServer, unsigned int move){

	int msgLength = send(socketServer, &move, sizeof(move), 0);

	// Check the number of bytes sent
	if (msgLength < 0)
		showError("ERROR while writing to the socket");
}
// ------------------------------------------------ Auxiliary Functions ------------------------------------------------ //

// ---------------------------------------------- Our Auxiliary Functions ---------------------------------------------- //
void playerAction(const int socketfd, const unsigned int code, tBoard board, char* message){

	switch(code) {
	case TURN_MOVE: //When is his turn
		unsigned int column = readMove();
		sendMoveToServer(socketfd, column);
		break;
	case TURN_WAIT: //When is not his turn
		break;
	default: //Smething go wrong
		printf("Error: code unexpected\n");
		exit(EXIT_FAILURE);
	}
}
// ---------------------------------------------- Our Auxiliary Functions ---------------------------------------------- //

int main(int argc, char *argv[]){

	int socketfd;						/** Socket descriptor */
	unsigned int port;					/** Port number (server) */
	struct sockaddr_in server_address;	/** Server address structure */
	char* serverIP;						/** Server IP */

	tBoard board;						/** Board to be displayed */
	tString playerName;					/** Name of the player */
	tString rivalName;					/** Name of the rival */
	tString message;					/** Message received from server */
	unsigned int column;				/** Selected column */
	unsigned int code;					/** Code sent/receive to/from server */
	unsigned int endOfGame;				/** Flag to control the end of the game */


	// Check arguments!
	if (argc != 3){
		fprintf(stderr,"ERROR wrong number of arguments\n");
		fprintf(stderr,"Usage:\n$>%s serverIP port\n", argv[0]);
		exit(0);
	}

	// Get the server address
	serverIP = argv[1];

	// Get the port
	port = atoi(argv[2]);

	// Create socket
	socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Check if the socket has been successfully created
	if (socketfd < 0)
		showError("ERROR while creating the socket");

	// Fill server address structure
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(serverIP);
	server_address.sin_port = htons(port);

	// Connect with server
	if (connect(socketfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
		showError("ERROR while establishing connection");

	printf ("Connection established with server!\n");

	// Init player's name
	do{
		memset(&playerName, 0, STRING_LENGTH);
		printf ("Enter player name: ");
		fgets(playerName, STRING_LENGTH - 1, stdin);

		// Remove '\n'
		playerName[strlen(playerName) - 1] = 0;

		// Display player name
		printf("Welcome %s!\n", playerName);

	}while (strlen(playerName) <= 2);

	// Send player's name to the server
	sendMessageToServer(socketfd, playerName);

	// Receive rival's name
	memset(&rivalName, 0, STRING_LENGTH);
	receiveMessageFromServer(socketfd, &rivalName);
	printf("You are playing against %s\n", rivalName);

	// Game starts
	printf("Game starts!\n\n");

	// While game continues...
	endOfGame = FALSE;
	while(endOfGame == FALSE){

		// 1. Receive game code
		code = receiveCode(socketfd);

		// Check if game has ended
		if(code == GAMEOVER_WIN || code == GAMEOVER_LOSE || code == GAMEOVER_DRAW){
			endOfGame = TRUE;
			continue;
		}

		// 2. Receive message
		memset(&message, 0, STRING_LENGTH);
		receiveMessageFromServer(socketfd, &message);

		// 3. Receive board
		memset(&board, EMPTY_CELL, BOARD_HEIGHT * BOARD_WIDTH);
		receiveBoard(socketfd, board);
		printBoard(board, &message);

		// 4. Player action
		memset(&message, 0, strlen(message));
		playerAction(socketfd, code, board, &message);

		// 5. Clear buffers
		memset(&message, 0, strlen(message));
	}

	// Print last game result: message + final board
	memset(&message, 0, STRING_LENGTH);
	receiveMessageFromServer(socketfd, &message);
	receiveBoard(socketfd, board);
	printBoard(board, &message);
	memset(&message, 0, strlen(message));
	
	// Close socket
	shutdown(socketfd, SHUT_RDWR);
	printf("Leaving the game...\n");

	return 0;
}