// Contains defs for standard packet io protocol
// To read floating point numbers, you need to change your arduino configuration settings


#ifndef _TELEMETRY_H
#define _TELEMETRY_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#define PORT 5000
#define BUFFER_SIZE 1024

int client_fd = 0;

void wait_for_connection() {
  int server_fd = 0;
  struct sockaddr_in address;
  int addrlen = sizeof(address);

  // Create a TCP socket
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
      perror("socket failed");
      exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  // Bind the socket to the specified port
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
      perror("bind failed");
      exit(EXIT_FAILURE);
  }

  // Listen for incoming connections
  if (listen(server_fd, 3) < 0) {
      perror("listen");
      exit(EXIT_FAILURE);
  }

  if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
      perror("accept");
  } else {
      // Set the new client socket to be non-blocking
      if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0) {
          perror("fcntl");
          exit(EXIT_FAILURE);
      }
  }
  }

#define BEGIN_SEND {                            \
  char buffer[BUFFER_SIZE] = {0};               \
  strcpy(buffer, "@@@@@_time:");                \
  sprintf(buffer + strlen(buffer), "%d", millis());
  
#define SEND_ITEM(field, value, format)         \
  strcat(buffer, ";");                          \
  strcat(buffer, #field);                       \
  strcat(buffer, ":");                          \
  sprintf(buffer + strlen(buffer), format, value);
  
#define SEND_GROUP_ITEM(value, format)          \
  strcat(buffer, ",");                          \
  sprintf(buffer + strlen(buffer), format, value);
  
#define SEND_ITEM_NAME(field, value, format)    \
  strcat(buffer, ";");                          \
  strcat(buffer, field);                        \
  strcat(buffer, ":");                          \
  sprintf(buffer + strlen(buffer), format, value);

#define END_SEND                                \
  strcat(buffer, "&&&&&");                      \
  if (write(client_fd, buffer, strlen(buffer)) == -1)\
    perror("write");                              \
  }

#define SEND(field, value, format)              \
  BEGIN_SEND                                    \
  SEND_ITEM(field, value, format)               \
    END_SEND

#define SEND_NAME(field, value, format)         \
  BEGIN_SEND                                    \
  SEND_ITEM_NAME(field, value, format)          \
    END_SEND

// Sorry about the gotos, only needed because macros.  
#define BEGIN_READ                                                      \
  char _buffer[BUFFER_SIZE];                                            \
  char _data[BUFFER_SIZE - 10];                                         \
  if (read(client_fd, _buffer, BUFFER_SIZE) > 0) {                      \
    if (!sscanf(_buffer, "@@@@@%[^&]&&&&&", _data)) {                   \
      printf("READ packet error");                 \
      goto L_ENDREAD;                                                   \
    }                                                                   \
    if (0);

#define READ_FIELD(field, spec, var)            \
  else if (sscanf(_data, #field":" spec, &var))

#define READ_FLAG(field)                        \
  else if (!strcmp(_data, #field":"))

#define READ_DEFAULT(field_name, var)                           \
  else if (sscanf(_data, "%[^:]:%s", field_name, var))

#define END_READ } L_ENDREAD:;

#endif
