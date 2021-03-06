#ifndef	__mylunp_h
#define	__mylunp_h

#undef HAVE_INET6_RTH_INIT // fix for Ubuntu 14.04

#include <rpc/xdr.h>

typedef int SOCKET;

enum bool { false = 0, true = 1 };
typedef enum bool bool;

#define DEFAULT_CHUNK_SIZE 256

#include "myerror.h"
#include "myother.h"
#include "mytcp.h"
#include "mytcpserver.h"
#include "myudp.h"
#include "myudpserver.h"
#include "myxdr.h"

#include "common.h"

#endif
