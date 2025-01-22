#ifndef MENU_SYSTEM_H
#define MENU_SYSTEM_H
extern const int BACKSPACE_ASCII_CODE;

void clear_console();
void show_main_menu();
char *accept_variable_length_input();
void prompt_return_to_main_menu();
void print_greeting();
#endif