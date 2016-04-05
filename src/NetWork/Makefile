ROOT_DIR=../../..

include $(ROOT_DIR)/Makefile.include

MODULES_DIR=libevent-1.4.14b-stable protobuf-2.4.1 snappy-1.0.5 net

all:
	@for i in $(MODULES_DIR); do \
		cd $$i &&make && cd ..;\
	done;

clean:
	@for i in $(MODULES_DIR); do\
		cd $$i && make clean && cd ..;\
	done;
