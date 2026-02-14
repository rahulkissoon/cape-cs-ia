#ifndef MENU_SYSTEM_H_INCLUDED
#define MENU_SYSTEM_H_INCLUDED
void clear_console();
void print_greeting();
char *read_variable_length_input();
void show_main_menu();
void show_admin_panel();
void prompt_return(char *to);
void print_table(int total_columns, int total_rows, int *column_widths, int total_footer_rows, char *column_headings[], char ***contents, char *footer_rows[]);
time_t parse_time_input();
char *format_time_t(time_t timestamp);
void report_alloc_error(long long int bytes);
#endif