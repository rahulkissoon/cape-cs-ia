#include <time.h>
#include "auth.h"
#include "students.h"
#include "core.h"

#ifndef CLUBS_H_INCLUDED
#define CLUBS_H_INCLUDED

#define MAX_CLUBS 100
#define MAX_CLUB_NAME_LENGTH 68
#define MAX_CLUB_DESCRIPTION_LENGTH 100
#define MAX_WEEKLY_MEETING_DAY_LENGTH 9
#define MAX_MEMBERS_PER_CLUB 1024
#define MAX_STUDENT_REPS_PER_CLUB 1024
#define MAX_MEETING_TOPIC_LENGTH 40

struct Meeting
{
    int id;
    int club_id;
    char topic[MAX_MEETING_TOPIC_LENGTH];
    struct Node *present_member_ids;
    int present_member_count;
    struct Node *absent_member_ids;
    int absent_member_count;
    time_t convened_at;
    time_t adjourned_at;
};

struct Club
{
    int id;
    char name[MAX_CLUB_NAME_LENGTH];
    char description[MAX_CLUB_DESCRIPTION_LENGTH];
    struct Node *student_rep_ids;
    int student_rep_count;
    struct Node *member_ids;
    int member_count;
    char weekly_meeting_day[MAX_WEEKLY_MEETING_DAY_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    time_t registered_at;
    int meeting_count;
    int prev_meeting_id;
    int transaction_count;
    int prev_transaction_id;
};

void manage_clubs();
void register_club();
void view_club_info(struct Club club);
void manage_club_members(int club_pos);
void enroll_new_member(struct Club club, int club_pos);
void rescind_membership(struct Club club, int club_pos);
void update_club_info(struct Club club, int club_pos);
void delete_club(struct Club club, int club_pos);
void post_meeting(struct Club club, int club_pos);
struct Meeting edit_attendance_sheet(struct Club club, struct Meeting meeting);
void list_club_meetings(int club_pos);

extern struct Club clubs[MAX_CLUBS];
extern struct Meeting *meetings;

extern int club_count;
extern int prev_club_id;
extern int meeting_count;
#endif
