#!/bin/bash

twoClients= # set to true or false
twoServers= # set to true or false

# root = exam_dp_monthyyyy/source/
zipDir="../" # exam_dp_monthyyyy/
typesDir="../tools/" # exam_dp_monthyyyy/tools/
srcZipPath=${PWD##*/}"/" # exam_dp_monthyyyy/source/ ( http://stackoverflow.com/a/1371283/1267803 )

clear


case $1 in

  "compile")
    
    if [ "$twoClients" = true ] ; then
      clientName="client1"
    else
      clientName="client"
    fi
    
    if [ "$twoServers" = true ] ; then
      serverName="server1"
    else
      serverName="server"
    fi
    
    echo -e "*******************Compiling $clientName...*******************"
    if gcc -Wall -Wno-deprecated-declarations -o socket_$clientName $clientName/*.c *.c -I$clientName -lpthread -lm ; then # http://stackoverflow.com/a/1024532
      echo -e "\n\n*******************Compiling $serverName...*******************"
      if gcc -Wall -Wno-deprecated-declarations -o socket_$serverName $serverName/*.c *.c -I$serverName -lpthread -lm ; then
	echo -e "\n\n*******************Launching $serverName...*******************"
	shift # http://lglinux.blogspot.it/2008/10/removing-bash-arguments.html
	./socket_$serverName "$@"
      fi
    fi
    ;;

    
  "compile2")
    
    if [ "$twoClients" = true ] ; then
      clientName="client2"
    else
      clientName="client"
    fi
    
    if [ "$twoServers" = true ] ; then
      serverName="server2"
    else
      serverName="server"
    fi
    
    echo -e "*******************Compiling $clientName...*******************"
    if gcc -Wall -Wno-deprecated-declarations -o socket_$clientName $clientName/*.c *.c -I$clientName -lpthread -lm ; then
      echo -e "\n\n*******************Compiling $serverName...*******************"
      if gcc -Wall -Wno-deprecated-declarations -o socket_$serverName $serverName/*.c *.c -I$serverName -lpthread -lm ; then
	echo -e "\n\n*******************Launching $serverName...*******************"
	shift
	./socket_$serverName "$@"
      fi
    fi
    ;;
   
   
  "test")
    
    echo -e "*******************Zipping...*******************"
    cd $zipDir
    if [ -f "socket.zip" ] ; then
      rm "socket.zip"
    fi
    
    if [ "$twoServers" = false ] ; then
      if [ "$twoClients" = false ] ; then
        zip "socket.zip" "$srcZipPath"client/*.c "$srcZipPath"client/*.h "$srcZipPath"server/*.c "$srcZipPath"server/*.h "$srcZipPath"*.c "$srcZipPath"*.h
      else # twoClients = true
        zip "socket.zip" "$srcZipPath"client1/*.c "$srcZipPath"client1/*.h "$srcZipPath"client2/*.c "$srcZipPath"client2/*.h "$srcZipPath"server/*.c "$srcZipPath"server/*.h "$srcZipPath"*.c "$srcZipPath"*.h
      fi
      
    else # twoServers = true
      if [ "$twoClients" = false ] ; then
        zip "socket.zip" "$srcZipPath"client/*.c "$srcZipPath"client/*.h "$srcZipPath"server1/*.c "$srcZipPath"server1/*.h "$srcZipPath"server2/*.c "$srcZipPath"server2/*.h "$srcZipPath"*.c "$srcZipPath"*.h
      else # twoClients = true
        zip "socket.zip" "$srcZipPath"client1/*.c "$srcZipPath"client1/*.h "$srcZipPath"client2/*.c "$srcZipPath"client2/*.h "$srcZipPath"server1/*.c "$srcZipPath"server1/*.h "$srcZipPath"server2/*.c "$srcZipPath"server2/*.h "$srcZipPath"*.c "$srcZipPath"*.h
      fi
    fi
    
    echo -e "\n\n*******************Launching tests...*******************"
    bash "./test.sh"
    ;;
    
    
  *)
  
    echo $"Invalid command"
    exit 1
    
esac