/** @file
  * @brief Contiene i wrapper delle funzioni per i socket.
  */

#ifndef	__lunp_lg_socket_h
#define	__lunp_lg_socket_h

/** @brief Chiude il socket specificato.
  * @param sockfd Il file descriptor del socket da chiudere.
  */
void myClose(SOCKET sockfd);

#endif