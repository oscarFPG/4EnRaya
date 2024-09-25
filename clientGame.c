#include "clientGame.h"

void sendMessageToServer (int socketServer, char* message) { 

	// Enviamos el mensaje
	int msgLength = send(socketServer, message, strlen(message), 0);

	// Check the number of bytes sent
	if (msgLength < 0)
		showError("ERROR while writing to the socket");
}

void receiveMessageFromServer (int socketServer, char* message){

	// Recibimos el mensaje
	int msgLength = recv(socketServer, message, 10, 0);

	// Check read bytes
	if (msgLength < 0)
		showError("ERROR while reading from socket");
}

void receiveBoard (int socketServer, tBoard board){

	int size = BOARD_HEIGHT * BOARD_WIDTH;
	int messageLength = recv(socketServer, board, size, 0);

	// Check read bytes
	if (messageLength < 0)
		showError("ERROR while reading from socket");
}

unsigned int receiveCode (int socketServer){

	tString codeString;
	int msgLength = recv(socketServer, codeString, CODE_SIZE, 0);
	printf("Recibidos %d bytes\n", msgLength);

	int code = atoi(codeString);
	printf("Code received: %d\n", code);

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

	char* moveString;
	sprintf(moveString, "d", move);
	int msgLength = send(socketServer, moveString, sizeof(unsigned int), 0);
	memset(moveString, 0, strlen(moveString));

	// Check the number of bytes sent
	if (msgLength < 0)
		showError("ERROR while writing to the socket");
}

// -------------------------------------------------------------------

// -------------------------------------------------------------------

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
		memset(playerName, 0, STRING_LENGTH);
		printf ("Enter player name: ");
		fgets(playerName, STRING_LENGTH-1, stdin);

		// Remove '\n'
		playerName[strlen(playerName)-1] = 0;

		// Display player name
		printf("Welcome %s!\n", playerName);

	}while (strlen(playerName) <= 2);


	// Send player's name to the server
	sendMessageToServer(socketfd, playerName);

	// Receive rival's name
	receiveMessageFromServer(socketfd, &rivalName);
	printf("You are playing against %s\n", &rivalName);

	// Init
	endOfGame = FALSE;

	// Game starts
	printf("Game starts!\n\n");

	// While game continues...
	while(!endOfGame){

		// 1. Receive game code
		code = receiveCode(socketfd);

		// 2. Print message
		memset(message, 0, strlen(message));
		receiveMessageFromServer(socketfd, &message);
		printf("%s\n", message);

		shutdown(socketfd, SHUT_RDWR);
		printf("Leaving the game...\n");
		exit(0);

		// 3. Print board
		//memset(board, 0, sizeof(board));
		//receiveBoard(socketfd, board);

		/*
		switch (code){
		case TURN_MOVE:
			column = readMove();
			sendMoveToServer(socketfd, column);
			break;
		case TURN_WAIT:
			break;
		case GAMEOVER_DRAW:
			endOfGame = TRUE;
			break;
		default:
			printf("Error: code unexpected\n");
			exit(EXIT_FAILURE);
		}
		*/
	}

	// Close socket
	shutdown(socketfd, SHUT_RDWR);
	printf("Leaving the game...\n");

	return 0;

}