output: display_monitor.o slc3.o lc3.o cpu.o alu.o memory.o
	gcc -g display_monitor.o slc3.o lc3.o cpu.o alu.o memory.o -lncurses -lmenu -lm
memory.o: memory.c
	gcc -c -g memory.c
alu.o: alu.c
	gcc -c -g alu.c
cpu.o: cpu.c
	gcc -c -g cpu.c
lc3.o: lc3.c
	gcc -c -g lc3.c
slc3.o: slc3.c
	gcc -c -g slc3.c
display_monitor.o: display_monitor.c
	gcc -c -g display_monitor.c