#ifndef MENU_SYSTEM_H
#define MENU_SYSTEM_H
void clear_console();
void print_greeting();
char *read_variable_length_input();
char *read_fixed_length_input(int size);
void show_main_menu();
void show_admin_panel();
void prompt_return(char *to);
void print_table(int total_columns, int total_rows, int *column_widths, int total_footer_rows, char *column_headings[], char ***contents, char *footer_rows[]);
time_t parse_time_input();
char *format_time_t();
#endif