output: debug_monitor.o slc3.o 
	gcc -g debug_monitor.o slc3.o -lncurses -lmenu -lm
slc3.o: slc3.c
	gcc -c -g slc3.c
debug_monitor.o: debug_monitor.c
	gcc -c -g debug_monitor.c