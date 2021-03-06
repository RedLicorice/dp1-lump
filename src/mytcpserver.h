/** @file
 * @brief Contiene i wrapper delle funzioni per i server con socket connessi TCP.
 */

#ifndef	__mytcpserver_h
#define	__mytcpserver_h

/** @brief La funzione che viene chiamata dalle funzioni @c myTcpServerXxx ogni volta che un client si connette al server.
 * @param sockfd Il file descriptor del socket a cui il client da servire è conesso.
 */
typedef void (*myTcpServerChildTask)(SOCKET sockfd);

/** @brief La funzione che viene chiamata dalla funzione @c myTcpServerSelect ogni volta che un client si connette al server.
 * @param sockfd Il file descriptor del socket a cui il client da servire è conesso.
 * @return Deve restituire false se si vuole chiudere la connessione, true se si vuole mantenere attiva la connessione.
 */
typedef bool (*myTcpServerSelectChildTask)(SOCKET sockfd);

/** @brief Accetta una connessione sul socket TCP specificato.
 * @param sockfd Il file descriptor del socket TCP su cui accettare la connessione.
 * @retval clientStruct La struttura contenente le informazioni sul client.
 * @return Restituisce il file descriptor del socket TCP accettato.
 */
SOCKET myTcpServerAccept(SOCKET sockfd, struct sockaddr_in *clientStruct);

/** @brief Crea un socket TCP associato alla porta specificata.
 * @param serverPort La porta del server a cui associare il socket creato.
 * @return Restituisce il file descriptor del socket TCP creato.
 */
SOCKET myTcpServerStartup(const char *serverPort);

/** @brief Implementa un server TCP che serve un client per volta.
 * @param sockfd Il file descriptor del socket a cui i client si connettono.
 * @param childTask La funzione da chiamare ogni volta che un client si connette al socket @p sockfd.
 * @warning Questa funzione non ritorna mai.
 */
void myTcpServerSimple(SOCKET sockfd, myTcpServerChildTask childTask);

/** @brief Implementa un server TCP che serve più client tramite il fork di più processi figlio, uno per client.
 * @param sockfd Il file descriptor del socket a cui i client si connettono.
 * @param childTask La funzione da chiamare ogni volta che un client si connette al socket @p sockfd.
 * @warning Questa funzione non ritorna mai (nel processo padre).
 */
void myTcpServerOCPC(SOCKET sockfd, myTcpServerChildTask childTask);

/** @brief Implementa un server TCP che serve fino a @p maxChildCount client tramite il fork di più processi figlio, uno per client.
 * @param sockfd Il file descriptor del socket a cui i client si connettono.
 * @param maxChildCount Il numero massimo di client da servire allo stesso tempo.
 * @param childTask La funzione da chiamare ogni volta che un client si connette al socket @p sockfd.
 * @warning Questa funzione non ritorna mai (nel processo padre).
 */
void myTcpServerOCPCMax(SOCKET sockfd, int maxChildCount, myTcpServerChildTask childTask);

/** @brief Implementa un server TCP che serve fino a @p childCount client tramite il pre-fork di @p childCount processi figlio.
 * @param sockfd Il file descriptor del socket a cui i client si connettono.
 * @param childCount Il numero massimo di client da servire allo stesso tempo.
 * @param childTask La funzione da chiamare ogni volta che un client si connette al socket @p sockfd.
 * @warning Questa funzione non ritorna mai.
 */
void myTcpServerPreforked(SOCKET sockfd, int childCount, myTcpServerChildTask childTask);

/** @brief Implementa un server TCP che serve più client tramite il pre-fork di @p minChildCount processi figlio.
 * @param sockfd Il file descriptor del socket a cui i client si connettono.
 * @param minChildCount Il numero minimo di processi figlio generati con il pre-fork.
 * @param childTask La funzione da chiamare ogni volta che un client si connette al socket @p sockfd.
 * @warning Questa funzione non ritorna mai.
 */
void myTcpServerMixed(SOCKET sockfd, int minChildCount, myTcpServerChildTask childTask);

/** @brief Implementa un server TCP che serve fino a @p maxChildCount client tramite il pre-fork di @p minChildCount processi figlio.
 * @param sockfd Il file descriptor del socket a cui i client si connettono.
 * @param minChildCount Il numero minimo di processi figlio generati con il pre-fork.
 * @param maxChildCount Il numero massimo di client da servire allo stesso tempo.
 * @param childTask La funzione da chiamare ogni volta che un client si connette al socket @p sockfd.
 * @warning Questa funzione non ritorna mai.
 */
void myTcpServerMixedMax(SOCKET sockfd, int minChildCount, int maxChildCount, myTcpServerChildTask childTask);

/** @brief Implementa un server TCP che serve fino a @p maxChildCount client nello stesso processo.
 * @param sockfd Il file descriptor del socket a cui i client si connettono.
 * @param maxChildCount Il numero massimo di client da servire allo stesso tempo.
 * @param childTask La funzione da chiamare ogni volta che un client si connette al socket @p sockfd.
 * @warning Questa funzione non ritorna mai.
 */
void myTcpServerSelect(SOCKET sockfd, int maxChildCount, myTcpServerSelectChildTask childTask);

#endif