ROOT_DIR=../../../..

include $(ROOT_DIR)/Makefile.include

FILES=$(wildcard *.cpp)
MODULES=$(FILES:%.cpp=%.o)


INCLUDE=-I$(ROOT_DIR)/engine/src/Common
INCLUDE3=-I$(ROOT_DIR)/engine/src/Network/libevent-1.4.14b-stable/mac
INCLUDE2=-I$(ROOT_DIR)/engine/src/
INCLUDE1=-I$(ROOT_DIR)/engine/src/Network/libevent-1.4.14b-stable

TARGET=libnet.so

DEPEND_LIB=-lsnappy -lprotobuf -levent -lCommon

$(TARGET):$(MODULES)
	@$(CC) $(SHARED_LIB) $(TARGET) $(MODULES) -L$(ROOT_DIR)/$(OUTPUT_DIR) $(DEPEND_LIB);
	@echo Linking ..... $(TARGET)
	@cp $(TARGET) $(ROOT_DIR)/$(OUTPUT_DIR)
	@echo "Copy " $(TARGET) " to " $(ROOT_DIR)/$(OUTPUT_DIR)

$(MODULES):%.o:%.cpp
	@$(CC) $(MODULE) $(INCLUDE) $(INCLUDE1) $(INCLUDE2) $(INCLUDE3) $< -o $@
	@echo Compiling ..... $<
clean :
	rm $(TARGET) *.o
