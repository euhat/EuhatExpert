CPLUSPLUS=g++
CCOMPILER=gcc

DIR_EUHAT=../../euhat
DIR_OS=$(DIR_EUHAT)/os/linux
DIR_OUTPUT=release

CFLAGS=-g -I. -I$(DIR_OS) -I$(DIR_EUHAT) -Wl,-Map=test.map -no-pie -rdynamic `pkg-config --cflags gtk+-3.0`
LDFLAGS=`pkg-config --libs gtk+-3.0` -lpthread -ldl

SOURCES_COMMON=$(wildcard $(DIR_EUHAT)/common/*.cpp)
SOURCES_OS=$(wildcard $(DIR_OS)/*.cpp)
SOURCES_DBOP=$(wildcard $(DIR_EUHAT)/dbop/*.cpp)
SOURCES_OPENSSL=$(wildcard $(DIR_EUHAT)/openssl/*.c)
SOURCES_OPENSSL_BN=$(wildcard $(DIR_EUHAT)/openssl/bn/*.c)
SOURCES_SQLITE=$(DIR_EUHAT)/sqlite/sqlite3euhat.c
SOURCES=$(wildcard *.cpp)

OBJS_COMMON=$(SOURCES_COMMON:$(DIR_EUHAT)/common/%.cpp=$(DIR_OUTPUT)/common/%.o)
OBJS_OS=$(SOURCES_OS:$(DIR_OS)/%.cpp=$(DIR_OUTPUT)/os/%.o)
OBJS_DBOP=$(SOURCES_DBOP:$(DIR_EUHAT)/dbop/%.cpp=$(DIR_OUTPUT)/dbop/%.o)
OBJS_OPENSSL=$(SOURCES_OPENSSL:$(DIR_EUHAT)/openssl/%.c=$(DIR_OUTPUT)/openssl/%.o)
OBJS_OPENSSL_BN=$(SOURCES_OPENSSL_BN:$(DIR_EUHAT)/openssl/bn/%.c=$(DIR_OUTPUT)/openssl/bn/%.o)
OBJS_SQLITE=$(SOURCES_SQLITE:$(DIR_EUHAT)/sqlite/%.c=$(DIR_OUTPUT)/sqlite/%.o)
OBJS=$(SOURCES:%.cpp=$(DIR_OUTPUT)/%.o)

TARGET=./release/test

all: before $(TARGET)

before:
	@test -d $(DIR_OUTPUT) || mkdir -p $(DIR_OUTPUT)
	@test -d $(DIR_OUTPUT)/os || mkdir -p $(DIR_OUTPUT)/os
	@test -d $(DIR_OUTPUT)/common || mkdir -p $(DIR_OUTPUT)/common
	@test -d $(DIR_OUTPUT)/dbop || mkdir -p $(DIR_OUTPUT)/dbop
	@test -d $(DIR_OUTPUT)/openssl || mkdir -p $(DIR_OUTPUT)/openssl
	@test -d $(DIR_OUTPUT)/openssl/bn || mkdir -p $(DIR_OUTPUT)/openssl/bn
	@test -d $(DIR_OUTPUT)/sqlite || mkdir -p $(DIR_OUTPUT)/sqlite

$(OBJS_COMMON): $(DIR_OUTPUT)/common/%.o: $(DIR_EUHAT)/common/%.cpp
	$(CPLUSPLUS) -c $(CFLAGS) $< -o $@

$(OBJS_OS): $(DIR_OUTPUT)/os/%.o: $(DIR_OS)/%.cpp
	$(CPLUSPLUS) -c $(CFLAGS) $< -o $@

$(OBJS_DBOP): $(DIR_OUTPUT)/dbop/%.o: $(DIR_EUHAT)/dbop/%.cpp
	$(CPLUSPLUS) -c $(CFLAGS) $< -o $@

$(OBJS_OPENSSL): $(DIR_OUTPUT)/openssl/%.o: $(DIR_EUHAT)/openssl/%.c
	$(CCOMPILER) -c $(CFLAGS) $< -o $@

$(OBJS_OPENSSL_BN): $(DIR_OUTPUT)/openssl/bn/%.o: $(DIR_EUHAT)/openssl/bn/%.c
	$(CCOMPILER) -c $(CFLAGS) $< -o $@

$(OBJS_SQLITE): $(DIR_OUTPUT)/sqlite/%.o: $(DIR_EUHAT)/sqlite/%.c
	$(CCOMPILER) -c $(CFLAGS) $< -o $@

$(OBJS): $(DIR_OUTPUT)/%.o: %.cpp
	$(CPLUSPLUS) -c $(CFLAGS) $< -o $@

$(TARGET): $(OBJS) $(OBJS_SQLITE) $(OBJS_OPENSSL_BN) $(OBJS_OPENSSL) $(OBJS_DBOP) $(OBJS_OS) $(OBJS_COMMON)
	$(CPLUSPLUS) -o $@ $^ $(LDFLAGS)

clean:
	rm -Rf release

run:
	$(TARGET)

dbg:
	gdb --args $(TARGET)
