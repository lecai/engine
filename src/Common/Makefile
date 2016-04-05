ROOT_DIR=../../

include $(ROOT_DIR)/Makefile.include

FILES=$(wildcard *.cpp)
MODULES=$(FILES:%.cpp=%.o)

TARGET=libCommom.so

$(TARGET):$(MODULES)
	@$(CC) $(BUILD_SYSTEM) $(SHARED_LIB) $(TARGET) $(MODULES)
	@echo Linking ..... $(TARGET)
	@cp $(TARGET) $(ROOT_DIR)/$(OUTPUT_DIR)
	@echo "Copy " $(TARGET) " to " $(ROOT_DIR)/$(OUTPUT_DIR)

$(MODULES):%.o:%.cpp
	@$(CC) $(BUILD_SYSTEM) $(MODULE) $< -o $@
	@echo Compiling ..... $<
clean :
	rm $(TARGET) *.o