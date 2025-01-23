#ifndef DATABASE_H
#define DATABASE_H
#define MAX_SCHOOL_NAME_LENGTH 68

void first_time_setup();
void load_data_from_file();
void save_data_to_file();

extern char school_name[MAX_SCHOOL_NAME_LENGTH];
#endif
