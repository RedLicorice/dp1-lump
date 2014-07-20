/** @file
 * @brief Contiene i wrapper delle funzioni per la stampa degli errori.
 */

#ifndef	__lunp_lg_error_h
#define	__lunp_lg_error_h

/** @brief Stampa un messaggio di errore personalizzato, poi termina immediatamente il programma.
 * @param errorMsg Il messaggio di errore da stampare. Se viene passato NULL, questo parametro viene ignorato.
 * @param bracketText L'eventuale testo da stampare all'inizio, racchiuso in parentesi quadre. Se viene passato NULL, questo parametro viene ignorato.
 * @warning Questa funzione non ritorna mai.
 */
void myError(const char *errorMsg, const char *bracketText);

/** @brief Stampa un messaggio di errore relativo all'errore generato da una funzione, poi termina immediatamente il programma.
 * @param functionName Il nome della funzione.
 * @param errorMsg Il messaggio di errore da stampare. Se viene passato NULL, questo parametro viene ignorato.
 * @param bracketText L'eventuale testo da stampare all'inizio, racchiuso in parentesi quadre. Se viene passato NULL, questo parametro viene ignorato.
 * @warning Questa funzione non ritorna mai.
 */
void myFunctionError(const char *functionName, const char *errorMsg, const char *bracketText);

/** @brief Stampa un messaggio di errore relativo all'ultimo errore generato dall'ultima funzione di sistema chiamata, poi termina immediatamente il programma.
 * @param systemCallName Il nome dell'ultima funzione di sistema chiamata.
 * @param bracketText L'eventuale testo da stampare all'inizio, racchiuso in parentesi quadre. Se viene passato NULL, questo parametro viene ignorato.
 * @warning Questa funzione non ritorna mai.
 * @warning Non effettuare altre chiamate a funzioni di sistema tra la chiamata alla funzione di sistema che ha generato l'errore e la chiamata a questa funzione.
 */
void mySystemError(const char *systemCallName, const char *bracketText);

/** @brief Stampa un messaggio di avvertimento personalizzato. Non termina il programma.
 * @param warningMsg Il messaggio di avvertimento da stampare. Se viene passato NULL, questo parametro viene ignorato.
 * @param bracketText L'eventuale testo da stampare all'inizio, racchiuso in parentesi quadre. Se viene passato NULL, questo parametro viene ignorato.
 * @return Restituisce sempre false.
 */
bool myWarning(const char *warningMsg, const char *bracketText);

/** @brief Stampa un messaggio di avvertimento relativo all'errore generato da una funzione. Non termina il programma.
 * @param functionName Il nome della funzione.
 * @param warningMsg Il messaggio di avvertimento da stampare. Se viene passato NULL, questo parametro viene ignorato.
 * @param bracketText L'eventuale testo da stampare all'inizio, racchiuso in parentesi quadre. Se viene passato NULL, questo parametro viene ignorato.
 * @return Restituisce sempre false.
 */
bool myFunctionWarning(const char *functionName, const char *warningMsg, const char *bracketText);

/** @brief Stampa un messaggio di avvertimento relativo all'ultimo errore generato dall'ultima funzione di sistema chiamata. Non termina il programma.
 * @param systemCallName Il nome dell'ultima funzione di sistema chiamata.
 * @param bracketText L'eventuale testo da stampare all'inizio, racchiuso in parentesi quadre. Se viene passato NULL, questo parametro viene ignorato.
 * @return Restituisce sempre false.
 * @warning Non effettuare altre chiamate a funzioni di sistema tra la chiamata alla funzione di sistema che ha generato l'errore e la chiamata a questa funzione.
 */
bool mySystemWarning(const char *systemCallName, const char *bracketText);

#endif