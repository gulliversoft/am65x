######### Make script for the LightSense demo ###########
### Author: Martin Shishkov, gulliversoft, 2020/2021 ####
#########################################################

mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))
root_path := $(subst $(current_dir)/Makefile,, $(mkfile_path))

DIR_LIB = ../
DIR_BIN = ../build

vpath %.c $(DIR_LIB)
vpath %.c ${CURDIR}/src
vpath %.h $(DIR_LIB)
vpath %.h ${CURDIR}/src

OBJ_C = $(wildcard ${DIR_LIB}/*.c)
OBJ_C += $(wildcard ${CURDIR}/src/*.c)

OBJ_O = $(patsubst %.c,${DIR_BIN}/%.o,$(notdir ${OBJ_C}))

TARGET = LightSense

CC = gcc

CFLAGS = -g -O0 -Wall 

LIB = $(root_path)dependency/$(shell uname -m)/libi2c.a

${TARGET}:${OBJ_O}
	@echo $(OBJ_O)
	@echo creating $@ ...
	$(CC) $(CFLAGS) $(OBJ_O) -o $@ $(LIB)

${DIR_BIN}/%.o : %.c
	@echo $(OBJ_C)
	@echo compiling $< ... $@
	$(CC) $(CFLAGS) -c  $< -o $@
	$(if $(filter $<,../am65x.c), @echo equal,@echo non equal)

clean :
	rm $(DIR_BIN)/*.* 
	rm $(TARGET) 
