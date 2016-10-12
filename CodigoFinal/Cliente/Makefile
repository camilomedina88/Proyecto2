################################################################################
# Makefile to make sample application
################################################################################

CC=g++
CFLAGS= 
LDFLAGS=

RM := rm -rf

LIBS := -lpthread -lwebsockets -lmraa
COMPILE_OPTION = -O2

SRC_OBJS = cliente.o 

LIB_FILES = 

LIB_PATH = 

SRC_HEADERS = 

INCLUDE_PATH =

TARGET_BIN = cliente

# All Target
all: $(TARGET_BIN)

# Compile c source file
%.o: %.c $(SRC_HEADERS)
	@echo 'Compiling file: $<'
	$(CC) $(COMPILE_OPTION) $(INCLUDE_PATH) -Wall $(CFLAGS) -c -o"$@" "$<"
	@echo 'Finished compiling: $<'
	@echo ' '

# Tool invocations
$(TARGET_BIN): $(SRC_OBJS) $(LIB_FILES)
	@echo 'Building target: $@'
	$(CC) $(LIB_PATH) $(LDFLAGS) -o $(TARGET_BIN) $(SRC_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

install:
	
	cp test /opt/LivingHub/zwave/

# Other Targets
clean:
	-$(RM) *.o
	-$(RM) $(TARGET_BIN)
	-@echo ' '

.PHONY: all clean
