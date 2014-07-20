/** @file
  * @brief Contiene i wrapper delle funzioni per i socket connessi TCP.
  */

#ifndef	__lunp_lg_tcp_h
#define	__lunp_lg_tcp_h

/** @brief Crea un socket TCP e si connette al server specificato.
  * @param serverAddress L'indirizzo del server a cui connettersi.
  * @param serverPort La porta del server a cui connettersi.
  * @return In caso di successo, restituisce il file descriptor del socket TCP creato e connesso.
  */
SOCKET myTcpClientStartup(const char *serverAddress, const char *serverPort);

/** @brief Crea un socket TCP associato alla porta specificata. Non chiama la funzione Listen().
  * @param serverPort La porta del server a cui associare il socket creato.
  * @return In caso di successo, restituisce il file descriptor del socket TCP creato.
  */
SOCKET myTcpServerStartup(const char *serverPort);

/** @brief Riceve fino a @p byteCount byte di dati.
  * @param sockfd Il file descriptor del socket da cui leggere i dati.
  * @retval buffer Il buffer in cui scrivere i dati ricevuti (dimensione @p byteCount). Se è uguale a NULL, il buffer viene allocato dinamicamente.
  * @param byteCount Il numero massimo di byte dei dati da ricevere.
  * @retval readByteCount In caso di successo, restituisce il numero di byte letti. Se viene passato NULL, questo parametro viene ignorato.
  * @return Restituisce true se tutti i byte sono stati letti, false se è stato raggiunto l'end-of-file (numero di byte letti minore di @p byteCount).
  */
bool myTcpReadBytes(SOCKET sockfd, void *buffer, int byteCount, int *readByteCount);

/** @brief Invia @p byteCount byte di dati.
 * @param sockfd Il file descriptor del socket su cui scrivere i dati.
 * @param data Il buffer da cui leggere i dati da inviare (dimensione @p byteCount).
 * @param byteCount Il numero di byte dei dati da inviare.
 */
void myTcpWriteBytes(SOCKET sockfd, void *data, int byteCount);

/** @brief Riceve una stringa.
  * @param sockfd Il file descriptor del socket da cui leggere i dati.
  * @retval buffer Il buffer in cui scrivere la stringa ricevuta (dimensione @p charCount + 1). Se è uguale a NULL, il buffer viene allocato dinamicamente.
  * @param charCount Il numero massimo di caratteri della stringa da ricevere (escluso @p \\0).
  * @retval readCharCount In caso di successo, restituisce il numero di caratteri letti (escluso @p \\0). Se viene passato NULL, questo parametro viene ignorato.
  * @return Restituisce true se tutti i caratteri sono stati letti, false se è stato raggiunto l'end-of-file (numero di caratteri letti minore di @p charCount).
  */
bool myTcpReadString(SOCKET sockfd, char *buffer, int charCount, int *readCharCount);

/** @brief Invia una stringa.
 * @param sockfd Il file descriptor del socket su cui scrivere i dati.
 * @param string Il buffer da cui leggere la stringa da inviare.
 */
void myTcpWriteString(SOCKET sockfd, char *string);

/** @brief Riceve una riga terminata con @p \\n. La lettura non è bufferizzata.
 * @param sockfd Il file descriptor del socket da cui leggere la riga.
 * @retval buffer Il buffer in cui scrivere la riga ricevuta (lunghezza @p maxLength + 1). Se è uguale a NULL, il buffer viene allocato dinamicamente.
 * @param maxLength Il numero massimo di caratteri della riga da ricevere (escluso @p \\0).
 * @return In caso di successo, restituisce il numero di caratteri letti (escluso @p \\0).
 */
ssize_t myTcpReadLine(SOCKET sockfd, char *buffer, int maxLength);

/** @brief Riceve una riga terminata con @p \\n. La lettura è bufferizzata.
 * @param sockfd Il file descriptor del socket da cui leggere la riga.
 * @retval buffer Il buffer in cui scrivere la riga ricevuta (lunghezza @p maxLength + 1). Se è uguale a NULL, il buffer viene allocato dinamicamente.
 * @param maxLength Il numero massimo di caratteri della riga da ricevere (escluso @p \\0).
 * @return In caso di successo, restituisce il numero di caratteri letti (escluso @p \\0).
 * @warning Non mischiare questa funzione con le funzioni di lettura non bufferizzate.
 */
ssize_t myTcpBufferedReadLine(SOCKET sockfd, char *buffer, int maxLength);

#endif