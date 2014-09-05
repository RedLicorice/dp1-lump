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

/** @brief Riceve una stringa.
 * @param sockfd Il file descriptor del socket da cui leggere i dati.
 * @retval buffer Il buffer in cui scrivere la stringa ricevuta (dimensione @p charCount). Se è uguale a NULL, il buffer viene allocato dinamicamente.
 * @param maxCharCount Il numero massimo di caratteri della stringa da ricevere (incluso @c \\0).
 * @retval sourceStruct La struttura contenente le informazioni sulla sorgente da cui i dati sono ricevuti. Se viene passato NULL, questo parametro viene ignorato.
 * @retval readCharCount Il numero di caratteri letti (escluso @c \\0). Se viene passato NULL, questo parametro viene ignorato.
 * @return Restituisce true se tutta la stringa è stata letta. Restituisce false se è stato raggiunto l'end-of-file (numero di caratteri letti minore di @p charCount).
 */
bool myUdpReadString(SOCKET sockfd, char *buffer, int maxCharCount, struct sockaddr_in *sourceStruct, int *readCharCount);

/** @brief Invia una stringa.
 * @param sockfd Il file descriptor del socket su cui scrivere i dati.
 * @param string Il buffer da cui leggere la stringa da inviare.
 * @param destStruct La struttura contenente le informazioni sulla destinazione a cui inviare i dati.
 */
void myUdpWriteString(SOCKET sockfd, char *string, struct sockaddr_in destStruct);

#endif