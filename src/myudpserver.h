/** @file
 * @brief Contiene i wrapper delle funzioni per i server con socket non connessi UDP.
 */

#ifndef	__lunp_lg_udpserver_h
#define	__lunp_lg_udpserver_h

/** @brief La funzione che viene chiamata dalle funzioni @c myUdpServerXxx ogni volta che un client si connette al server.
 * @param sockfd Il file descriptor del socket a cui il client da servire è conesso.
 */
typedef void (*myUdpServerChildTask)(SOCKET sockfd);

/** @brief Crea un socket UDP associato alla porta specificata.
 * @param serverPort La porta del server a cui associare il socket creato.
 * @return Restituisce il file descriptor del socket UDP creato.
 */
SOCKET myUdpServerStartup(const char *serverPort);

/** @brief Implementa un server UDP che serve un client per volta.
 * @param sockfd Il file descriptor del socket a cui i client si connettono.
 * @param childTask La funzione da chiamare ogni volta che un client si connette al socket @p sockfd.
 * @warning Questa funzione non ritorna mai.
 */
void myUdpServerSimple(SOCKET sockfd, myUdpServerChildTask childTask);

/** @brief Implementa un server UDP che serve fino a @p childCount client tramite il pre-fork di @p childCount processi figlio.
 * @param sockfd Il file descriptor del socket a cui i client si connettono.
 * @param childCount Il numero massimo di client da servire allo stesso tempo.
 * @param childTask La funzione da chiamare ogni volta che un client si connette al socket @p sockfd.
 * @warning Questa funzione non ritorna mai.
 */
void myUdpServerPreforked(SOCKET sockfd, int childCount, myUdpServerChildTask childTask);

/** @brief Verifica che non siano stati ricevuti più di @p maxDatagrams datagrammi dal client @p clientStruct.
 * @param clientStruct La struttura contenente le informazioni sul client da cui i dati sono stati ricevuti.
 * @param maxDatagrams Il numero massimo di datagrammi che può essere ricevuto dal client @p clientStruct.
 * @param maxClients Il numero massimo di client da riconoscere.
 * @returns Restituisce false se sono stati ricevuti più di @p maxDatagrams datagrammi dal client @p clientStruct, altrimenti restituisce true.
 */
bool myUdpLimitClients(struct sockaddr_in clientStruct, int maxDatagrams, int maxClients);

#endif