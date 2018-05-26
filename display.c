/* LC-3 Emulator
 *
 * Date: May 2018
 *
 * This a terminal-based program that emulates the low-level functions of the
 * 16-bit LC-3 machine based a finite state machine (FSM) interpretation of its
 * operations.
 */

#include "display.h"
#include "global.h"
#include "lc3.h"
#include <curses.h>
#include <menu.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#define INDEX_REG 0
#define INDEX_MEM 1
#define CPU 2

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 4

#define REG_PANEL_WIDTH 36
#define REG_PANEL_HEIGHT 12
#define MEM_PANEL_WIDTH 30
#define MEM_PANEL_HEIGHT 22
#define CPU_PANEL_WIDTH 36
#define CPU_PANEL_HEIGHT 12
#define IO_PANEL_HEIGHT 5
#define HEIGHT_PADDING 2

#define OUTPUT_CONSOLE_LINES 3
#define OUTPUT_CONSOLE_COLS 64

#define CPU_ELEMENTS_COUNT 10

static const char MSG_CPU_HALTED[] = "CPU halted :*)";
static const char MSG_LOAD[] = "1) Enter a program to load >> ";
static const char MSG_LOADED[] = "1) Loaded %s";
static const char MSG_STEP[] = "3) Stepped";
static const char MSG_STEP_NO_FILE[] = "3) No file loaded yet!";
static const char MSG_RUNNING_CODE[] = "4) Running code";
static const char MSG_RUN_NO_FILE[] = "4) No file loaded yet!";
static const char MSG_DISPLAY_MEM[] = "5) Enter the hex address to jump to >> ";
static const char MSG_EDIT_MEM_PPT_ADDR[] = "6) Enter the hex address to edit >> ";
static const char MSG_EDIT_MEM_PPT_DATA[] = "6) Enter the hex data to push to %s >> ";
static const char MSG_EDIT_MEM_NO_FILE[] = "6) No file loaded yet!";
static const char MSG_SET_UNSET_BRKPT[] =
    "8) Enter the hex address to set/unset breakpoint >> ";
static const char MSG_SET_UNSET_BRKPT_CONFIRM[] = "8) Breakpoint was %s";
static const char MSG_BRKPT_HIT[] = "4) Breakpoint hit at %s. Step or run to continue >> ";
static const char MSG_SET_UNSET_BRKPT_NO_FILE[] = "8) No file loaded yet!";
static const char MSG_CPU_HALTED_STEP[] = "3) Cannot step: CPU halted";
static const char MSG_CPU_HALTED_RUN[] = "4) Cannot run: CPU halted";

typedef struct menu_string_t {
    char label[31];
    char description[31];
} menu_string_t;

typedef struct display_t {
    menu_string_t reg_strings[REGISTER_SIZE + 1];
    menu_string_t mem_strings[MEMORY_SIZE + 1];
    menu_string_t cpu_strings[CPU_ELEMENTS_COUNT + 1];

    WINDOW *menu_windows[3];
    WINDOW *input_window, *output_window;
    MENU *menus[3];
    bool_t menus_populated;
    ITEM **menu_list_items[3];
    int item_counts[3];
    int saved_menu_index[3];
    int active_window;
    int c, i;
    char console_content[OUTPUT_CONSOLE_LINES][OUTPUT_CONSOLE_COLS];
    unsigned char console_line_ptr;
    unsigned char console_col_ptr;

    bool breakpoints[MEMORY_SIZE];
} display_t, *display_p;

int initialize_display(display_p disp);
int free_display(display_p);
void print_message(const char *, char *);
void clear_message();
void clear_line(int line);
void print_title(WINDOW *, int, char *, chtype);
void draw_io_window(WINDOW *, char *);
void print_window_titles();
word_t get_word_from_string(char *);

/** Ensure these are at least as big (or equal to) the actual vaues in the CPU.
 * These have +1 because in addition to the actual string representing data in
 * the LC-3, a null
 * value must also be present to signify the end of the array. */

display_p display_create() {
    display_p disp = calloc(1, sizeof(display_t));
    initialize_display(disp);
    return disp;
}

/** Frees all memory associated with Ncurses and prepares the LC-3 simulator for
 * a complete shutdown. */
int display_destroy(display_p disp) {
    free_display(disp);
    int i;
    for (i = 0; i < 3; i++) {
        delwin(disp->menu_windows[i]);
    }
    return endwin();
}

