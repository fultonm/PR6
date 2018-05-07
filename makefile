output: display_monitor.o slc3.o 
	gcc -g display_monitor.o slc3.o -lncurses -lmenu -lm
slc3.o: slc3.c
	gcc -c -g slc3.c
display_monitor.o: display_monitor.c
	gcc -c -g display_monitor.c