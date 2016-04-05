ROOT_DIR=../../../..

include $(ROOT_DIR)/Makefile.include

FILES=$(wildcard *.cc)
INCLUDE=-I$(ROOT_DIR)/engine/src/Network/snappy-1.0.5

Filters=snappy-test.cc snappy_unittest.cc
TEMP=$(filter-out $(Filters), $(FILES))
MODULES=$(TEMP:%.cc=%.o)


TARGET=libsnappy.so

$(TARGET):$(MODULES)
	@$(CC) $(SHARED_LIB) $(TARGET) $(MODULES)
	@echo Linking ..... $(TARGET)
	@cp $(TARGET) $(ROOT_DIR)/$(OUTPUT_DIR)
	@echo "Copy " $(TARGET) " to " $(ROOT_DIR)/$(OUTPUT_DIR)

$(MODULES):%.o:%.cc
	@$(CC) $(MODULE) $(INCLUDE) $< -o $@
	@echo Compiling ..... $<
clean :
	rm $(TARGET) *.o