/** Initializes the debug monitor with variables needed throughout the execution
 * of the LC-3 simulator. */
int initialize_display(display_p disp) {
    /* Initialize breakpoint array */
    int i;
    for (i = 0; i < MEMORY_SIZE; i++) {
        disp->breakpoints[i] = false;
    }

    /* Initialize ncurses */
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);

    /* Stores the size of each array */
    disp->item_counts[INDEX_REG] = REGISTER_SIZE;
    disp->item_counts[INDEX_MEM] = MEMORY_SIZE;
    disp->item_counts[CPU] = CPU_ELEMENTS_COUNT;

    disp->saved_menu_index[INDEX_REG] = 0;
    disp->saved_menu_index[INDEX_MEM] = 0;
    disp->saved_menu_index[CPU] = 0;

    disp->console_line_ptr = 0;
    disp->console_col_ptr = 0;

    disp->menu_list_items[INDEX_REG] =
        (ITEM **)calloc(disp->item_counts[INDEX_REG] + 1, sizeof(ITEM *));
    disp->menu_list_items[INDEX_MEM] =
        (ITEM **)calloc(disp->item_counts[INDEX_MEM] + 1, sizeof(ITEM *));
    disp->menu_list_items[CPU] = (ITEM **)calloc(disp->item_counts[CPU] + 1, sizeof(ITEM *));

    /* Create the window instances to be associated with the menus */
    disp->menu_windows[INDEX_REG] =
        newwin(REG_PANEL_HEIGHT, REG_PANEL_WIDTH, HEIGHT_PADDING, 4);
    disp->menu_windows[INDEX_MEM] =
        newwin(MEM_PANEL_HEIGHT, MEM_PANEL_WIDTH, HEIGHT_PADDING, CPU_PANEL_WIDTH + 8);
    disp->menu_windows[CPU] =
        newwin(CPU_PANEL_HEIGHT, CPU_PANEL_WIDTH, REG_PANEL_HEIGHT + HEIGHT_PADDING, 4);

    /** TODO: This flag is uncessary, but only solution I can think of for not attempting to
     * save/restore selected menu item before they're populated. */
    disp->menus_populated = FALSE;

    disp->input_window = newwin(IO_PANEL_HEIGHT, MEM_PANEL_WIDTH + REG_PANEL_WIDTH + 4,
                                MEM_PANEL_HEIGHT + HEIGHT_PADDING + 3, 4);
    draw_io_window(disp->input_window, "Input");
    disp->output_window = newwin(IO_PANEL_HEIGHT + OUTPUT_CONSOLE_LINES - 1,
                                 MEM_PANEL_WIDTH + REG_PANEL_WIDTH + 4,
                                 MEM_PANEL_HEIGHT + IO_PANEL_HEIGHT + HEIGHT_PADDING + 3, 4);
    draw_io_window(disp->output_window, "Output");

    for (i = 0; i < OUTPUT_CONSOLE_LINES; i++) {
        memset(disp->console_content[i], '\0', OUTPUT_CONSOLE_COLS);
    }

    return 0;
}

/** Frees the memory associated with windows within the LC-3 debug monitor and
 * prepares the debug monitor for a refresh of displayed contents. */
int free_display(display_p disp) {
    int i, j;
    for (i = 0; i < 3; i++) {
        /* Unpost and free all the memory taken up */
        unpost_menu(disp->menus[i]);
        free_menu(disp->menus[i]);
        for (j = 0; j < disp->item_counts[i] + 1; ++j) {
            free_item(disp->menu_list_items[i][j]);
        }
    }

    return 0;
}

/** Returns whether the specified address has a breakpoint set */
bool_t display_has_breakpoint(display_p disp, word_t address) {
    int bp_index = get_index_from_address(address);
    return disp->breakpoints[bp_index];
}

/** Prints a message pertaining to a user operation. It could be a prompt if the
 * user just selected an operation, the outcome of an operation, or additional
 * information about the state of the LC-3 */
void print_message(const char *message, char *arg) {
    clear_line(MEM_PANEL_HEIGHT + HEIGHT_PADDING + 1);
    /** Positions the message just below the memory window (the tallest window)
     * with a left padding of 4 */
    attron(COLOR_PAIR(2));
    mvprintw(MEM_PANEL_HEIGHT + HEIGHT_PADDING + 1, 4, message, arg);
    attroff(COLOR_PAIR(2));
    refresh();
}

