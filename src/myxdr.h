/** @file
 * @brief Contiene i wrapper delle funzioni per il protocollo XDR.
 */

#ifndef	__myxdr_h
#define	__myxdr_h

/** @brief Una generica funzione XDR.
 * @param xdrs Il flusso XDR.
 * @retval data I dati.
 * @return Restituisce TRUE se l'operazione è stata completata con successo, altrimenti restituisce FALSE.
 */
typedef bool_t (*myXdrFunction)(XDR *xdrs, void *data);

/** @brief Riceve dei dati. Utilizza la funzione xdrstdio_create().
 * @param sockfd Il file descriptor del socket da cui leggere i dati.
 * @param xdrFunction La funzione XDR per la ricezione dei dati.
 * @param data I dati ricevuti.
 * @return Restituisce true se l'operazione di ricezione è stata completata con successo, altrimenti restituisce false.
 * @warning Questa funzione funziona anche con il protocollo UDP.
 */
bool myTcpReadXdr(SOCKET sockfd, myXdrFunction xdrFunction, void *data);

/** @brief Invia i dati @p data. Utilizza la funzione xdrstdio_create().
 * @param sockfd Il file descriptor del socket su cui scrivere i dati.
 * @param xdrFunction La funzione XDR per l'invio dei dati.
 * @param data I dati da inviare.
 * @return Restituisce true se l'operazione di invio è stata completata con successo, altrimenti restituisce false.
 * @warning Questa funzione funziona anche con il protocollo UDP purché il socket sia connesso.
 */
bool myTcpWriteXdr(SOCKET sockfd, myXdrFunction xdrFunction, void *data);

/** @brief Crea un flusso XDR per la decodifica dei dati ricevuti.
 * @param buffer Il buffer da cui leggere i dati ricevuti da decodificare.
 * @param bufferPos La posizione finale nel buffer, ovvero il numero di byte di dati ricevuti.
 * @return Restituisce il flusso XDR creato.
 */
XDR *myUdpReadXdrStartup(char *buffer, int bufferPos);

/** @brief Decodifica i dati ricevuti. Utilizza la funzione xdrmem_create().
 * @param xdrs Il flusso XDR per la decodifica dei dati ricevuti.
 * @param xdrFunction La funzione XDR per la decodifica dei dati ricevuti.
 * @retval data I dati decodificati.
 * @return Restituisce true se l'operazione di decodifica è stata completata con successo, altrimenti restituisce false.
 * @warning Prima di questa funzione è necessario chiamare la funzione myUdpReadXdrStartup(). Dopo questa funzione è necessario chiamare myUdpReadXdrCleanup().
 * @warning Questa funzione funziona anche con il protocollo TCP purché la chiamata alla funzione read() sia singola.
 */
bool myUdpReadXdr(XDR *xdrs, myXdrFunction xdrFunction, void *data);

/** @brief Effettua la pulizia al termine della decodifica dei dati ricevuti.
 * @param xdrs Il flusso XDR per la decodifica dei dati ricevuti.
 */
void myUdpReadXdrCleanup(XDR *xdrs);

/** @brief Crea un flusso XDR per la codifica dei dati ricevuti.
 * @retval buffer Il buffer in cui scrivere i dati codificati da inviare.
 * @param bufferSize La dimensione del buffer.
 * @return Restituisce il flusso XDR creato.
 */
XDR *myUdpWriteXdrStartup(char *buffer, int bufferSize);

/** @brief Codifica i dati da inviare. Utilizza la funzione xdrmem_create().
 * @param xdrs Il flusso XDR per la codifica dei dati da inviare.
 * @param xdrFunction La funzione XDR per la codifica dei dati da inviare.
 * @param data I dati da codificare.
 * @return Restituisce true se l'operazione di codifica è stata completata con successo, altrimenti restituisce false.
 * @warning Prima di questa funzione è necessario chiamare la funzione myUdpWriteXdrStartup(). Dopo questa funzione è necessario chiamare myUdpWriteXdrCleanup().
 * @warning Questa funzione funziona anche con il protocollo TCP.
 */
bool myUdpWriteXdr(XDR *xdrs, myXdrFunction xdrFunction, void *data);

/** @brief Effettua la pulizia al termine della codifica dei dati da inviare.
 * @param xdrs Il flusso XDR per la codifica dei dati da inviare.
 * @param success Il risultato delle operazioni di codifica dei dati.
 * @return Restituisce la posizione finale nel buffer, ovvero il numero di byte di dati da inviare. Se @p success è uguale a false, restituisce -1.
 */
int myUdpWriteXdrCleanup(XDR *xdrs, bool success);

#endif