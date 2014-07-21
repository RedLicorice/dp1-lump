CC=gcc
CFLAGS=-Wall -g

.PHONY: cscope doxygen # http://stackoverflow.com/questions/3931741/why-does-make-think-the-target-is-up-to-date

compile: copylunp
	$(CC) $(CFLAGS) -o /tmp/lunp-compilation test/client/client-empty.c test/*.c -I client -lpthread -lm
	rm -f /tmp/lunp-compilation
	
client: copylunp
	$(CC) $(CFLAGS) -o test/socket_client test/client/client.c test/*.c -I client -lpthread -lm
	./test/socket_client $(filter-out $@,$(MAKECMDGOALS)) # http://stackoverflow.com/a/6273809
	
server: copylunp
	$(CC) $(CFLAGS) -o test/socket_server test/server/server.c test/*.c -I server -lpthread -lm
	./test/socket_server $(filter-out $@,$(MAKECMDGOALS)) # http://stackoverflow.com/a/6273809
	
%:      # thanks to chakrit
	@:    # thanks to William Pursell
	
copylunp: clean
	cp src/*.c test
	find src/ -mindepth 1 -maxdepth 1 -name *.h -and ! -name common.h -exec cp \{} test/ \;
	cp -n src/common.h test/common.h

clean:
	rm -f test/*.c
	find test/ -mindepth 1 -maxdepth 1 -name *.h -and ! -name common.h -exec rm \{} \;

doxygen:
	doxygen Doxyfile
	#firefox doxygen/index.html

cscope:
	# il percorso del direttorio di lavoro di cscope non può contenere spazi
	rm -rf /tmp/lunp
	mkdir -p /tmp/lunp
	ln -s "$$PWD/src" /tmp/lunp/src
	mkdir -p /tmp/lunp/cscope
	cd /tmp/lunp/cscope ; find ../src/ -name '*.c' > cscope.files
	cd /tmp/lunp/cscope ; find ../src/ -name '*.h' >> cscope.files
	cd /tmp/lunp/cscope ; cscope -b -q -k
	rm -rf cscope
	mkdir cscope
	mv /tmp/lunp/cscope/cscope.in.out cscope/cscope.in.out
	mv /tmp/lunp/cscope/cscope.out cscope/cscope.out
	mv /tmp/lunp/cscope/cscope.po.out cscope/cscope.po.out
	rm /tmp/lunp/cscope/cscope.files
	rm -d /tmp/lunp/cscope
	rm /tmp/lunp/src
	rm -d /tmp/lunp
	#cd cscope
	#cscope -d