/** Clears the message displayed just below the list of available user
 * operations */
void clear_line(int line) {
    move(line, 1);
    clrtoeol();
}

/** Prints output resulting from the LC-3 */
void display_print_output(display_p disp, char ch) {
    /* Clear the whole output window */
    wclrtobot(disp->output_window);
    int i;
    /* If the character is a new-line, we'll shift all the lines up and clear
    the last line */
    if (ch == '\n') {
        if (disp->console_line_ptr == OUTPUT_CONSOLE_LINES - 1) {

            for (i = 0; i < OUTPUT_CONSOLE_LINES - 1; i++) {
                strcpy(disp->console_content[i], disp->console_content[i + 1]);
            }
        }
        /* Unless we have less than 3 lines written so far */
        else {
            disp->console_line_ptr++;
        }
        disp->console_col_ptr = 0;
        memset(disp->console_content[disp->console_line_ptr], '\0', OUTPUT_CONSOLE_COLS);
    } else {
        /* Write the character to the correct position.. duh.. */
        disp->console_content[disp->console_line_ptr][disp->console_col_ptr] = ch;
        if (disp->console_col_ptr < OUTPUT_CONSOLE_COLS - 1) {
            /* Increment the current column */
            disp->console_col_ptr++;
        }
    }
    /* If the character is a null-terminator we actually want to decrement the
    pointer. This is because the next character written might want to be on the
    same line (i.e. collecting user input on the same line after a prompt.) The
    user input will overwrite the null-terminator, [[TODO: Then there will be no null
    terminator after a PUTC???]] */
    if (ch == '\0') {
        disp->console_col_ptr--;
    }

    /** Position the message */
    for (i = 0; i < OUTPUT_CONSOLE_LINES; i++) {
        wattron(disp->output_window, COLOR_PAIR(3));
        mvwprintw(disp->output_window, 2 + i, 2, "%s", disp->console_content[i]);
        wattroff(disp->output_window, COLOR_PAIR(3));
    }

    draw_io_window(disp->output_window, "Output");
    refresh();
}

char display_get_input(display_p disp) {
    int prev_active_window = disp->active_window;

    /* Draw all other windows as inactive, draw input as active */
    disp->active_window = -1;
    print_window_titles(disp);
    print_title(disp->input_window, 0, "Input", COLOR_PAIR(2));
    wrefresh(disp->input_window);

    mvprintw(MEM_PANEL_HEIGHT + IO_PANEL_HEIGHT + HEIGHT_PADDING, 6, ">> ");
    move(MEM_PANEL_HEIGHT + IO_PANEL_HEIGHT + HEIGHT_PADDING, 9);
    echo();
    char input_ch = getch();
    noecho();
    clear_line(MEM_PANEL_HEIGHT + IO_PANEL_HEIGHT + HEIGHT_PADDING);

    draw_io_window(disp->input_window, "Input");
    disp->active_window = prev_active_window;
    print_window_titles(disp);
    return input_ch;
}

void print_window_titles(display_p disp) {
    /** Print a border around the windows and print a title */
    print_title(disp->menu_windows[INDEX_REG], 1, "Registers",
                (disp->active_window == INDEX_REG) ? COLOR_PAIR(2) : COLOR_PAIR(1));
    mvwaddch(disp->menu_windows[INDEX_REG], 2, 0, ACS_LTEE);
    mvwhline(disp->menu_windows[INDEX_REG], 2, 1, ACS_HLINE, 38);
    /** mvwaddch(menu_windows[REG], 2, REG_PANEL_WIDTH - 1, ACS_RTEE);
        box(reg_menu_win, 0, 0); */

    print_title(disp->menu_windows[INDEX_MEM], 1, "Memory",
                (disp->active_window == INDEX_MEM) ? COLOR_PAIR(2) : COLOR_PAIR(1));
    mvwaddch(disp->menu_windows[INDEX_MEM], 2, 0, ACS_LTEE);
    mvwhline(disp->menu_windows[INDEX_MEM], 2, 1, ACS_HLINE, 38);
    /** mvwaddch(menu_windows[MEM], 2, MEM_PANEL_WIDTH - 1, ACS_RTEE);
        box(mem_menu_win, 0, 0); */

    print_title(disp->menu_windows[CPU], 1, "CPU Registers",
                (disp->active_window == CPU) ? COLOR_PAIR(2) : COLOR_PAIR(1));
    mvwaddch(disp->menu_windows[CPU], 2, 0, ACS_LTEE);
    mvwhline(disp->menu_windows[CPU], 2, 1, ACS_HLINE, 38);
    /** mvwaddch(menu_windows[CPU], 2, CPU_PANEL_WIDTH - 1, ACS_RTEE);
        box(cpu_menu_win, 0, 0); */

    /* Refresh the menus */
    int i;
    for (i = 0; i < 3; i++) {
        wrefresh(disp->menu_windows[i]);
    }

    refresh();
}

