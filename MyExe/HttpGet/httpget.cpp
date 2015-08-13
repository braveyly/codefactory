/*
   httpget -- a command-line program for running scripts on a remote web server.
   Copyright (C) 2001 lookandfeel new media, LLC

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   http://freesoftware.lookandfeel.com/
   http://www.lookandfeel.com/
 */
#ifdef _WIN32
// Windows NT

#ifndef CLOSE_SOCKET
#define CLOSE_SOCKET closesocket
#endif // CLOSE_SOCKET
#ifndef IOCTL
#define IOCTL ioctlsocket
#endif // IOCTL

#ifdef _AFXDLL
#include "stdafx.h"
#include <winsock2.h>
#else
#include <winsock2.h>
#include <windows.h>
#endif //_AFXDLL
#include <process.h>

#else
// Linux

#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif // SOCKET_ERROR
#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif // INVALID_SOCKET
#ifndef SOCKET
#define SOCKET int
#endif // SOCKET
#ifndef SD_SEND
#define SD_SEND 1
#endif // SD_SEND
#ifndef CLOSE_SOCKET
#define CLOSE_SOCKET close
#endif // CLOSE_SOCKET
#ifndef IOCTL
#define IOCTL ioctl
#endif // IOCTL

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>

#endif // _WIN32

#include <stdio.h>
#include <time.h>
#include <string.h>

#ifdef __APPLE__
// Mac OS X

#include <stdlib.h>
#include <sys/select.h>

#else

#include <malloc.h>

#endif // __APPLE__

struct sockaddr_in target_server_address;

/*
 * Function: void log(int level, char *message)
 * Description: log function
 * Input:  level: log level
 *         message: log message
 * Output: none
 * Return: void
 * Others: none
 */
void log(int level, char *message)
{
  time_t tmp_time = time(NULL);
  char *tmp_ctime = ctime(&tmp_time);

  tmp_ctime[strlen(tmp_ctime) - 1] = '\0';

  printf("%s ", tmp_ctime);
  printf(message);

  return;
}

/*
 * Function: void print_usage(char *message)
 * Description: print help info
 * Input:  message: help info
 * Output: none
 * Return: void
 * Others: none
 */
void print_usage(char *message)
{
  time_t tmp_time = time(NULL);
  char *tmp_ctime = ctime(&tmp_time);

  tmp_ctime[strlen(tmp_ctime) - 1] = '\0';

  if (message != NULL) {
    printf("%s ", tmp_ctime);
    printf("ERROR: %s\n\n", message);
  }

  printf("USAGE: httpget http://[USERNAME:PASSWORD@]HOSTNAME[:PORT]/PATH [TIMEOUT]\n\n"
         "HOSTNAME is the name or IP address of the remote web server.  PORT is the\n"
         "port number to connect to (optional).  PATH is the path to the page to\n"
         "fetch.  PATH may include querystring parameters.  If PATH is not provided,\n"
         "the URL must end in a forward slash (/).  NOTE: httpget does not support\n"
         "SSL.\n\n"
         "USERNAME and PASSWORD are optional.  If given, they must be separated by a\n"
         "colon and the password must be followed by an \"at\" symbol.  When they are\n"
         "present, they will be provided to the remote server in a Basic Authentication\n"
         "header, whether authentication is required or not.\n\n"
         "httpget will wait up to TIMEOUT minutes for a complete response from the\n"
         "remote server.  TIMEOUT is optional; if it is not given, httpget defaults to\n"
         "12 hours.\n\n"
         "httpget is intended to be used to fetch a web page from a remote host and\n"
         "remain connected to the host long enough to allow the page to completely\n"
         "execute.  This makes it perfect for fetching pages that perform some kind of\n"
         "periodic processing on a schedule.\n\n"
         "httpget will print any errors to stdout, preceeded by the current date/time\n"
         "and the word \"ERROR:\".  Any usage errors will be followed by this usage\n"
         "message.  In the absence of errors, httpget will print informational\n"
         "messages preceeded by the current date/time and \"HTTPGET:\" to stdout.  It\n"
         "will then print the response it receives from the remote server to stdout.\n\n"
         "httpget will always begin its output with the current date/time and the\n"
         "message \"HTTPGET: OUTPUT START\".  It will always terminate its output with\n"
         "the current date/time and the message \"HTTPGET: OUTPUT END\".\n");

  return;
}

/*
 * Function: int base64_encode(char **return_value, char *input_string)
 * Description: encode by base 64 
 * Input:  return_value:encode result
 *         input_string:input string
 * Output: none
 * Return: int
 * Others: none
 */
