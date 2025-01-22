#include <stdbool.h>
#include <time.h>

#ifndef STUDENTS_H
#define STUDENTS_H

#define MAX_STUDENTS 1024
#define MAX_NAME_LENGTH 128
#define MAX_CLASS_NAME_LENGTH 128

void register_student();
void manage_students();
void view_student_info();

struct Student
{
    int id;
    char name[MAX_NAME_LENGTH];
    char class[MAX_CLASS_NAME_LENGTH];
    bool is_active;
    int club_memberships[1024];
    time_t registered_at;
};
extern struct Student students[MAX_STUDENTS];

extern int last_student_id;
extern int student_count;
#endif
