/** @file
  * @brief Contiene eventuali risorse da condividere al volo tra il client e il server.
  */

#ifndef	__common_h
#define	__common_h

typedef
struct Request_t {
	uint32_t id, op1, op2;
} Request;

typedef
struct Response_t {
	uint32_t id, res;
} Response;

typedef
enum exit_code_t {
	success = 0,
	wrong_response = 1,
	expired_timeout = 2
} exit_code;

#endif
