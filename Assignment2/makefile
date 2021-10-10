I_DIR = include
O_DIR = obj
S_DIR = src
CC = mpicc
CFLAGS = -Wall -c -I$(I_DIR) -lm
LFLAGS = -Wall -o -I$(I_DIR) -lm
SRC := $(wildcard $(S_DIR)/*.c)
OBJ := $(patsubst %.c,$(O_DIR)/%.o,$(notdir $(SRC)))

.PHONY: mkdir clean new main all

.SILENT:

main: mkdir $(OBJ) main.c
	$(CC) $(LFLAGS) $(OBJ) main.c -o Out && echo Successful make: Executable is ./Out

$(O_DIR)/%.o: $(S_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $(O_DIR)/$(@F)

mkdir:
	mkdir -pv $(O_DIR) $(S_DIR) $(I_DIR)

clean:
	rm obj/* Out -vf
	test -d "obj" && rmdir obj -v || :

new: clean main
