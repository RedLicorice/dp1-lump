CC=gcc
CFLAGS=-Wall -g -Wno-deprecated-declarations

.PHONY: config cscope doxygen # http://stackoverflow.com/questions/3931741/why-does-make-think-the-target-is-up-to-date

config:
	cd config ; ./configure
	mv config/config.h src/config.h
	rm config/config.log
	rm config/config.status
	rm config/Make.defines
	rm config/Makefile

compile: copylunp
	@echo "*******************Compiling client-empty...*******************"
	@rm -rf /tmp/lunp
	@mkdir -p /tmp/lunp
	@$(CC) $(CFLAGS) -o /tmp/lunp/compilation test/client/client-empty.c test/*.c -Iclient -lpthread -lm
	@rm -rf /tmp/lunp
	
client: compile-client
	@echo "\n\n*******************Launching client...*******************"
	@cd test ; ./socket_client $(filter-out $@,$(MAKECMDGOALS)) # http://stackoverflow.com/a/6273809
	
server: compile-server
	@echo "\n\n*******************Launching server...*******************"
	@cd test ; ./socket_server $(filter-out $@,$(MAKECMDGOALS)) # http://stackoverflow.com/a/6273809
	
%:      # thanks to chakrit
	@:    # thanks to William Pursell
	
compile-client: copylunp
	@echo "*******************Compiling client...*******************"
	@$(CC) $(CFLAGS) -o test/socket_client test/client/client.c test/*.c -Iclient -lpthread -lm
	
compile-server: copylunp
	@echo "*******************Compiling server...*******************"
	@$(CC) $(CFLAGS) -o test/socket_server test/server/server.c test/*.c -Iserver -lpthread -lm
	
copylunp: clean
	@cp src/*.c test
	@find src/ -mindepth 1 -maxdepth 1 -name *.h -and ! -name common.h -exec cp \{} test/ \;
	@cp -n src/common.h test/common.h

clean:
	@clear
	@find test/ -mindepth 1 -maxdepth 1 -name *.c -and ! -name types.c -exec rm -f \{} \;
	@find test/ -mindepth 1 -maxdepth 1 -name *.h -and ! -name common.h -and ! -name types.h -exec rm -f \{} \;

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