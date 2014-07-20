/** @file
  * @brief Contiene i wrapper delle funzioni per i socket non connessi UDP.
  */

#ifndef	__lunp_lg_udp_h
#define	__lunp_lg_udp_h

/** @brief Crea un socket UDP.
  * @param serverAddress L'indirizzo del server.
  * @param serverPort La porta del server.
  * @retval serverStruct La struttura contenente le informazioni sul server, per le chiamate future alla funzione myUdpSend().
  * @return In caso di successo, restituisce il file descriptor del socket UDP creato.
  */
SOCKET myUdpClientStartup(const char *serverAddress, const char *serverPort, struct sockaddr_in *serverStruct);

/** @brief Crea un socket UDP associato alla porta specificata.
  * @param serverPort La porta del server a cui associare il socket creato.
  * @return In caso di successo, restituisce il file descriptor del socket UDP creato.
  */
SOCKET myUdpServerStartup(const char *serverPort);

#endif