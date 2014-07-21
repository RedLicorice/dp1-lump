#!/bin/bash

# root = exam_2014-07-21/DP_July14_Test/socket/
zipDir="../" # exam_2014-07-21/DP_July14_Test/
typesDir="../tools/" # exam_2014-07-21/DP_July14_Test/tools/

case $1 in

  "compile") # http://stackoverflow.com/a/1024532
    
    if gcc -Wall -o socket_client client/*.c *.c -I client -lpthread -lm ; then
      if gcc -Wall -o socket_server server/*.c *.c -I server -lpthread -lm ; then
	shift # http://lglinux.blogspot.it/2008/10/removing-bash-arguments.html
	./socket_server "$@"
      fi
    fi
    ;;

    
  "compile1") # http://stackoverflow.com/a/1024532
    
    if gcc -Wall -o socket_client1 client1/*.c *.c -I client1 -lpthread -lm ; then
      if gcc -Wall -o socket_server1 server1/*.c *.c -I server1 -lpthread -lm ; then
	shift # http://lglinux.blogspot.it/2008/10/removing-bash-arguments.html
	./socket_server1 "$@"
      fi
    fi
    ;;

    
  "compile2") # http://stackoverflow.com/a/1024532
    
    if gcc -Wall -o socket_client client/*.c *.c -I client -lpthread -lm ; then
      if gcc -Wall -o socket_server server/*.c *.c -I server -lpthread -lm ; then
	shift # http://lglinux.blogspot.it/2008/10/removing-bash-arguments.html
	./socket_server "$@"
      fi
    fi
    ;;
   
   
  "test")
      
    if [ -f $zipDir"socket.zip" ] ; then
      rm $zipDir"socket.zip"
    fi
    zip $zipDir"socket.zip" client/*.c client/*.h server/*.c server/*.h *.c *.h
    
    cd $zipDir
    bash "./test.sh"
    ;;
   
   
  "test12")
      
    if [ -f $zipDir"socket.zip" ] ; then
      rm $zipDir"socket.zip"
    fi
    zip $zipDir"socket.zip" client1/*.c client1/*.h server1/*.c server1/*.h client2/*.c client2/*.h server2/*.c server2/*.h *.c *.h
    
    cd $zipDir
    bash "./test.sh"
    ;;
    
esac