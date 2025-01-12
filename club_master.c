#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "menu_system.h"
#include "club_master.h"

struct Club
{
    char name[255];
    char student_reps[255];
    char weekly_meeting_day[9];
    time_t registered_at;
};

struct Member
{
    char name[255];
    char class[255];
    time_t joined_since;
};

void register_club()
{
    struct Club club = {"", "", "", time(NULL)};

    printf("Name: ");
    while (club.name[0] == '\0')
    {
        fgets(club.name, sizeof(club.name), stdin);
        club.name[strlen(club.name) - 1] = '\0';
    }
    clear_console();
    printf("Name: %s\n", club.name);

    printf("Weekly Meeting Day: Choose a number corresponding to the desired weekday from the list presented below.\n");
    printf("[1] Monday\n");
    printf("[2] Tuesday\n");
    printf("[3] Wednesday\n");
    printf("[4] Thursday\n");
    printf("[5] Friday\n");
    char weekly_meeting_day = '\0';
    while (club.weekly_meeting_day[0] == '\0')
    {
        weekly_meeting_day = getch();
        switch (weekly_meeting_day)
        {
        case '1':
            strcpy(club.weekly_meeting_day, "Monday");
            break;
        case '2':
            strcpy(club.weekly_meeting_day, "Tuesday");
            break;
        case '3':
            strcpy(club.weekly_meeting_day, "Wednesday");
            break;
        case '4':
            strcpy(club.weekly_meeting_day, "Thursday");
            break;
        case '5':
            strcpy(club.weekly_meeting_day, "Friday");
            break;
        }
    }
    clear_console();
    printf("Name: %s\n", club.name);
    printf("Weekly Meeting Day: %s\n", club.weekly_meeting_day);

    printf("Student Representatives: ");
    while (club.student_reps[0] == '\0')
    {
        fgets(club.student_reps, sizeof(club.student_reps), stdin);
        club.student_reps[strlen(club.student_reps) - 1] = '\0';
    }
    clear_console();

    printf("Confirm registration of '%s'? (y/n) ", club.name);
    char confirmation;
    scanf(" %c", &confirmation);
    printf("\n");
    if (tolower(confirmation) != 'y')
    {
        printf("Registration of '%s' cancelled. ", club.name);
        prompt_return_to_main_menu();
        return;
    }

    clear_console();
    printf("New club successfully registered.\n\n");
    printf("> Name: %s\n", club.name);
    printf("> Registered At: %s", ctime(&club.registered_at));
    printf("> Member Count: 0\n");
    printf("> Weekly Meeting Day: %s\n", club.weekly_meeting_day);
    printf("> Student Representatives: %s\n\n", club.student_reps);
    prompt_return_to_main_menu();
}
