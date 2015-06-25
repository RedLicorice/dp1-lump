#At home#

1. Generate the `config.h` file:

        make config
    
    or at Labinf:

        cp "config/config labinf.h" src/


2. Check that the library works:

        make compile

3. Put the client into: `test/client/client.c`.

4. Put the server into: `test/server/server.c`.

5. Compile the server:

        make compile-server

6. Compile the client:

        make compile-client

7. Compile and launch the server:

        make server <arguments>

8. Compile and launch the client:

        make client <arguments>

***

#At exam time#

You only need folder `src`.

1. Make sure you have the `config.h` file for Labinf computers:

        cp "config/config labinf.h" src/

2. If the exam is not about XDR, you have to use the `script-noxdr.sh` script:

        mv script-noxdr.sh script.sh

    If the exam is about XDR, you have to use the `script-xdr.sh` script:

        mv script-xdr.sh script.sh

3. Inside the script, set the `twoClients` variable to:
    * `false` if just a client, in folder `client`, is required;
    * `true` if two clients, in folders `client1` and `client2`, are required.


4. Inside the script, set the `twoServers` variable to:
    * `false` if just a server, in folder `server`, is required;
    * `true` if two servers, in folders `server1` and `server2`, are required.

5. Compile the client and the server, and launch the server:

        ./script.sh compile <server_arguments>

6. If two clients and/or two servers are required, compile the (second) client and the (second) server, and launch the (second) server:

        ./script.sh compile2 <server_arguments>

7. Create the zip file, and launch the tests:

        ./script.sh test

***

#Help#

* To generate doxygen documentation:

        make doxygen

    or you can read it in [my website](http://lucaghio.webege.com/files/school/programming/lunp/index.html).

* To generate cscope files:

        make cscope