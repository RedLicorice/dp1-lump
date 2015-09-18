/*
 * myFunctions.h
 *
 *  Created on: 08/ago/2015
 *      Author: francesco
 */

#ifndef MY_MYTCP_H_
#define MY_MYTCP_H_

/** @brief Legge i dati da un file e li invia un chunk per volta.
 * @param sockfd Il file descriptor del socket su cui scrivere i dati.
 * @param filePath Il percorso del file da cui leggere i dati da scrivere.
 * @retval byteLetti Il numero di byte letti. Se viene passato NULL, questo parametro viene ignorato.
 * @return Restituisce false se l'endpoint remoto ha chiuso la connessione, altrimenti restituisce true.
 */
bool myMyTcpReadFromFileAndWriteChunks(SOCKET sockfd, char* fileName, int *byteLetti);

/** @brief Riceve fino a @p byteCount byte di dati, un chunk per volta, e li scrive in un file.
 * @param sockfd Il file descriptor del socket da cui leggere i dati.
 * @param filePath Il percorso del file in cui scrivere i dati letti.
 * @retval byteLetti Il numero di byte letti. Se viene passato NULL, questo parametro viene ignorato.
 * @return Questa funzione non ritorna
 */
void myMyTcpReadChunksAndWriteToFile(SOCKET sockfd, char* fileName, int *byteLetti);


#endif
