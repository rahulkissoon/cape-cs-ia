#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED
#define MAX_SCHOOL_NAME_LENGTH 68

void first_time_setup();
void load_data_from_file();
void save_data_to_file();
void change_school_name();

extern char school_name[MAX_SCHOOL_NAME_LENGTH];
#endif
