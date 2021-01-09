######### Make script for the LightSense demo ###########
### Author: Martin Shishkov, gulliversoft, 2020/2021 ####
#########################################################

DIR_LIB = ../
export DIR = $(DIR_LIB)
DIR_BIN = ./bin

vpath %.c $(DIR_LIB)
vpath %.c ${CURDIR}
vpath %.h $(DIR_LIB)
vpath %.h ${CURDIR}

OBJ_C = $(wildcard ${DIR}/*.c)
OBJ_C += $(wildcard ${CURDIR}/*.c)

OBJ_O = $(patsubst %.c,${DIR_BIN}/%.o,$(notdir ${OBJ_C}))

TARGET = LightSense

CC = gcc

CFLAGS = -g -O0 -Wall 

LIB = /usr/lib/aarch64-linux-gnu/libi2c.a

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