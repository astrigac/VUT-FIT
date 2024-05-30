/*
 * IPK Project 1: IPK Calculator Protocol
 *
 * Aurel Strigac (xstrig00@vutbr.cz)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUFSIZE 1024
int client_socket;
enum available_modes
{
    tcp,
    udp
} mode;

void sig_handler()
{
    // In case of tcp, we need to send BYE before closing the socket
    if (mode == tcp)
    {
        char buf[BUFSIZE];
        char end_message[] = "BYE\n";
        strcpy(buf, end_message);
        printf("\n%s", buf);

        int bytestx = send(client_socket, buf, strlen(buf), 0);
        if (bytestx < 0)
        {
            perror("[ ERROR ] In sendto");
        }

        // Recieving and displaying servers response
        bzero(buf, BUFSIZE);
        int bytesrx = recv(client_socket, buf, BUFSIZE, 0);
        if (bytesrx < 0)
        {
            perror("[ ERROR ] In recvfrom");
        }

        // Displaying servers response
        if (!strcmp(buf, end_message))
        {
            printf("BYE");
        }
        else
        {
            fprintf(stderr, "[ ERROR ] Unexpected server response\n");
        }
    }

    // This happens both to udp and tcp
    close(client_socket);
    exit(0);
}

bool check_argument_format(int argc, const char *argv[])
{
    // Indexes:         0     1   2     3   4     5   6
    // Correct usage: ipkcpc -h <host> -p <port> -m <mode>
    if (argc != 7)
    {
        return false;
    }

    if (strcmp(argv[1], "-h"))
    {
        return false;
    }

    if (strcmp(argv[3], "-p"))
    {
        return false;
    }

    if (strcmp(argv[5], "-m"))
    {
        return false;
    }

    for (long unsigned int i = 0; i < strlen(argv[4]); i++)
    {
        if (!isdigit(argv[4][i]))
        {
            return false;
        }
    }
    if (strlen(argv[6]) != 3)
    {
        return false;
    }

    for (int i = 0; i < 3; i++)
    {
        if (!isalpha(argv[6][i]))
        {
            return false;
        }
    }

    if (strcmp(argv[6], "tcp") && strcmp(argv[6], "udp"))
    {
        return false;
    }

    return true;
}

void print_udp_packet(char udp_packet[])
{
    int index = 3;
    while (udp_packet[index] != 0x00)
    {
        printf("%c", udp_packet[index++]);
    }
}

void buf_to_udp(char buf[], char udp_packet_buf[])
{
    int index = 0;

    bzero(udp_packet_buf, BUFSIZE);
    udp_packet_buf[index++] = 0x00;
    udp_packet_buf[index++] = (unsigned char)strlen(buf);
    for (long unsigned int i = 0; i < strlen(buf); i++)
    {
        udp_packet_buf[index++] = buf[i];
    }
    udp_packet_buf[index] = '\n';
}

int main(int argc, const char *argv[])
{
    int port_number, bytestx, bytesrx;
    socklen_t serverlen;
    const char *server_hostname;
    struct hostent *server;
    struct sockaddr_in server_address;
    char buf[BUFSIZE];
    struct timeval timeout = {7, 0}; // Set timeout for 7 seconds

    // Control if program was called correctly
    bool arguments_are_valid = check_argument_format(argc, argv);

    if (arguments_are_valid)
    {
        server_hostname = argv[2];
        port_number = atoi(argv[4]);

        // Checking correct mode format
        if (!strcmp(argv[6], "tcp"))
        {
            mode = tcp;
        }
        else
        {
            mode = udp;
        }
    }
    else
    {
        fprintf(stderr, "[ ERROR ] Wrong script calling\n");
        fprintf(stderr, "Usage: %s -h <host> -p <port> -m <mode>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Obtaining the server address using DNS
    if ((server = gethostbyname(server_hostname)) == NULL)
    {
        fprintf(stderr, "[ ERROR ] No such host as %s\n", server_hostname);
        exit(EXIT_FAILURE);
    }

    // Finding server IP addresses and initializing the server_address structure
    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(port_number);

    switch (mode)
    {
    case tcp:

        // Socket creation
        if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
        {
            perror("[ ERROR ] Socket");
            exit(EXIT_FAILURE);
        }

        // Establishing connection
        if (connect(client_socket, (const struct sockaddr *)&server_address, sizeof(server_address)) != 0)
        {
            perror("[ ERROR ] Connect");
            exit(EXIT_FAILURE);
        }

        char end_message[] = "BYE\n";

        while (true)
        {
            signal(SIGINT, sig_handler);

            // Loading users message
            bzero(buf, BUFSIZE);
            if (!fgets(buf, BUFSIZE, stdin))
            {
                fprintf(stderr, "[ ERROR ] No input");
                close(client_socket);
                exit(EXIT_FAILURE);
            }

            // User didn't type any request
            if (!strcmp(buf, "\n"))
            {
                fprintf(stderr, "[ ERROR ] No input\n");
                strcpy(buf, end_message);
                printf("%s", buf);
            }

            // Sending users message to the server
            bytestx = send(client_socket, buf, strlen(buf), 0);
            if (bytestx < 0)
            {
                perror("[ ERROR ] In sendto");
            }
            // Recieving and displaying servers response
            bzero(buf, BUFSIZE);
            bytesrx = recv(client_socket, buf, BUFSIZE, 0);
            if (bytesrx < 0)
            {
                perror("[ ERROR ] In recvfrom");
            }

            // Displaying servers response
            if (!strcmp(buf, end_message))
            {
                printf("BYE");
                break;
            }
            else
            {
                printf("%s", buf);
            }
        }

        close(client_socket);
        break;

    case udp:
        signal(SIGINT, sig_handler);
        // Socket creation
        if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
        {
            perror("[ ERROR ] Socket");
            exit(EXIT_FAILURE);
        }

        char udp_packet_buf[BUFSIZE];
        bool file_is_empty = true; //variable that determines if file is empty *

        while (true)
        {

            // Loading users message
            bzero(buf, BUFSIZE);
            if (!fgets(buf, BUFSIZE, stdin))
            {   
                if(file_is_empty) { //
                    fprintf(stderr, "[ ERROR ] No input");
                    exit(EXIT_FAILURE);
                } break; //
            }
            file_is_empty = false; //first line is not EOF *

            if (!strcmp(buf, "\n"))
            {
                // Users message is empty
                fprintf(stderr, "[ ERROR ] No input");
                exit(EXIT_FAILURE);
            }

            // Converting message to udp packet
            buf_to_udp(buf, udp_packet_buf);

            // Sending users message to the server
            serverlen = sizeof(server_address);
            bytestx = sendto(client_socket, udp_packet_buf, strlen(buf) + 2, 0, (struct sockaddr *)&server_address, serverlen);
            if (bytestx < 0)
            {
                perror("[ ERROR ] Sendto");
            }

            // Setting timeout interval
            setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));

            // Recieving servers response
            bzero(udp_packet_buf, BUFSIZE);
            bytesrx = recvfrom(client_socket, udp_packet_buf, BUFSIZE, 0, (struct sockaddr *)&server_address, &serverlen);
            if (bytesrx < 0)
            {
                perror("[ ERROR ] Time out");
            }

            // Displaying servers response
            if (udp_packet_buf[1] == 0x00)
            {
                printf("OK: ");
                print_udp_packet(udp_packet_buf);
                printf("\n");
            }
            else
            {
                fprintf(stderr, "Err: ");
                print_udp_packet(udp_packet_buf);
            }

            if (feof(stdin))
            {
                break;
            }
        }
        close(client_socket);

        break;
    }

    return 0;
}
