#include <stdbool.h>
#include <time.h>

#ifndef STUDENTS_H
#define STUDENTS_H

#define MAX_STUDENT_NAME_LENGTH 256
#define MAX_CLASS_NAME_LENGTH 256
#define MAX_EMAIL_ADDRESS_LENGTH 320
#define MAX_CLUB_MEMBERSHIPS 100

struct Student
{
    int id;
    char name[MAX_STUDENT_NAME_LENGTH];
    char class[MAX_CLASS_NAME_LENGTH];
    char email_address[MAX_EMAIL_ADDRESS_LENGTH];
    int club_memberships[MAX_CLUB_MEMBERSHIPS];
    time_t registered_at;
};
extern struct Student *students;

void manage_students();
void view_student_info(struct Student student);
void register_student();
void update_student_info(struct Student student, int student_pos);
void delete_student(struct Student student, int student_pos);

extern int prev_student_id;
extern int student_count;
#endif
