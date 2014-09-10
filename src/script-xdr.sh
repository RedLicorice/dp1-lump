#!/bin/bash

# root = exam_dp_monthyyyy/source/
zipDir="../" # exam_dp_monthyyyy/
typesDir="../tools/" # exam_dp_monthyyyy/tools/

clear


echo -e "\n*******************Generating rcpgen files...*******************"
if [ ! -f $typesDir"types.h" ] ; then
  rpcgen -h $typesDir"types.x" -o $typesDir"types.h"
fi
    
if [ ! -f $typesDir"types.c" ] ; then
  rpcgen -c $typesDir"types.x" -o $typesDir"types.c"
fi


case $1 in

  "compile") # http://stackoverflow.com/a/1024532
  
    if [ ! -f "types.h" ] ; then
      cp $typesDir"types.h" "types.h"
    fi
	
    if [ ! -f "types.c" ] ; then
      cp $typesDir"types.c" "types.c"
    fi
      
    echo -e "\n*******************Compiling client...*******************"
    if gcc -Wall -Wno-deprecated-declarations -o socket_client client/*.c *.c -Iclient -lpthread -lm ; then
      echo -e "\n*******************Compiling server...*******************"
      if gcc -Wall -Wno-deprecated-declarations -o socket_server server/*.c *.c -Iserver -lpthread -lm ; then
	echo -e "\n*******************Launching server...*******************"
	shift # http://lglinux.blogspot.it/2008/10/removing-bash-arguments.html
	./socket_server "$@"
      fi
    fi
    ;;

    
  "compile1") # http://stackoverflow.com/a/1024532
  
    if [ ! -f "types.h" ] ; then
      cp $typesDir"types.h" "types.h"
    fi
	
    if [ ! -f "types.c" ] ; then
      cp $typesDir"types.c" "types.c"
    fi
    
    echo -e "\n*******************Compiling client1...*******************"
    if gcc -Wall -Wno-deprecated-declarations -o socket_client1 client1/*.c *.c -Iclient1 -lpthread -lm ; then
      echo -e "\n*******************Compiling server1...*******************"
      if gcc -Wall -Wno-deprecated-declarations -o socket_server1 server1/*.c *.c -Iserver1 -lpthread -lm ; then
	echo -e "\n*******************Launching server1...*******************"
	shift # http://lglinux.blogspot.it/2008/10/removing-bash-arguments.html
	./socket_server1 "$@"
      fi
    fi
    ;;

    
  "compile2") # http://stackoverflow.com/a/1024532
  
    if [ ! -f "types.h" ] ; then
      cp $typesDir"types.h" "types.h"
    fi
	
    if [ ! -f "types.c" ] ; then
      cp $typesDir"types.c" "types.c"
    fi
    
    echo -e "\n*******************Compiling client2...*******************"
    if gcc -Wall -Wno-deprecated-declarations -o socket_client2 client2/*.c *.c -Iclient2 -lpthread -lm ; then
      echo -e "\n*******************Compiling server2...*******************"
      if gcc -Wall -Wno-deprecated-declarations -o socket_server2 server2/*.c *.c -Iserver2 -lpthread -lm ; then
	echo -e "\n*******************Launching server2...*******************"
	shift # http://lglinux.blogspot.it/2008/10/removing-bash-arguments.html
	./socket_server2 "$@"
      fi
    fi
    ;;
   
   
  "test")
    
    if [ -f "types.h" ] ; then
      rm "types.h"
    fi
    
    if [ -f "types.c" ] ; then
      rm "types.c"
    fi
      
    echo -e "\n*******************Zipping...*******************"
    if [ -f $zipDir"socket.zip" ] ; then
      rm $zipDir"socket.zip"
    fi
    zip $zipDir"socket.zip" client/*.c client/*.h server/*.c server/*.h *.c *.h
    
    cd $zipDir
    echo -e "\n*******************Launching tests...*******************"
    bash "./test.sh"
    ;;
   
   
  "test12")
    
    if [ -f "types.h" ] ; then
      rm "types.h"
    fi
    
    if [ -f "types.c" ] ; then
      rm "types.c"
    fi
      
    echo -e "\n*******************Zipping...*******************"
    if [ -f $zipDir"socket.zip" ] ; then
      rm $zipDir"socket.zip"
    fi
    zip $zipDir"socket.zip" client1/*.c client1/*.h server1/*.c server1/*.h client2/*.c client2/*.h server2/*.c server2/*.h *.c *.h
    
    cd $zipDir
    echo -e "\n*******************Launching tests12...*******************"
    bash "./test.sh"
    ;;
    
    
  *)
  
    echo $"Invalid command"
    exit 1
    
esac