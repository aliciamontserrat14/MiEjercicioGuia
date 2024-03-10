#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h> // for toupper function

int main(int argc, char* argv[])
{
    int sock_conn, sock_listen, ret;
    struct sockaddr_in serv_adr;
    char peticion[512];
    char respuesta[512];
    // INICIALIZATIONS
    // Open the socket
    if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        printf("Error creating socket");
    // Bind to the port
    memset(&serv_adr, 0, sizeof(serv_adr)); // initialize serv_addr to zero
    serv_adr.sin_family = AF_INET;
    // Bind to any IP of the machine.
    // htonl formats the number received into the necessary format
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    // We will listen on port 9050
    serv_adr.sin_port = htons(9050);
    if (bind(sock_listen, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) < 0)
        printf("Error binding");
    // The queue of pending requests cannot be greater than 4
    if (listen(sock_listen, 4) < 0)
        printf("Error in listen");
    int i;
    // We will handle only 10 requests
    for (i = 0; i < 10; i++)
    {
        printf("Listening\n");

        sock_conn = accept(sock_listen, NULL, NULL);
        printf("Connection received\n");
        // sock_conn is the socket we will use for this client

        // Now we receive its request
        ret = read(sock_conn, peticion, sizeof(peticion));
        printf("Request received\n");
        // We have to add the null terminator
        // to prevent writing what's after in the buffer
        peticion[ret] = '\0';

        // Write the request to the console
        printf("The request is: %s\n", peticion);
        char* p = strtok(peticion, "/");
        int codigo = atoi(p);
        p = strtok(NULL, "/");
        char nombre[20];
        strcpy(nombre, p);
        printf("Code: %d, Name: %s\n", codigo, nombre);

        // Perform actions based on the request code
        if (codigo == 1) // Request for the length of the name
            sprintf(respuesta, "%d", strlen(nombre));
        else if (codigo == 2) // Check if the name is beautiful
        {
            if ((nombre[0] == 'M') || (nombre[0] == 'S'))
                strcpy(respuesta, "YES");
            else
                strcpy(respuesta, "NO");
        }
        else if (codigo == 3) // Check if the name is a palindrome
        {
            int len = strlen(nombre);
            int isPalindrome = 1; // Assume it's a palindrome initially

            // Compare characters from start and end
            for (int j = 0; j < len / 2; j++)
            {
                if (tolower(nombre[j]) != tolower(nombre[len - j - 1])) // Compare ignoring case
                {
                    isPalindrome = 0; // Not a palindrome
                    break;
                }
            }

            if (isPalindrome)
                strcpy(respuesta, "YES");
            else
                strcpy(respuesta, "NO");
        }
        else if (codigo == 4) // Return the name in uppercase
        {
            for (int j = 0; nombre[j] != '\0'; j++)
                nombre[j] = toupper(nombre[j]);
            strcpy(respuesta, nombre);
        }
        else
        {
            strcpy(respuesta, "Invalid request code");
        }

        // Send the response
        write(sock_conn, respuesta, strlen(respuesta));

        // End the service for this client
        close(sock_conn);
    }
}

}
