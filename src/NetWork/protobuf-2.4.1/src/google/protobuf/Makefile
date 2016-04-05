ROOT_DIR=../../../../../../..

include $(ROOT_DIR)/Makefile.include

FILES=$(wildcard *.cc)
MODULES=$(FILES:%.cc=%.o)

Filters=io/gzip_stream.cc

SUBDIR=io stubs

SUBSRC=$(shell find $(SUBDIR) -name '*.cc')
TEMP=$(filter-out $(Filters), $(SUBSRC))
SUBOBJ=$(TEMP:%.cc=%.o)

INCLUDE=-I$(ROOT_DIR)/engine/src/Network/protobuf-2.4.1/src
INCLUDE1=-I$(ROOT_DIR)/engine/src/Network/protobuf-2.4.1

TARGET=libprotobuf.so

$(TARGET):$(MODULES) $(SUBOBJ)
	@$(CC) $(SHARED_LIB) $(TARGET) $(MODULES) $(SUBOBJ);
	@echo Linking ..... $(TARGET)
	@cp $(TARGET) $(ROOT_DIR)/$(OUTPUT_DIR)
	@echo copy $(TARGET) to $(ROOT_DIR)/$(OUTPUT_DIR)

$(MODULES):%.o:%.cc
	@$(CC) $(MODULE) $(INCLUDE) $(INCLUDE1) $< -o $@;
	@echo Compiling ..... $<

$(SUBOBJ):%.o:%.cc
	@$(CC) $(MODULE) $(INCLUDE) $(INCLUDE1) $< -o $@;
	@echo Compiling ..... $<

clean:
	rm $(TARGET) *.o
