# SPDX-License-Identifier: MIT
# Author:  Giovanni Santini
# Mail:    giovanni.santini@proton.me
# License: MIT

#
# Compiler flags
#
CFLAGS        = -Wall -Werror -Wextra -Wpedantic -std=c99 -I ../../include -rdynamic
DEBUG_FLAGS   = -ggdb
RELEASE_FLAGS = -DCOMMIT_HASH=$(shell git rev-parse --verify HEAD)
LDFLAGS       = -lXrandr -lX11
CC?           = gcc

#
# Project files
#
OUT_NAME = hot_reloading
OBJ      = hot_reloading.o \
           micro-engine-impl.o

# App module
MODULE_NAME = app_module.so
MODULE_OBJ  = app_module.o
MODULE_FLAGS = -shared -fPIC

#
# Commands
#
all: $(OUT_NAME)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(OUT_NAME)

release: CFLAGS += $(RELEASE_FLAGS) 
release: $(OUT_NAME)

run: $(OUT_NAME)
	chmod +x $(OUT_NAME)
	./$(OUT_NAME)

clean:
	rm -f $(OBJ)

distclean:
	rm -f $(OUT_NAME)

$(OUT_NAME): $(OBJ) $(MODULE_NAME)
	$(CC) $(OBJ) $(LDFLAGS) $(CFLAGS) -o $(OUT_NAME)

$(MODULE_NAME): $(MODULE_OBJ)
	$(CC) $(MODULE_OBJ) $(LDFLAGS) $(CFLAGS) $(MODULE_FLAGS) -o $(MODULE_NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
