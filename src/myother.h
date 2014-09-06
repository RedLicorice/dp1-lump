/** @file
 * @brief Contiene i wrapper di funzioni varie.
 */

#ifndef	__myother_h
#define	__myother_h

/** @brief Attende finché il descrittore di file @p fileDescriptor non diventa pronto oppure l'intervallo di timeout non scade.
 * @param maxSeconds Il numero massimo di secondi per cui attendere che il descrittore diventi pronto. Se viene passato -1, l'intervallo di timeout è infinito e la funzione non ritorna finché il descrittore di file non diventa pronto.
 * @param fileDescriptor Il descrittore di file da attendere che diventi pronto.
 * @return Restituisce false se il timeout è scaduto prima che il descrittore diventasse pronto. Restituisce true se il descrittore è diventato pronto prima che il timeout scadesse.
 */
bool myWaitForSingleObject(int maxSeconds, int fileDescriptor);

/** @brief Attende finché uno dei @p fileDescriptorCount descrittori di file specificati non diventa pronto oppure l'intervallo di timeout non scade.
 * @param maxSeconds Il numero massimo di secondi per cui attendere che uno dei descrittori diventi pronto. Se viene passato -1, l'intervallo di timeout è infinito e la funzione non ritorna finché uno dei descrittori di file non diventa pronto.
 * @param fileDescriptorCount Il numero di descrittori di file specificati.
 * @param ... I descrittori di file da attendere che diventino pronti.
 * @return Restituisce 0 (false) se il timeout è scaduto prima che uno dei descrittori diventasse pronto. Restituisce 1 se il primo descrittore è diventato pronto prima che il timeout scadesse, e così via.
 */
int myWaitForMultipleObjects(int maxSeconds, int fileDescriptorCount, ...);

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