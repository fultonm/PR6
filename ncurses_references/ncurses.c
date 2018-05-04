#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <menu.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 4

char *choices[] = {
    "Choice 1",
    "Choice 2",
    "Choice 3",
    "Choice 4",
    "Choice 5",
    "Choice 6",
    "Choice 7",
    "Choice 8",
    "Choice 9",
    "Choice 10",
    "Choice 11",
    "Choice 12",
    "Choice 13",
    "Choice 14",
    "Choice 15",
    "Choice 16",
    "Choice 17",
    "Choice 18",
    "Choice 19",
    "Choice 20",
    "Exit",
    (char *)NULL,
};

int main()
{
    ITEM **my_items;
    int c;
    MENU *my_menu, *my_menu_2;
    WINDOW *my_menu_win, *my_menu_win_2;
    int n_choices, i;

    /* Initialize curses */
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);

    /* Create items */
    n_choices = ARRAY_SIZE(choices);
    my_items = (ITEM **)calloc(n_choices, sizeof(ITEM *));
    for (i = 0; i < n_choices; ++i)
        my_items[i] = new_item(choices[i], choices[i]);

    /* Crate menu */
    my_menu = new_menu((ITEM **)my_items);
    my_menu_2 = new_menu((ITEM **)my_items);

    /* Set menu option not to show the description */
    menu_opts_off(my_menu, O_SHOWDESC);
    menu_opts_off(my_menu_2, O_SHOWDESC);
    /* Create the window to be associated with the menu */
    my_menu_win = newwin(15, 45, 10, 10);
    my_menu_win_2 = newwin(15, 45, 10, 60);
    keypad(my_menu_win, TRUE);
    keypad(my_menu_win_2, TRUE);

    /* Set main window and sub window */
    set_menu_win(my_menu, my_menu_win);
    set_menu_sub(my_menu, derwin(my_menu_win, 10, 50, 5, 2));
    set_menu_format(my_menu, 10, 1);
    set_menu_mark(my_menu, " * ");

    /* Set main window and sub window */
    set_menu_win(my_menu_2, my_menu_win_2);
    set_menu_sub(my_menu_2, derwin(my_menu_win_2, 10, 50, 5, 2));
    set_menu_format(my_menu_2, 10, 1);
    set_menu_mark(my_menu_2, " * ");

    /* Print a border around the main window and print a title */
    box(my_menu_win, 0, 0);
    box(my_menu_win_2, 0, 0);

    attron(COLOR_PAIR(2));
    mvprintw(LINES - 3, 0, "Use PageUp and PageDown to scroll");
    mvprintw(LINES - 2, 0, "Use Arrow Keys to navigate (F1 to Exit)");
    attroff(COLOR_PAIR(2));
    refresh();

    /* Post the menu */
    post_menu(my_menu);
    wrefresh(my_menu_win);

    post_menu(my_menu_2);
    wrefresh(my_menu_win_2);

    while ((c = wgetch(my_menu_win)) != KEY_F(1))
    {
        switch (c)
        {
        case KEY_DOWN:
            menu_driver(my_menu, REQ_DOWN_ITEM);
            break;
        case KEY_UP:
            menu_driver(my_menu, REQ_UP_ITEM);
            break;
        case KEY_LEFT:
            menu_driver(my_menu, REQ_LEFT_ITEM);
            break;
        case KEY_RIGHT:
            menu_driver(my_menu, REQ_RIGHT_ITEM);
            break;
        case KEY_NPAGE:
            menu_driver(my_menu, REQ_SCR_DPAGE);
            break;
        case KEY_PPAGE:
            menu_driver(my_menu, REQ_SCR_UPAGE);
            break;
        }
        wrefresh(my_menu_win);
    }

    /* Unpost and free all the memory taken up */
    unpost_menu(my_menu);
    free_menu(my_menu);
    for (i = 0; i < n_choices; ++i)
        free_item(my_items[i]);
    endwin();
}