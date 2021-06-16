OBJS = main.c
OBJ_NAME = main

all : $(OBJS)
				gcc $(OBJS) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
				LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib ./a.out