/** Prints the title of a window */
void print_title(WINDOW *win, int y, char *string, chtype color) {
    wattron(win, color);
    mvwprintw(win, y, 4, "%s", string);
    wattroff(win, color);
    refresh();
}

/** Draws the input and output windows with title. This method is called when
 * the display monitor is first created, and every time input or output is
 * erased from the screen */
void draw_io_window(WINDOW *window, char *title) {
    box(window, 0, 0);
    print_title(window, 0, title, COLOR_PAIR(1));
    wrefresh(window);
}

void save_menu_indicies(display_p disp) {
    if (disp->menus_populated == FALSE) {
        return;
    }
    int i;
    for (i = 0; i < 3; i++) {
        ITEM *item = (ITEM *)current_item(disp->menus[i]);
        if (item != NULL) {
            disp->saved_menu_index[i] = item_index(item);
        } else {
            disp->saved_menu_index[i] = 0;
        }
    }
}

void restore_menu_indicies(display_p disp) {
    int i;
    for (i = 0; i < 3; i++) {
        set_current_item(disp->menus[i], disp->menu_list_items[i][disp->saved_menu_index[i]]);
    }
    refresh();
}

/** Updates the Ncurses window each time this function is called. The arrays
 * containing menu data are all rebuilt, the menus are reinstantiated,
 * positioned, and posted to the windows. */