int base64_encode(char **return_value, char *input_string)
{
  const char *base64_alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  const char base64_pad_char = '=';
  unsigned int strlen_input_string;
  char *tmp_text = NULL,
  *cur_new,
  *cur_old;

  if (input_string != NULL) {
    strlen_input_string = strlen(input_string);
    tmp_text = (char *)malloc((((strlen_input_string + 2) * 4) / 3) + 1);
    cur_new = tmp_text;
    cur_old = input_string;

    while ((strlen_input_string - (cur_old - input_string)) >= 3) {
      cur_new[0] = base64_alphabet[(cur_old[0] & (unsigned char)0xFC) >> 2];
      cur_new[1] = base64_alphabet[((cur_old[0] & (unsigned char)0x03) << 4) | ((cur_old[1] & (unsigned char)0xF0) >> 4)];
      cur_new[2] = base64_alphabet[((cur_old[1] & (unsigned char)0x0F) << 2) | ((cur_old[2] & (unsigned char)0xC0) >> 6)];
      cur_new[3] = base64_alphabet[cur_old[2] & (unsigned char)0x3F];

      cur_new += 4;
      cur_old += 3;
    }

    switch (strlen_input_string - (cur_old - input_string)) {
    case 1:
      cur_new[0] = base64_alphabet[(cur_old[0] & (unsigned char)0xFC) >> 2];
      cur_new[1] = base64_alphabet[(cur_old[0] & (unsigned char)0x03) << 4];
      cur_new[2] = base64_pad_char;
      cur_new[3] = base64_pad_char;
      cur_new += 4;

      break;
    case 2:
      cur_new[0] = base64_alphabet[(cur_old[0] & (unsigned char)0xFC) >> 2];
      cur_new[1] = base64_alphabet[((cur_old[0] & (unsigned char)0x03) << 4) | ((cur_old[1] & (unsigned char)0xF0) >> 4)];
      cur_new[2] = base64_alphabet[(cur_old[1] & (unsigned char)0x0F) << 2];
      cur_new[3] = base64_pad_char;
      cur_new += 4;
    }

    cur_new[0] = '\0';
    *return_value = tmp_text;
  }

  return((tmp_text == NULL) ? 0 : 1);
}

/*
 * Function: int main(int argc, char args[])
 * Description: process main function
 * Input:  argc: parameter number
 *         args: parameter value array
 * Output: none
 * Return: function exit status
 * Others: none
 */
