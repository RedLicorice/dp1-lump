#!/bin/bash

# root = exam_dp_monthyyyy/source/
zipDir="../" # exam_dp_monthyyyy/
typesDir="../tools/" # exam_dp_monthyyyy/tools/

clear


case $1 in

  "compile") # http://stackoverflow.com/a/1024532
    
    echo -e "*******************Compiling client...*******************"
    if gcc -Wall -Wno-deprecated-declarations -o socket_client client/*.c *.c -I client -lpthread -lm -lrt ; then
      echo -e "\n\n*******************Compiling server...*******************"
      if gcc -Wall -Wno-deprecated-declarations -o socket_server server/*.c *.c -I server -lpthread -lm -lrt ; then
	echo -e "\n\n*******************Launching server...*******************"
	shift # http://lglinux.blogspot.it/2008/10/removing-bash-arguments.html
	./socket_server "$@"
      fi
    fi
    ;;

    
  "compile1") # http://stackoverflow.com/a/1024532
    
    echo -e "*******************Compiling client1...*******************"
    if gcc -Wall -Wno-deprecated-declarations -o socket_client1 client1/*.c *.c -I client1 -lpthread -lm -lrt ; then
      echo -e "\n\n*******************Compiling server1...*******************"
      if gcc -Wall -Wno-deprecated-declarations -o socket_server1 server1/*.c *.c -I server1 -lpthread -lm -lrt ; then
	echo -e "\n\n*******************Launching server1...*******************"
	shift # http://lglinux.blogspot.it/2008/10/removing-bash-arguments.html
	./socket_server1 "$@"
      fi
    fi
    ;;

    
  "compile2") # http://stackoverflow.com/a/1024532
    
    echo -e "*******************Compiling client2...*******************"
    if gcc -Wall -Wno-deprecated-declarations -o socket_client2 client2/*.c *.c -I client -lpthread -lm -lrt ; then
      echo -e "\n\n*******************Compiling server2...*******************"
      if gcc -Wall -Wno-deprecated-declarations -o socket_server2 server2/*.c *.c -I server -lpthread -lm -lrt ; then
	echo -e "\n\n*******************Launching server2...*******************"
	shift # http://lglinux.blogspot.it/2008/10/removing-bash-arguments.html
	./socket_server2 "$@"
      fi
    fi
    ;;
   
   
  "test")
    
    echo -e "*******************Zipping...*******************"
    if [ -f $zipDir"socket.zip" ] ; then
      rm $zipDir"socket.zip"
    fi
    zip $zipDir"socket.zip" client/*.c client/*.h server/*.c server/*.h *.c *.h
    
    cd $zipDir
    echo -e "\n\n*******************Launching tests...*******************"
    bash "./test.sh"
    ;;
   
   
  "test12")
      
    echo -e "*******************Zipping...*******************"
    if [ -f $zipDir"socket.zip" ] ; then
      rm $zipDir"socket.zip"
    fi
    zip $zipDir"socket.zip" client1/*.c client1/*.h server1/*.c server1/*.h client2/*.c client2/*.h server2/*.c server2/*.h *.c *.h
    
    cd $zipDir
    echo -e "\n\n*******************Launching tests12...*******************"
    bash "./test.sh"
    ;;
    
    
  *)
  
    echo $"Invalid command"
    exit 1
    
esac