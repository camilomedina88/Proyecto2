################################################################################
# Makefile to make sample application
################################################################################

CC=gcc
CFLAGS= -DZIP_V2 -DCYASSL_DTLS -D_POSIX_THREADS -DDEBUG -DDEBUG_CYASSL -DHAVE_THREAD_LS -pthread -DNO_DSA -DNO_DH -DNO_MD4 -DNO_PWDBASED -DNO_HC128 -DNO_RABBIT
LDFLAGS=

RM := rm -rf

LIBS := -lpthread -lsqlite3
COMPILE_OPTION = -O2

SRC_OBJS = servidor.o serial_data.o 

LIB_FILES = 

LIB_PATH = 

SRC_HEADERS = 

INCLUDE_PATH =

TARGET_BIN = servidor

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
	cp index.html /www/pages

# Other Targets
clean:
	-$(RM) *.o
	-$(RM) $(TARGET_BIN)
	-@echo ' '

.PHONY: all clean
