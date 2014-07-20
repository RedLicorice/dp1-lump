/** @file
 * @brief Contiene i wrapper di funzioni varie.
 */

#ifndef	__lunp_lg_other_h
#define	__lunp_lg_other_h

/** @brief Chiude il socket specificato.
 * @param sockfd Il file descriptor del socket da chiudere.
 */
void myClose(SOCKET sockfd);

/** @brief Verifica l'esistenza di un file.
 * @param filePath Il percorso del file di cui verificare l'esistenza.
 * @return Restituisce true se il file esiste, altrimenti restituisce false.
 */
bool fileExists(const char *filePath);

/** @brief Recupera la dimensione di un file.
 * @param filePath Il percorso del file di cui recuperare la dimensione.
 * @return Restituisce la dimensione del file.
 */
uint32_t getFileSize(const char *filePath);

#endif