void display_update(display_p disp, const lc3_snapshot_t lc3_snapshot) {
    save_menu_indicies(disp);
    free_display(disp);

    int i;
    for (i = 0; i < disp->item_counts[INDEX_REG]; ++i) {
        sprintf(disp->reg_strings[i].label, "R%d:", i);
        sprintf(disp->reg_strings[i].description, "x%04X",
                lc3_snapshot.cpu_snapshot.registers[i]);
        disp->menu_list_items[INDEX_REG][i] =
            new_item(disp->reg_strings[i].label, disp->reg_strings[i].description);
    }
    disp->menu_list_items[INDEX_REG][i] = new_item((char *)NULL, (char *)NULL);

    /* Create the items for the memory */
    for (i = 0; i < disp->item_counts[INDEX_MEM]; ++i) {
        sprintf(disp->mem_strings[i].label,
                "x%04X:", lc3_snapshot.starting_address + i); /* So to start at x3000 */
        /* If this memory location has a breakpoint we will display a small square
         * next to it. */
        if (disp->breakpoints[i]) {
            sprintf(disp->mem_strings[i].description, "x%04X [x]",
                    lc3_snapshot.memory_snapshot.data[i]);
        } else {
            sprintf(disp->mem_strings[i].description, "x%04X    ",
                    lc3_snapshot.memory_snapshot.data[i]);
        }

        disp->menu_list_items[INDEX_MEM][i] =
            new_item(disp->mem_strings[i].label, disp->mem_strings[i].description);
    }
    disp->menu_list_items[INDEX_MEM][i] = new_item((char *)NULL, (char *)NULL);

    /* Create items for the CPU */
    sprintf(disp->cpu_strings[0].label, "PC:");
    sprintf(disp->cpu_strings[0].description, "x%04X", lc3_snapshot.cpu_snapshot.pc);
    sprintf(disp->cpu_strings[1].label, "IR:");
    sprintf(disp->cpu_strings[1].description, "x%04X", lc3_snapshot.cpu_snapshot.ir);
    sprintf(disp->cpu_strings[2].label, "ALU A:");
    sprintf(disp->cpu_strings[2].description, "x%04X", lc3_snapshot.alu_snapshot.a);
    sprintf(disp->cpu_strings[3].label, "ALU B:");
    sprintf(disp->cpu_strings[3].description, "x%04X", lc3_snapshot.alu_snapshot.b);
    sprintf(disp->cpu_strings[4].label, "MAR:");
    sprintf(disp->cpu_strings[4].description, "x%04X", lc3_snapshot.cpu_snapshot.mar);
    sprintf(disp->cpu_strings[5].label, "MDR:");
    sprintf(disp->cpu_strings[5].description, "x%04X", lc3_snapshot.cpu_snapshot.mdr);
    sprintf(disp->cpu_strings[6].label, "CC N:");
    sprintf(disp->cpu_strings[6].description, "%d", lc3_snapshot.cpu_snapshot.cc_n);
    /* Inserting a blank one so the CC stuff looks better */
    sprintf(disp->cpu_strings[7].label, " ");
    sprintf(disp->cpu_strings[7].description, " ");
    sprintf(disp->cpu_strings[8].label, "CC Z:");
    sprintf(disp->cpu_strings[8].description, "%d", lc3_snapshot.cpu_snapshot.cc_z);
    sprintf(disp->cpu_strings[9].label, "CC P:");
    sprintf(disp->cpu_strings[9].description, "%d", lc3_snapshot.cpu_snapshot.cc_p);

    for (i = 0; i < CPU_ELEMENTS_COUNT; i++) {
        disp->menu_list_items[CPU][i] =
            new_item(disp->cpu_strings[i].label, disp->cpu_strings[i].description);
    }
    disp->menu_list_items[CPU][i] = new_item((char *)NULL, (char *)NULL);

    /* Create menu instances from the lists */
    for (i = 0; i < 3; i++) {
        disp->menus[i] = new_menu((ITEM **)(disp->menu_list_items[i]));
    }
    disp->menus_populated = TRUE;

    /* Set the initially active window */
    keypad(disp->menu_windows[disp->active_window], TRUE);

    /* Set menus to their windows */
    for (i = 0; i < 3; i++) {
        set_menu_win(disp->menus[i], disp->menu_windows[i]);
    }

    /* The the menu sub ??? and its foprint_messrmat menu_format is number of rows, columns
     * for the list's visible contents and scrolls the rest of the list. */
    set_menu_sub(disp->menus[INDEX_REG],
                 derwin(disp->menu_windows[INDEX_REG], REG_PANEL_HEIGHT - 4,
                        REG_PANEL_WIDTH - 4, 3, 1));
    set_menu_format(disp->menus[INDEX_REG], REG_PANEL_HEIGHT - 4, 1);
    /* Memories... */
    set_menu_sub(disp->menus[INDEX_MEM],
                 derwin(disp->menu_windows[INDEX_MEM], MEM_PANEL_HEIGHT - 4,
                        MEM_PANEL_WIDTH - 4, 3, 1));
    set_menu_format(disp->menus[INDEX_MEM], MEM_PANEL_HEIGHT - 4, 1);
    /* CPU... */
    set_menu_sub(disp->menus[CPU], derwin(disp->menu_windows[CPU], CPU_PANEL_HEIGHT - 4,
                                          CPU_PANEL_WIDTH - 4, 3, 1));
    set_menu_format(disp->menus[CPU], MEM_PANEL_HEIGHT - 4, 2);

    /* Set menu mark to the string " * " */
    for (i = 0; i < 3; i++) {
        set_menu_mark(disp->menus[i], " * ");
    }

    print_window_titles(disp);

    /* Post the menus */
    for (i = 0; i < 3; i++) {
        post_menu(disp->menus[i]);
        wrefresh(disp->menu_windows[i]);
    }

    /* Print labels on the window */
    attron(COLOR_PAIR(2));
    mvprintw(0, 4, "Welcome to the LC-3 Simulator Simulator!");
    mvprintw(MEM_PANEL_HEIGHT + 2, 4,
             "1) Load, 3) Step, 4) Run, 5) Show Mem, 6) Edit 8) Set Brkpt 9) Exit");
    mvprintw(LINES - 2, 0, "Use Tab (\\t) to switch active panels");
    mvprintw(LINES - 1, 0, "Arrow Keys to navigate (9 to Exit)");
    attroff(COLOR_PAIR(2));

    restore_menu_indicies(disp);
}