int main(int argc, char *argv[])
{
  struct hostent *tmp_hostent;
  unsigned long tmp_address = INADDR_NONE;
  long response_len = 0,
       timeout_seconds = 43200;
  char server_name[4096],
       host_name[4096],
       page_name[4096],
       username_password[1024],
       send_message[10000],
       tmp_response[65536],
  *encoded_username_password;
  int port,
      octets[4],
      error_occurred = 0;
  SOCKET tmp_socket = INVALID_SOCKET;
  fd_set tmp_fd_set;
  struct timeval tmp_timeval;
  time_t start_time;

#ifdef _WIN32
// Windows NT
  unsigned long one = 1;
  WSADATA tmp_wsadata;

  WSAStartup(MAKEWORD(2, 2), &tmp_wsadata);

#endif

  host_name[0] = '\0';
  port = 80;

  log(0, "HTTPGET: OUTPUT BEGIN\n");

  if ((argc < 2) ||
      (argc > 3)) {
    print_usage(NULL);
    error_occurred = 1;
  }
  else {
    if (sscanf(argv[1], "http://%[^@]@%[^/]%s", username_password, server_name, page_name) != 3)
      if (sscanf(argv[1], "http://%[^/]%s", server_name, page_name) == 2)
        username_password[0] = '\0';
      else
        error_occurred = 1;

    if (!error_occurred) {
      if (sscanf(server_name, "%[^:]:%d", host_name, &port) == 2) {
        if ((strlen(host_name) == 0) ||
            (port <= 0) ||
            (port >= 65536)) {
          print_usage("The server name must not be zero-length and the port number must be between 1 and 65535, inclusive.");
          error_occurred = 1;
        }
      }
      else {
        strcpy(host_name, server_name);
        port = 80;
      }

      if (!error_occurred &&
          (argc == 3))
        if ((sscanf(argv[2], "%ld", &timeout_seconds) == 1) &&
            (timeout_seconds > 0))
          timeout_seconds *= 60;
        else {
          print_usage("The timeout value must be greater than 0.");
          error_occurred = 1;
        }

      if (!error_occurred) {
        target_server_address.sin_family = AF_INET;
        target_server_address.sin_port = htons((unsigned short)port);

        if ((sscanf(host_name, "%d.%d.%d.%d", &octets[0], &octets[1], &octets[2], &octets[3]) == 4) &&
            (octets[0] >= 0) &&
            (octets[0] <= 255) &&
            (octets[1] >= 0) &&
            (octets[1] <= 255) &&
            (octets[2] >= 0) &&
            (octets[2] <= 255) &&
            (octets[3] >= 0) &&
            (octets[3] <= 255))
          if ((tmp_address = inet_addr(host_name)) != INADDR_NONE) {
            memcpy(&target_server_address.sin_addr, &tmp_address, sizeof(unsigned long));

            sprintf(send_message, "HTTPGET: USING SERVER %s\n", host_name);
            log(0, send_message);
            sprintf(send_message, "HTTPGET: USING PORT %d\n", port);
            log(0, send_message);
            sprintf(send_message, "HTTPGET: FETCHING PAGE %s\n", page_name);
            log(0, send_message);
            sprintf(send_message, "HTTPGET: USING TIMEOUT OF %ld MINUTES\n", timeout_seconds / 60);
            log(0, send_message);
          }
          else {
            log(0, "The IP address provided is invalid.\n");
            error_occurred = 1;
          }
        else
        if ((tmp_hostent = gethostbyname(host_name)) != NULL) {
          memcpy(&target_server_address.sin_addr, tmp_hostent->h_addr_list[0], tmp_hostent->h_length);

          sprintf(send_message, "HTTPGET: USING SERVER %s (%s)\n", host_name, inet_ntoa(target_server_address.sin_addr));
          log(0, send_message);
          sprintf(send_message, "HTTPGET: USING PORT %d\n", port);
          log(0, send_message);
          sprintf(send_message, "HTTPGET: FETCHING PAGE %s\n", page_name);
          log(0, send_message);
          sprintf(send_message, "HTTPGET: USING TIMEOUT OF %ld MINUTES\n", timeout_seconds / 60);
          log(0, send_message);
        }
        else {
          log(0, "ERROR: Unable to resolve host name.\n");
          error_occurred = 1;
        }
      }

      if (!error_occurred) {
        if (strlen(username_password) > 0) {
          sprintf(send_message, "HTTPGET: USING BASIC AUTHENTICATION\n");
          log(0, send_message);

          base64_encode(&encoded_username_password, username_password);
          sprintf(send_message, "GET %s HTTP/1.0\r\nHost: %s\r\nAuthorization: Basic %s\r\n\r\n", page_name, host_name, encoded_username_password);
        }
        else {
          sprintf(send_message, "HTTPGET: NOT USING BASIC AUTHENTICATION\n");
          log(0, send_message);

          sprintf(send_message, "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", page_name, host_name);
        }

        start_time = time(NULL);

        if ((tmp_socket = socket(AF_INET, SOCK_STREAM, 0)) != INVALID_SOCKET) {
#ifdef _WIN32
// Windows NT

          if (IOCTL(tmp_socket, FIONBIO, &one) == 0)

#endif // _WIN32

          if ((connect(tmp_socket, (struct sockaddr *)&target_server_address, sizeof(struct sockaddr_in)) != SOCKET_ERROR)

#ifdef _WIN32
// Windows NT

              || (WSAGetLastError() == WSAEWOULDBLOCK)

#endif // _WIN32

              ) {
            tmp_timeval.tv_sec = timeout_seconds;
            tmp_timeval.tv_usec = 0;

            FD_ZERO(&tmp_fd_set);
            FD_SET(tmp_socket, &tmp_fd_set);

            switch (select(tmp_socket + 1, NULL, &tmp_fd_set, NULL, &tmp_timeval)) {
            case SOCKET_ERROR:
              log(0, "ERROR: Unable to connect.\n");
              error_occurred = 1;
              break;
            case 0:
              log(0, "ERROR: Timeout waiting for connection.\n");
              error_occurred = 1;
              break;
            default:
              if (send(tmp_socket, send_message, strlen(send_message), 0) != SOCKET_ERROR) {
                do {
                  tmp_timeval.tv_sec = timeout_seconds - (time(NULL) - start_time);
                  tmp_timeval.tv_usec = 0;

                  FD_ZERO(&tmp_fd_set);
                  FD_SET(tmp_socket, &tmp_fd_set);

                  switch (select(tmp_socket + 1, &tmp_fd_set, NULL, NULL, &tmp_timeval)) {
                  case SOCKET_ERROR:
                    log(0, "ERROR: Unable to receive.\n");
                    error_occurred = 1;
                    break;
                  case 0:
                    log(0, "ERROR: Timeout waiting for response.\n");
                    error_occurred = 1;
                    break;
                  default:
                    if ((response_len = recv(tmp_socket, tmp_response, 65535, 0)) > 0) {
                      tmp_response[response_len] = '\0';
                      //printf(tmp_response);
                    }
                    else
                      printf("\n");
                  }
                }
                while ((response_len > 0) &&
                       !error_occurred &&
                       ((time(NULL) - start_time) < timeout_seconds));
              }
              else {
                log(0, "ERROR: Unable to send request.\n");
                error_occurred = 1;
              }
            }
          }
          else
            log(0, "ERROR: Unable to connect to remote host.\n");

#ifdef _WIN32
// Windows NT

          else
            log(0, "ERROR: Unable to set socket properties.\n");

#endif // _WIN32

          if ((shutdown(tmp_socket, SD_SEND) == SOCKET_ERROR) ||
              (CLOSE_SOCKET(tmp_socket) == SOCKET_ERROR)) {
            log(0, "ERROR: Unable to close connection.\n");
            error_occurred = 1;
          }
        }
        else
          log(0, "ERROR: Unable to create socket.\n");
      }
    }
    else {
      print_usage("URL does not match the required format.\n");
      error_occurred = 1;
    }
  }

  log(0, "HTTPGET: OUTPUT END\n");

#ifdef _WIN32
// Windows NT

  WSACleanup();

#endif // _WIN32

  return(0);
}
/*
 * End of file
 */
