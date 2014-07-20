#!/bin/bash

# root = exam_2014-07-21/DP_July14_Test/socket/
zipDir="../" # exam_2014-07-21/DP_July14_Test/
typesDir="../tools/" # exam_2014-07-21/DP_July14_Test/tools/

if [ ! -f $typesDir"types.h" ] ; then
  rpcgen -h $typesDir"types.x" -o $typesDir"types.h"
fi
    
if [ ! -f $typesDir"types.c" ] ; then
  rpcgen -c $typesDir"types.x" -o $typesDir"types.c"
fi

case $1 in

  "compile")
  
    if [ ! -f "types.h" ] ; then
      cp $typesDir"types.h" "types.h"
    fi
	
    if [ ! -f "types.c" ] ; then
      cp $typesDir"types.c" "types.c"
    fi
      
    gcc -o socket_client client/*.c *.c -I client -lpthread -lm
    gcc -o socket_server server/*.c *.c -I server -lpthread -lm
    ;;
   
   
  "test")
    
    if [ -f "types.h" ] ; then
      rm "types.h"
    fi
    
    if [ -f "types.c" ] ; then
      rm "types.c"
    fi
      
    if [ -f $zipDir"socket.zip" ] ; then
      rm $zipDir"socket.zip"
    fi
    zip $zipDir"socket.zip" client/*.c client/*.h server/*.c server/*.h *.c *.h
    
    cd $zipDir
    bash "./test.sh"
    ;;
    
esac