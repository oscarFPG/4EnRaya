#include "serverGame.h"

void sendMessageToPlayer (int socketClient, char* message){

	int msgLength = send(socketClient, message, strlen(message), 0);

	// Check the number of bytes sent
	if (msgLength < 0)
		showError("ERROR while writing to the socket");
}

void receiveMessageFromPlayer (int socketClient, char* message){

	memset(message, 0, STRING_LENGTH);
	int messageLength = recv(socketClient, message, STRING_LENGTH-1, 0);

	// Check read bytes
	if (messageLength < 0)
		showError("ERROR while reading from socket");

}

void sendCodeToClient (int socketClient, unsigned int code){


}

void sendBoardToClient (int socketClient, tBoard board){


}

unsigned int receiveMoveFromPlayer (int socketClient){

	return 0;
}

int getSocketPlayer (tPlayer player, int player1socket, int player2socket){

	int socket;

		if (player == player1)
			socket = player1socket;
		else
			socket = player2socket;

	return socket;
}

tPlayer switchPlayer (tPlayer currentPlayer){

	tPlayer nextPlayer;

		if (currentPlayer == player1)
			nextPlayer = player2;
		else
			nextPlayer = player1;

	return nextPlayer;
}

// --------------------------------------------------------------------------------------------------------------------- //
int acceptPlayer(int socketfd, struct sockaddr_in* playerAddress, tString* playerName){

	// Listen
	listen(socketfd, 10);

	// Get length of client structure
	int clientLength = sizeof(playerAddress);

	// Accept!
	int socketPlayer = accept(socketfd, playerAddress, &clientLength);

	// Check accept result
	if (socketPlayer < 0)
		showError("ERROR while accepting");


	//Nombre player
	receiveMessageFromPlayer(socketPlayer, playerName);
	printf("Se unio: %s.\n", playerName);

	return socketPlayer;
}
// --------------------------------------------------------------------------------------------------------------------- //

int main(int argc, char *argv[]){

	int socketfd;						/** Socket descriptor */
	struct sockaddr_in serverAddress;	/** Server address structure */
	unsigned int port;					/** Listening port */
	struct sockaddr_in player1Address;	/** Client address structure for player 1 */
	struct sockaddr_in player2Address;	/** Client address structure for player 2 */
	int socketPlayer1, socketPlayer2;	/** Socket descriptor for each player */
	unsigned int clientLength;			/** Length of client structure */

	tBoard board;						/** Board of the game */
	tPlayer currentPlayer;				/** Current player */
	tMove moveResult;					/** Result of player's move */
	tString player1Name;				/** Name of player 1 */
	tString player2Name;				/** Name of player 2 */
	int endOfGame;						/** Flag to control the end of the game*/
	unsigned int column;				/** Selected column to insert the chip */
	tString message;					/** Message sent to the players */



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

		//Listening and more
		socketPlayer1 = acceptPlayer(socketfd, &player1Address, &player1Name);
		socketPlayer2 = acceptPlayer(socketfd, &player2Address, &player2Name);

		//Send named palyers
		sendMessageToPlayer(socketPlayer1, player2Name);
		sendMessageToPlayer(socketPlayer2, player1Name);


		return 0;
}
