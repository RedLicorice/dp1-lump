/** @file
 * @brief Contiene i wrapper delle funzioni per i socket non connessi UDP.
 */

#ifndef	__lunp_lg_udp_h
#define	__lunp_lg_udp_h

/** @brief Crea un socket UDP.
 * @param serverAddress L'indirizzo del server.
 * @param serverPort La porta del server.
 * @retval serverStruct La struttura contenente le informazioni sul server, per le chiamate future alla funzione myUdpSend().
 * @return Restituisce il file descriptor del socket UDP creato.
 */
SOCKET myUdpClientStartup(const char *serverAddress, const char *serverPort, struct sockaddr_in *serverStruct);

/** @brief Crea un socket UDP associato alla porta specificata.
 * @param serverPort La porta del server a cui associare il socket creato.
 * @return Restituisce il file descriptor del socket UDP creato.
 */
SOCKET myUdpServerStartup(const char *serverPort);

/** @brief Riceve fino a @p byteCount byte di dati.
 * @param sockfd Il file descriptor del socket da cui leggere i dati.
 * @retval buffer Il buffer in cui scrivere i dati ricevuti (dimensione @p byteCount). Se è uguale a NULL, il buffer viene allocato dinamicamente.
 * @param maxByteCount Il numero massimo di byte di dati da ricevere.
 * @retval sourceStruct La struttura contenente le informazioni sulla sorgente da cui i dati sono ricevuti. Se viene passato NULL, questo parametro viene ignorato.
 * @retval readByteCount Il numero di byte letti. Se viene passato NULL, questo parametro viene ignorato.
 * @return Restituisce true se tutti i dati sono stati letti. Restituisce false se è stato raggiunto l'end-of-file (numero di byte letti minore di @p byteCount).
 */
bool myUdpReadBytes(SOCKET sockfd, void *buffer, int maxByteCount, struct sockaddr_in *sourceStruct, int *readByteCount);

/** @brief Invia @p byteCount byte di dati.
 * @param sockfd Il file descriptor del socket su cui scrivere i dati.
 * @param data Il buffer da cui leggere i dati da inviare (dimensione @p byteCount).
 * @param byteCount Il numero di byte di dati da inviare.
 * @param destStruct La struttura contenente le informazioni sulla destinazione a cui inviare i dati.
 */
void myUdpWriteBytes(SOCKET sockfd, void *data, int byteCount, struct sockaddr_in destStruct);

/** @brief Verifica che non siano stati ricevuti più di @p maxDatagrams datagrammi dal client @p clientStruct.
 * @param clientStruct La struttura contenente le informazioni sul client da cui i dati sono stati ricevuti.
 * @param maxDatagrams Il numero massimo di datagrammi che può essere ricevuto dal client @p clientStruct.
 * @param maxClients Il numero massimo di client da riconoscere.
 * @returns Restituisce false se sono stati ricevuti più di @p maxDatagrams datagrammi dal client @p clientStruct, altrimenti restituisce true.
 */
bool myUdpLimitClients(struct sockaddr_in clientStruct, int maxDatagrams, int maxClients);

#endif