/** @file
 * @brief Contiene i wrapper delle funzioni per i socket connessi TCP.
 */

#ifndef	__lunp_lg_tcp_h
#define	__lunp_lg_tcp_h

/** @brief Crea un socket TCP e si connette al server specificato.
 * @param serverAddress L'indirizzo del server a cui connettersi.
 * @param serverPort La porta del server a cui connettersi.
 * @return Restituisce il file descriptor del socket TCP creato e connesso.
 */
SOCKET myTcpClientStartup(const char *serverAddress, const char *serverPort);

/** @brief Crea un socket TCP associato alla porta specificata.
 * @param serverPort La porta del server a cui associare il socket creato.
 * @return Restituisce il file descriptor del socket TCP creato.
 */
SOCKET myTcpServerStartup(const char *serverPort);

/** @brief La funzione che viene chiamata dalla funzione @c myTcpServerSimple ogni volta che un client si connette al server.
 * @param sockfd Il file descriptor del socket a cui il client da servire è conesso.
 */
typedef void (*myTcpServerChildTask)(SOCKET sockfd);

/** @brief Implementa un server TCP che serve un client per volta.
 * @param sockfd Il file descriptor del socket a cui i client si connettono.
 * @param childTask La funzione da chiamare ogni volta che un client si connette al socket @p sockfd.
 * @warning Questa funzione non ritorna mai.
 */
void myTcpServerSimple(SOCKET sockfd, myTcpServerChildTask childTask);

/** @brief Accetta una connessione sul socket TCP specificato.
 * @param sockfd Il file descriptor del socket TCP su cui accettare la connessione.
 * @retval clientStruct La struttura contenente le informazioni sul client.
 * @return Restituisce il file descriptor del socket TCP accettato.
 */
SOCKET myTcpServerAccept(SOCKET sockfd, struct sockaddr_in *clientStruct);

/** @brief Riceve fino a @p byteCount byte di dati.
 * @param sockfd Il file descriptor del socket da cui leggere i dati.
 * @retval buffer Il buffer in cui scrivere i dati ricevuti (dimensione @p byteCount). Se è uguale a NULL, il buffer viene allocato dinamicamente.
 * @param byteCount Il numero di byte di dati da ricevere.
 * @retval readByteCount Il numero di byte letti. Se viene passato NULL, questo parametro viene ignorato.
 * @return Restituisce true se tutti i dati sono stati letti, false se è stato raggiunto l'end-of-file (numero di byte letti minore di @p byteCount).
 */
bool myTcpReadBytes(SOCKET sockfd, void *buffer, int byteCount, int *readByteCount);

/** @brief Invia @p byteCount byte di dati.
 * @param sockfd Il file descriptor del socket su cui scrivere i dati.
 * @param data Il buffer da cui leggere i dati da inviare (dimensione @p byteCount).
 * @param byteCount Il numero di byte di dati da inviare.
 */
void myTcpWriteBytes(SOCKET sockfd, void *data, int byteCount);

/** @brief Riceve una stringa.
 * @param sockfd Il file descriptor del socket da cui leggere i dati.
 * @retval buffer Il buffer in cui scrivere la stringa ricevuta (dimensione @p charCount + 1). Se è uguale a NULL, il buffer viene allocato dinamicamente.
 * @param charCount Il numero di caratteri della stringa da ricevere (escluso @c \\0).
 * @retval readCharCount Il numero di caratteri letti (escluso @c \\0). Se viene passato NULL, questo parametro viene ignorato.
 * @return Restituisce true se tutta la stringa è stata letta, false se è stato raggiunto l'end-of-file (numero di caratteri letti minore di @p charCount).
 */
bool myTcpReadString(SOCKET sockfd, char *buffer, int charCount, int *readCharCount);

/** @brief Invia una stringa.
 * @param sockfd Il file descriptor del socket su cui scrivere i dati.
 * @param string Il buffer da cui leggere la stringa da inviare.
 */
void myTcpWriteString(SOCKET sockfd, char *string);

/** @brief Riceve una riga terminata con il carattere @c \\n. La lettura non è bufferizzata.
 * @param sockfd Il file descriptor del socket da cui leggere la riga.
 * @retval buffer Il buffer in cui scrivere la riga ricevuta (lunghezza @p maxLength + 1). Se è uguale a NULL, il buffer viene allocato dinamicamente.
 * @param maxLength Il numero massimo di caratteri della riga da ricevere (escluso @c \\0).
 * @retval readCharCount Il numero di caratteri letti (escluso @c \\0). Se viene passato NULL, questo parametro viene ignorato.
 * @return Restituisce true se è stato trovato il carattere @c \\n, altrimenti restituisce false.
 */
bool myTcpReadLine(SOCKET sockfd, char *buffer, int maxLength, int *readCharCount);

