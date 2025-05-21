<h1 align="center">Tablut Rules</h1>

### Competition Rules
- Separated torunaments: teams will be grouped accordingly to theri dimension
- Round-trip matches: one game as White, one game as Black
- Every win, 3 points. Every draw, 1 point.
- Limited time to choose a move:
    - 1 minute for each move (strict constraints)
    - timeout given by the referee (server)
    - in case of timeout, the active player loses
- During the opposite turn the AI player can't do calculate
- The deadline for sending players is 25 May 2025 (23.59)

### VM Spec
- OS: Linux Debian 64 bit
- Number of CPU: 4
- RAM: 8GB
- Disk size: 30GB
- NO GPUs will be available
- No internet access (the player will be able to using only local resources)

### Player Configurations
- All the project resources should be included in a single folder named ```/home/tablut/tablut```
- Path for the script that run the player: ```/home/tablut/tablut/runmyplayer.sh```
- The launcher script must accept three parameters:
    1. Role (white or black, no case sensitive)
    2. The Timeout in seconds (60s, but it could be lowered)
    3. The Server-Ip 

    Examples: ```/home/tablut/tablut/runmyplayer.sh WHITE 60 192.168.20.101```
- The script code must is case-insensitive

### Server Comunication Info
Comunication messages are JSON strings:
- At the beginning the server waits for the player: the first player that must connect to the server is always the white player
- Both players communiucate their name through strings
- Once both the players are connected, the server reacts by sending the current game state to both the player, then the game starts
- Once the server has received the move from a player, it communicates the new state to both the players, than it waits for the move of the adversary
- The player after having sent its move to the server, must read from the server the new state (modified by its move). Then wait again for the new state (modified by its adversary move) through a blocking read function.

### Comunication JSON Format
This is the Action to string method (java version):
```Java
public String toString() {
		return "Turn: " + this.turn + " " + "Pawn from " + from + " to " + to;
	}
```
The format of the move is: ``` Turn: W Pawn from e4 to b4 ```

### Socket
We need a socket to comunicate with server:
```C
    // Initializes IP and port server
	port = (playerColor == WHITE) ? WHITE_SERVER_PORT : BLACK_SERVER_PORT;
	memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	host = gethostbyname(server_ip);
	servaddr.sin_addr.s_addr=((struct in_addr *)(host->h_addr))->s_addr;
	servaddr.sin_port = htons(port);

    // Socket creation
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if(sd < 0) {
	   perror("Socket error.\n");
	   exit(1);
	}

	// Connect
	if(connect(sd,(struct sockaddr *) &servaddr, sizeof(struct sockaddr)) < 0) {
	   perror("Connect error.\n");
	   exit(1);
	}

 ``` 
- we use ```read(sd, stateString, 1);``` to read information from server
- we use ```write(sd, name, sizeof(char) * strlen(name));``` to comunicate with server