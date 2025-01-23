#include "students.h"
#include <time.h>

#ifndef CLUBS_H
#define CLUBS_H
void register_club();
void manage_clubs();
void view_club_info();
void view_club_members();
void update_club();
void delete_club();
void save_data_to_file();
void load_data_from_file();

#define MAX_CLUBS
#define MAX_CLUB_NAME_LENGTH 255
#define MAX_CLUB_DESCRIPTION_LENGTH 512
#define MAX_WEEKLY_MEETING_DAY_LENGTH 9
#define MAX_MEMBERS_PER_CLUB 1024
#define MAX_STUDENT_REPS_PER_CLUB 1024

struct Club
{
    int id;
    char name[MAX_CLUB_NAME_LENGTH];
    char description[MAX_CLUB_DESCRIPTION_LENGTH];
    int student_rep_ids[MAX_STUDENT_REPS_PER_CLUB];
    int student_rep_count;
    int member_ids[MAX_MEMBERS_PER_CLUB];
    int member_count;
    char weekly_meeting_day[MAX_WEEKLY_MEETING_DAY_LENGTH];
    char *password;
    int password_length;
    time_t registered_at;
};

extern struct Club clubs[MAX_CLUBS];

extern int club_count;
extern int prev_club_id;
#endif
