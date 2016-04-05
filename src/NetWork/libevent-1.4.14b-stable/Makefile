ROOT_DIR=../../../..

include $(ROOT_DIR)/Makefile.include

FILES=$(wildcard *.c)

ifeq ($(BUILD_SYSTEM),-D__linux__)
	Filters=devpoll.c epoll_sub.c evport.c evrpc.c kqueue.c
	INCLUDE=-I$(ROOT_DIR)/engine/src/Network/libevent-1.4.14b-stable/Centos62
else
	Filters=devpoll.c epoll.c epoll_sub.c evport.c evrpc.c
	INCLUDE=-I$(ROOT_DIR)/engine/src/Network/libevent-1.4.14b-stable/mac
endif

TEMP=$(filter-out $(Filters), $(FILES))
MODULES=$(TEMP:%.c=%.o)

INCLUDE2=-I$(ROOT_DIR)/engine/src/Network/libevent-1.4.14b-stable
INCLUDE1=-I$(ROOT_DIR)/engine/src/Network/libevent-1.4.14b-stable/compat

BUILD_DEFINE=-DHAVE_CONFIG_H
DEPEND_LIB=-lrt

TARGET=libevent.so

$(TARGET):$(MODULES)
	@$(GCC) $(BUILD_DEFINE) $(SHARED_LIB) $(TARGET) $(MODULES) ##$(DEPEND_LIB_COMMON) $(DEPEND_LIB)
	@echo Linking ..... $(TARGET)
	@cp $(TARGET) $(ROOT_DIR)/$(OUTPUT_DIR)
	@echo "Copy " $(TARGET) " to " $(ROOT_DIR)/$(OUTPUT_DIR)

$(MODULES):%.o:%.c
	@$(GCC) $(BUILD_DEFINE) $(MODULE) $(INCLUDE) $(INCLUDE1) $(INCLUDE2) $< -o $@
	@echo Compiling ..... $<

clean :
	rm $(TARGET) *.o
