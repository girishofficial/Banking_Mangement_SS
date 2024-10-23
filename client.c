    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <signal.h>
    #include <arpa/inet.h>
    #include <termios.h>

    #define PORT 8087
    #define BUFFER_SIZE 10240

    int sock = 0;

    void signal_handler(int sig) {
        printf("Received signal %d, closing client...\n", sig);
        system("/home/girish-pc/projecter/cuslog");
        system("/home/girish-pc/projecter/logemp");
        close(sock);
        exit(0);
    }

    void logout() {
        char *command = "./client";
        int result = system(command);
        if (result == -1) {
            printf("Error Logging Out!!.\n");
        }
        exit(0);
    }

    int main() {
        system("clear");
        struct sockaddr_in server;
        signal(SIGINT, signal_handler);
        signal(SIGTERM,signal_handler);
        char message[BUFFER_SIZE], server_reply[BUFFER_SIZE];

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            perror("Could not create socket");
            return 1;
        }

        server.sin_addr.s_addr = inet_addr("127.0.0.1");
        server.sin_family = AF_INET;
        server.sin_port = htons(PORT);

        if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
            perror("Connect failed");
            return 1;
        }

        printf("Connected to server\n");

        // Receive the menu from the server
        bzero(server_reply, BUFFER_SIZE);
        if (read(sock, server_reply, BUFFER_SIZE) < 0) {
            perror("Recv failed");
            return 1;
        }
        printf("%s", server_reply);

        while (1) {
            printf("Client input\n");
            fgets(message, BUFFER_SIZE, stdin);
            message[strcspn(message, "\n")] = '\0';

            bzero(server_reply, BUFFER_SIZE);

            // Send user input to the server
            if (write(sock, message, strlen(message)) < 0) {
                return 1;
            }

            // Read server reply after sending user input
            if (read(sock, server_reply, BUFFER_SIZE) < 0) {
                perror("Recv failed");
                return 1;
            }

            // Check for different password prompts and mask input
            if (strcmp(server_reply, "Enter Customer Password: ") == 0 ||
                strcmp(server_reply, "Enter Employee Password: ") == 0 ||
                strcmp(server_reply, "Enter Manager Password: ") == 0 ||
                strcmp(server_reply, "Enter admin password: ") == 0) {
                char *password = getpass(server_reply);
                strcpy(message, password);

                if (write(sock, message, strlen(message)) < 0) {
                    return 1;
                }

                if (read(sock, server_reply, BUFFER_SIZE) < 0) {
                    perror("Recv failed");
                    return 1;
                }


                printf("%s\n", server_reply);
            }



            printf("%s\n", server_reply);


            if (strncmp(message, "10", 2) == 0 || strcmp(message, "EXIT") == 0) {
                printf("Exiting...\n");
                close(sock);
                break;
            }

            if (strncmp(message, "9", 1) == 0) {
                printf("Exiting...\n");
                close(sock);
                logout();
                break;
            }
        }

        close(sock);
        return 0;
    }