/** The main logic loop for the debug monitor. Listens for user keystrokes and
 * performs debugging operations */
display_result_t display_loop(display_p disp, const lc3_snapshot_t lc3_snapshot) {
    /* Set selected item in memory to be current PC */
    size_t pc_index = get_index_from_address(lc3_snapshot.cpu_snapshot.pc);
    set_current_item(disp->menus[INDEX_MEM], disp->menu_list_items[INDEX_MEM][pc_index]);
    display_update(disp, lc3_snapshot);

    /* Input vars used for 5) Show Mem, 6)Edit Mem, 8) Set/Unset breakpoint */
    char word_input_raw[6];
    word_t word_input;
    /* Secondary input vars used for 6) Edit Mem to specify the data to insert at the address
     */
    char word_data_input_raw[6];
    word_t word_data_input;

    /** This variable is used to return information about the user's selection
     * back to the LC-3. We load it with whether the current PC is a breakpoint. */
    display_result_t display_return = DISPLAY_NO_ACTION;

    if (lc3_snapshot.is_halted) {
        print_message(MSG_CPU_HALTED, NULL);
    }
    int index = get_index_from_address(lc3_snapshot.cpu_snapshot.pc);
    bool_t brkptq = disp->breakpoints[index];
    if (brkptq == TRUE) {
        /** We can reuse an existing char array to store this string */
        sprintf(word_input_raw, "x%04X", lc3_snapshot.cpu_snapshot.pc);
        print_message(MSG_BRKPT_HIT, word_input_raw);
    }

    int c;
    /* If the user selected 9) to quit this while loop will exit */
    while ((c = wgetch(disp->menu_windows[disp->active_window])) != 57) {
        switch (c) {
        case 9:
            /*　User pressed Tab to change active window */
            disp->active_window++;
            disp->active_window = disp->active_window % 3;
            keypad(disp->menu_windows[disp->active_window], TRUE);
            print_window_titles(disp);
            display_update(disp, lc3_snapshot);
            /** Continue the while loop (i.e. wait for another char) */
            continue;
        case 49:
            /* User selected 1) to load a file */
            print_message(MSG_LOAD, NULL);
            /** Move the cursor, turn on echo mode so the user can see their input
             *  then turn it back on after capturing file name input */
            move(MEM_PANEL_HEIGHT + HEIGHT_PADDING + 1, strlen(MSG_LOAD) + 4);
            echo();
            getstr(load_file_input);
            noecho();

            clear_line(MEM_PANEL_HEIGHT + HEIGHT_PADDING + 1);
            print_message(MSG_LOADED, load_file_input);
            display_return = DISPLAY_LOAD;
            break;
        case 51:
            /* User selected 3) to step through code */
            if (lc3_snapshot.file_loaded == FALSE) {
                print_message(MSG_STEP_NO_FILE, NULL);
                continue;
            } else if (lc3_snapshot.is_halted) {
                print_message(MSG_CPU_HALTED_STEP, NULL);
                continue;
            } else {
                print_message(MSG_STEP, NULL);
                display_return = DISPLAY_STEP;
            }
            break;
        case 52:
            /* User selected 4) to run code */
            if (lc3_snapshot.file_loaded == FALSE) {
                print_message(MSG_RUN_NO_FILE, NULL);
                continue;
            } else if (lc3_snapshot.is_halted) {
                print_message(MSG_CPU_HALTED_RUN, NULL);
                continue;
            } else {
                print_message(MSG_RUNNING_CODE, NULL);
                display_return = DISPLAY_RUN;
            }
            break;
        case 53:
            /* User selected 5) to display a specific memory address */
            print_message(MSG_DISPLAY_MEM, NULL);
            /** Move the cursor, turn on echo mode so the user can see their input
             *  then turn it back on after capturing file name input */
            move(MEM_PANEL_HEIGHT + HEIGHT_PADDING + 1, strlen(MSG_DISPLAY_MEM) + 4);
            echo();
            getstr(word_input_raw);
            noecho();
            word_input = get_word_from_string(word_input_raw);
            set_current_item(
                disp->menus[INDEX_MEM],
                disp->menu_list_items[INDEX_MEM][get_index_from_address(word_input)]);
            continue;
            break;
        case 54:
            if (lc3_snapshot.file_loaded == FALSE) {
                print_message(MSG_EDIT_MEM_NO_FILE, NULL);
                continue;
            }
            /* User selected 6) to edit a memory location */
            print_message(MSG_EDIT_MEM_PPT_ADDR, NULL);
            /** Move the cursor, turn on echo mode so the user can see their input
             *  then turn it back on after capturing address */
            move(MEM_PANEL_HEIGHT + HEIGHT_PADDING + 1, strlen(MSG_EDIT_MEM_PPT_ADDR) + 4);
            echo();
            getstr(word_input_raw);
            noecho();
            word_input = get_word_from_string(word_input_raw);
            print_message(MSG_EDIT_MEM_PPT_DATA, word_input_raw);
            /** Move the cursor, turn on echo mode so the user can see their input
             *  then turn it back on after capturing the data */
            move(MEM_PANEL_HEIGHT + HEIGHT_PADDING + 1,
                 strlen(MSG_EDIT_MEM_PPT_DATA) + strlen(word_input_raw) + 4);
            echo();
            getstr(word_data_input_raw);
            noecho();
            word_data_input = get_word_from_string(word_data_input_raw);
            /** TODO: This will not be the correct thing to do here. We need a method in slc3
             * and ultimately lc3 to set memory. */
            /*lc3_snapshot.memory_snapsshot.data[mem_addr_index] = mem_data;*/
            display_update(disp, lc3_snapshot);
            continue;
        case 56:
            /* User selected 8) to set/unset a breakpoint */
            if (lc3_snapshot.file_loaded == FALSE) {
                print_message(MSG_SET_UNSET_BRKPT_NO_FILE, NULL);
                continue;
            } else {
                print_message(MSG_SET_UNSET_BRKPT, NULL);
                /** Move the cursor, turn on echo mode so the user can see their input
                 *  then turn it back on after capturing file name input */
                move(MEM_PANEL_HEIGHT + HEIGHT_PADDING + 1, strlen(MSG_SET_UNSET_BRKPT) + 4);
                echo();
                getstr(word_input_raw);
                noecho();
                word_input = get_word_from_string(word_input_raw);
                disp->breakpoints[get_index_from_address(word_input)] =
                    !disp->breakpoints[get_index_from_address(word_input)];
                /** Reuse the existing char array. It's just big enough for the string "unset\0". */
                sprintf(word_input_raw, disp->breakpoints[get_index_from_address(word_input)] ? "set" : "unset");
                print_message(MSG_SET_UNSET_BRKPT_CONFIRM, word_input_raw);
                save_menu_indicies(disp);
                display_update(disp, lc3_snapshot);
                restore_menu_indicies(disp);
            }
            display_update(disp, lc3_snapshot);
            break;
        case KEY_DOWN:
            menu_driver(disp->menus[disp->active_window], REQ_DOWN_ITEM);
            continue;
        case KEY_UP:
            menu_driver(disp->menus[disp->active_window], REQ_UP_ITEM);
            continue;
        case KEY_LEFT:
            menu_driver(disp->menus[disp->active_window], REQ_LEFT_ITEM);
            continue;
        case KEY_RIGHT:
            menu_driver(disp->menus[disp->active_window], REQ_RIGHT_ITEM);
            continue;
        case KEY_NPAGE:
            menu_driver(disp->menus[disp->active_window], REQ_SCR_DPAGE);
            continue;
        case KEY_PPAGE:
            menu_driver(disp->menus[disp->active_window], REQ_SCR_UPAGE);
            continue;
        }

        /* Shows the last inputted character as Ncurses recognizes it */
        attron(COLOR_PAIR(1));
        mvprintw(LINES - 3, 0, "Debug: %d", c, c);
        attroff(COLOR_PAIR(1));

        refresh();

        /** Reaching this point means no 'continue;' was hit. Time to pass control back to the
         * simulator */
        return display_return;
    }

    /** The only way to break out of the while loop and reach this point is pressing 9 */
    return DISPLAY_QUIT;
}

/** Returns a 16 bit LC3 word parsed from the specified string */
word_t get_word_from_string(char *mem_string) {
    /* A little bit of edge case handling...
     * x3002 + strlen("x3002") - 4 = 3002
     * 3002 + strlen("3002") - 4 = 3002 */
    unsigned short address = strtol(mem_string + strlen(mem_string) - 4, NULL, 16);
    return (word_t)address;
}