/** @brief Riceve una riga terminata con il carattere @c \\n. La lettura è bufferizzata.
 * @param sockfd Il file descriptor del socket da cui leggere la riga.
 * @retval buffer Il buffer in cui scrivere la riga ricevuta (lunghezza @p maxLength + 1). Se è uguale a NULL, il buffer viene allocato dinamicamente.
 * @param maxLength Il numero massimo di caratteri della riga da ricevere (escluso @c \\0).
 * @return Restituisce il numero di caratteri letti (escluso @c \\0).
 * @warning Non mischiare questa funzione con le funzioni di lettura non bufferizzate.
 */
int myTcpBufferedReadLine(SOCKET sockfd, char *buffer, int maxLength);

/** @brief La funzione di callback chiamata dalla funzione @c myTcpReadChunks ogni volta che viene ricevuto un chunk di dati.
 * @param chunk Il buffer da cui leggere il chunk di dati ricevuto (dimensione @p chunkSize).
 * @param chunkSize Il numero di byte contenuti nel chunk di dati @p chunk. L'ultimo chunk di dati può contenere meno di @c DEFAULT_CHUNK_SIZE byte.
 * @param param L'eventuale parametro passato a questa funzione.
 * @return Se restituisce true, la funzione @c myTcpReadChunks legge il chunk successivo (a meno che non sia l'ultimo). Se restituisce false, la funzione @c myTcpReadChunks restituisce immediatamente false.
 */
typedef bool (*myTcpReadChunksCallback)(void *chunk, int chunkSize, void *param);

/** @brief Riceve fino a @p byteCount byte di dati, un chunk per volta.
 * @param sockfd Il file descriptor del socket da cui leggere i dati.
 * @param byteCount Il numero di byte di dati da ricevere.
 * @retval readByteCount Il numero di byte letti. Se viene passato NULL, questo parametro viene ignorato.
 * @param callback La funzione di callback da chiamare ogni volta che viene ricevuto un chunk di dati. Se viene passato NULL, questo parametro viene ignorato.
 * @param callbackParam L'eventuale parametro passato alla funzione di callback @p callback.
 * @return Restituisce true se tutti i dati sono stati letti, false se è stato raggiunto l'end-of-file (numero di byte letti minore di @p byteCount) oppure la funzione di callback ha restituito false.
 */
bool myTcpReadChunks(SOCKET sockfd, int byteCount, int *readByteCount, myTcpReadChunksCallback callback, void *callbackParam);

/** @brief Riceve fino a @p byteCount byte di dati, un chunk per volta, e li scrive in un file.
 * @param sockfd Il file descriptor del socket da cui leggere i dati.
 * @param filePath Il percorso del file in cui scrivere i dati letti.
 * @param fileSize Il numero di byte di dati da ricevere.
 * @retval readByteCount Il numero di byte letti. Se viene passato NULL, questo parametro viene ignorato.
 * @return Restituisce true se tutti i dati sono stati letti, false se è stato raggiunto l'end-of-file (numero di byte letti minore di @p byteCount) oppure non è possibile scrivere tutti i dati ricevuti nel file specificato.
 */
bool myTcpReadChunksAndWriteToFile(SOCKET sockfd, const char *filePath, int fileSize, int *readByteCount);

/** @brief La funzione di callback chiamata dalla funzione @c myTcpWriteChunks ogni volta che deve essere inviato un chunk di dati.
 * @param chunk Il buffer in cui scrivere il chunk di dati da inviare (dimensione @c DEFAULT_CHUNK_SIZE).
 * @retval chunkSize Il numero di byte contenuti nel chunk di dati @p chunk. L'ultimo chunk di dati può contenere meno di @c DEFAULT_CHUNK_SIZE byte.
 * @param param L'eventuale parametro passato a questa funzione.
 * @return Deve restituire false se il chunk corrente è l'ultimo, altrimenti deve restituire true.
 */
typedef bool (*myTcpWriteChunksCallback)(void *chunk, int *chunkSize, void *param);

/** @brief Invia dei dati, un chunk per volta.
 * @param sockfd Il file descriptor del socket su cui scrivere i dati.
 * @param callback La funzione di callback da chiamare ogni volta che deve essere inviato un chunk di dati.
 * @param callbackParam L'eventuale parametro passato alla funzione di callback @p callback.
 * @return Il numero di byte scritti.
 */
int myTcpWriteChunks(SOCKET sockfd, myTcpWriteChunksCallback callback, void *callbackParam);

/** @brief Legge i dati da un file e li invia un chunk per volta.
 * @param sockfd Il file descriptor del socket su cui scrivere i dati.
 * @param filePath Il percorso del file da cui leggere i dati da scrivere.
 * @return Restituisce il numero di byte scritti.
 */
int myTcpReadFromFileAndWriteChunks(SOCKET sockfd, const char *filePath);

#endif