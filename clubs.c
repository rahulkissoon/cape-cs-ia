#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "auth.h"
#include "menu_system.h"
#include "clubs.h"
#include "database.h"
#include "students.h"

struct Club clubs[MAX_CLUBS] = {{0}};

int club_count = 0;

int last_club_id = 0;

void manage_clubs()
{
    while (true)
    {
        print_greeting();
        if (club_count == 0)
        {
            printf("No clubs have yet been registered. ");
            return prompt_return_to_main_menu();
        }

        printf("MANAGE CLUBS\n\n");
        printf("Type in the number indicated in brackets preceding the name and then press [Enter] to select the club. Alternatively, enter \"0\" to return to the main menu.\n\n");
        for (int i = 0; i < club_count; i++)
        {
            struct Club club = clubs[i];
            printf("[%d] %s\n", club.id, club.name);
        }
        printf("\n");

        int club_pos = 0;
        while (true)
        {
            char inputted_club_pos[5];
            fgets(inputted_club_pos, sizeof(inputted_club_pos), stdin);
            if (strcmp(inputted_club_pos, "0\n") == 0)
            {
                break;
            }

            club_pos = atoi(inputted_club_pos);
            if (club_pos <= club_count && club_pos != 0)
            {
                break;
            }
        }
        if (club_pos == 0)
        {
            break;
        }

        struct Club club = clubs[club_pos - 1];
        bool is_authorized = prompt_authorization("Selecting a club", club.password, STANDARD, "the main menu");
        if (!is_authorized)
        {
            return;
        }

        char choice = '\0';
        while (choice != BACKSPACE_ASCII_CODE)
        {
            print_greeting();
            printf("MANAGE CLUB '%s'\n\n", club.name);
            printf("Press the key (indicated in brackets) corresponding to one of the options presented below to continue.\n\n");

            printf("[1] View Information\n");
            printf("[2] View Members\n");
            printf("[3] Update Information\n");
            printf("[4] Delete\n");
            printf("[Backspace] Return\n");

            choice = _getch();
            switch (choice)
            {
            case '1':
                view_club_info(club);
                break;

            case '2':
                view_club_members(club);
                break;

            case '3':
                update_club(club, club_pos);
                break;

            case '4':
                delete_club(club, club_pos);
                break;
            }
        }
    }
}

void register_club()
{
    bool is_authorized = prompt_authorization("Registering a new club", admin_password, ADMIN, "the main menu");
    if (!is_authorized)
    {
        return;
    }

    print_greeting();
    if (student_count == 0)
    {
        printf("Each club must have at least one student representative and there are no registered students.");
        return prompt_return_to_main_menu();
    }
    struct Club club;

    while (true)
    {
        print_greeting();
        printf("Name: ");
        fgets(club.name, sizeof(club.name), stdin);
        club.name[strlen(club.name) - 1] = '\0';
        if (strlen(club.name) > 0)
        {
            break;
        }
    }

    print_greeting();
    printf("Name: %s\n", club.name);

    strcpy(club.weekly_meeting_day, "");
    printf("Weekly Meeting Day: Choose a number corresponding to the desired weekday from the list presented below.\n");
    printf("[1] Monday\n");
    printf("[2] Tuesday\n");
    printf("[3] Wednesday\n");
    printf("[4] Thursday\n");
    printf("[5] Friday\n");
    char weekly_meeting_day = '\0';
    while (strcmp(club.weekly_meeting_day, "") == 0)
    {
        weekly_meeting_day = _getch();
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

    char student_reps[1024] = "";
    club.student_rep_count = 0;
    while (true)
    {
        print_greeting();
        printf("Name: %s\n", club.name);
        printf("Weekly Meeting Day: %s\n", club.weekly_meeting_day);
        printf("Student Representatives: ");

        char *student_rep_ids = accept_variable_length_input();
        if (strcmp(student_rep_ids, "") != 0)
        {
            char *student_rep_id = strtok(student_rep_ids, ", ");
            while (student_rep_id != NULL && club.student_rep_count < MAX_STUDENT_REPS_PER_CLUB)
            {
                for (int i = 0; i < student_count; i++)
                {
                    struct Student student = students[i];
                    if (student.id == atoi(student_rep_id))
                    {
                        club.student_rep_count++;
                        club.student_rep_ids[club.student_rep_count - 1] = student.id;

                        if (strcmp(student_reps, "") != 0)
                        {
                            strcat(student_reps, ", ");
                        }

                        strcat(student_reps, student.name);
                        strcat(student_reps, " [");
                        char stringified_student_id[5] = "";
                        snprintf(stringified_student_id, sizeof(stringified_student_id), "%d", student.id);
                        strcat(student_reps, stringified_student_id);
                        strcat(student_reps, "]");
                    }
                }

                student_rep_id = strtok(NULL, ", ");
            }

            if (club.student_rep_count > 0)
            {
                break;
            }
        }
        free(student_rep_ids);
    }

    while (true)
    {
        print_greeting();
        printf("Name: %s\n", club.name);
        printf("Weekly Meeting Day: %s\n", club.weekly_meeting_day);
        printf("Student Representatives: %s\n", student_reps);

        printf("Password: ");
        char *password = accept_variable_length_input();
        if (strlen(password) < 8)
        {
            free(password);
            password = NULL;
            continue;
        }

        print_greeting();
        printf("Name: %s\n", club.name);
        printf("Weekly Meeting Day: %s\n", club.weekly_meeting_day);
        printf("Student Representatives: %s\n", student_reps);

        printf("Password: ");
        for (int i = 0; i < strlen(password); i++)
        {
            printf("*");
        }
        printf("\n");
        printf("Confirm Password: ");
        char *password_confirmation = accept_variable_length_input();
        if (strcmp(password, password_confirmation) == 0)
        {
            club.password_length = strlen(password) + 1;
            club.password = malloc(club.password_length * sizeof(char));
            strcpy(club.password, password);
            free(password);
            free(password_confirmation);
            password = NULL;
            password_confirmation = NULL;
            break;
        }
        free(password);
        free(password_confirmation);
        password = NULL;
        password_confirmation = NULL;
    }

    print_greeting();
    printf("Confirm registration of club '%s'? (y/n) ", club.name);
    char confirmation = '\0';
    scanf("%c", &confirmation);
    printf("\n");
    if (tolower(confirmation) != 'y')
    {
        clear_console();
        printf("Registration of club '%s' cancelled. ", club.name);
        return prompt_return_to_main_menu();
    }

    club.id = ++last_club_id;
    club.registered_at = time(NULL);
    clubs[club_count] = club;
    club_count++;
    for (int i = 0; i < club.student_rep_count; i++)
    {
        struct Student student_rep = students[club.student_rep_ids[i]];
        club.member_ids[i] = student_rep.id;
        for (int j = 0; j < sizeof(student_rep.club_memberships); j++)
        {
            if (student_rep.club_memberships[j] == 0)
            {
                students[i].club_memberships[j] = club.id;
                break;
            }
        }
    }
    club.member_count = club.student_rep_count;
    save_data_to_file();

    print_greeting();
    printf("New club successfully registered.\n\n");
    printf("> Name: %s\n", club.name);
    printf("> ID: %d\n", club.id);
    printf("> Registered At: %s", ctime(&club.registered_at));
    printf("> Student Representatives: %s\n", student_reps);
    printf("> Member Count: %d\n", club.member_count);
    printf("> Weekly Meeting Day: %s\n\n", club.weekly_meeting_day);

    prompt_return_to_main_menu();
}

void view_club_info(struct Club club)
{
    print_greeting();

    printf("VIEW CLUB INFORMATION\n\n");
    printf("> Name: %s\n", club.name);
    printf("> ID: %d\n", club.id);
    printf("> Registered At: %s", ctime(&club.registered_at));
    printf("> Member Count: %d\n", club.member_count);
    printf("> Weekly Meeting Day: %s\n", club.weekly_meeting_day);
    printf("> Student Representatives: ");

    for (int i = 0; i < student_count; i++)
    {
        struct Student student = students[i];
        for (int j = 0; j < club.student_rep_count; j++)
        {
            if (club.student_rep_ids[j] == student.id)
            {
                printf("%s", student.name);
                if (j < club.student_rep_count - 1)
                {
                    printf(", ");
                }
                else
                {
                    break;
                }
            }
        }
    }

    printf("\n\n");
    printf("Press [Backspace] at any time to return to the club menu.");
    char user_input = 0;
    while (user_input != BACKSPACE_ASCII_CODE)
    {
        user_input = _getch();
    };
}

void view_club_members(struct Club club)
{
    print_greeting();

    printf("CLUB MEMBER LISTING\n\n");

    for (int i = 0; i < student_count; i++)
    {
        struct Student student = students[i];
        for (int j = 0; j < club.member_count; j++)
        {
            if (student.id == club.member_ids[j])
            {
                printf("[%d] %s\n", student.id, student.name);
                break;
            }
        }
    }

    printf("\n");
    printf("Press [Backspace] at any time to return to the club menu.");
    char user_input = '\0';
    while (user_input != BACKSPACE_ASCII_CODE)
    {
        user_input = _getch();
    };
}

void update_club(struct Club club, int club_pos)
{
    while (true)
    {
        print_greeting();
        printf("UPDATE CLUB INFORMATION\n\n");
        printf("Press the key (indicated in brackets) corresponding to one of the options presented below to continue.\n");
        printf("[1] Name\n");
        printf("[2] Weekly Meeting Day\n");
        printf("[3] Student Representatives\n");
        printf("[Backspace] Return");

        char choice = _getch();
        if (choice == BACKSPACE_ASCII_CODE)
        {
            break;
        }
        else
        {
            while (true)
            {
                switch (choice)
                {
                case '1':
                    print_greeting();

                    printf("After typing the new name for '%s', press [Enter] to confirm your choice.\n\n", club.name);
                    char new_name[255] = "";
                    while (new_name[0] == '\0')
                    {
                        fgets(new_name, sizeof(new_name), stdin);
                        new_name[strlen(new_name) - 1] = '\0';
                    }
                    char old_name[255];
                    strcpy(old_name, club.name);
                    strcpy(clubs[club_pos - 1].name, new_name);
                    save_data_to_file();
                    print_greeting();
                    printf("Successfully updated the name of '%s' (previously '%s'). ", new_name, old_name);
                    break;

                case '2':
                    print_greeting();

                    printf("Choose a number corresponding to the desired weekday from the list presented below to update the weekly meeting day of '%s'.\n", club.name);
                    printf("[1] Monday\n");
                    printf("[2] Tuesday\n");
                    printf("[3] Wednesday\n");
                    printf("[4] Thursday\n");
                    printf("[5] Friday\n");
                    char old_weekly_meeting_day[9] = "";
                    strcpy(old_weekly_meeting_day, club.weekly_meeting_day);
                    char new_weekly_meeting_day = '\0';
                    while (new_weekly_meeting_day == '\0')
                    {
                        new_weekly_meeting_day = _getch();
                        switch (new_weekly_meeting_day)
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
                    strcpy(clubs[club_pos - 1].weekly_meeting_day, club.weekly_meeting_day);
                    save_data_to_file();
                    print_greeting();
                    printf("Successfully updated the weekly meeting day of '%s' from %s to %s. ", club.name, old_weekly_meeting_day, club.weekly_meeting_day);
                    break;

                case '3':
                    char prev_student_reps[2048] = "";
                    for (int i = 0; i < student_count; i++)
                    {
                        struct Student student = students[i];
                        for (int j = 0; j < club.student_rep_count; j++)
                        {
                            if (student.id == club.student_rep_ids[j])
                            {
                                if (strcmp(prev_student_reps, "") != 0)
                                {
                                    strcat(prev_student_reps, ", ");
                                }

                                strcat(prev_student_reps, student.name);
                                strcat(prev_student_reps, " [");
                                char stringified_student_id[5] = "";
                                snprintf(stringified_student_id, sizeof(stringified_student_id), "%d", student.id);
                                strcat(prev_student_reps, stringified_student_id);
                                strcat(prev_student_reps, "]");

                                break;
                            }
                        }
                    }

                    char new_student_reps[2048] = "";
                    for (int i = 0; i < club.student_rep_count; i++)
                    {
                        club.student_rep_ids[i] = 0;
                    }
                    club.student_rep_count = 0;
                    while (true)
                    {
                        print_greeting();
                        printf("Enter a comma-separated list of the IDs of the new student representatives for '%s' below. To confirm your choice, press [Enter].\n\n", club.name);

                        char *student_rep_ids = accept_variable_length_input();
                        if (strcmp(student_rep_ids, "") != 0)
                        {
                            char *student_rep_id = strtok(student_rep_ids, ", ");
                            while (student_rep_id != NULL && club.student_rep_count < MAX_STUDENT_REPS_PER_CLUB)
                            {
                                for (int i = 0; i < student_count; i++)
                                {
                                    struct Student student = students[i];
                                    if (student.id == atoi(student_rep_id))
                                    {
                                        club.student_rep_count++;
                                        clubs[club_pos - 1].student_rep_ids[club.student_rep_count - 1] = student.id;

                                        bool is_member = false;
                                        for (int j = 0; j < club.member_count; j++)
                                        {
                                            if (club.member_ids[j] == student.id)
                                            {
                                                is_member = true;
                                            }
                                        }
                                        if (!is_member)
                                        {
                                            clubs[club_pos - 1].member_ids[club.member_count - 1] = student.id;
                                            for (int j = 0; j < sizeof(student.club_memberships); j++)
                                            {
                                                if (student.club_memberships[j] == 0)
                                                {
                                                    students[i].club_memberships[j] = club.id;
                                                    break;
                                                }
                                            }
                                        }

                                        if (strcmp(new_student_reps, "") != 0)
                                        {
                                            strcat(new_student_reps, ", ");
                                        }
                                        strcat(new_student_reps, student.name);
                                        strcat(new_student_reps, " [");
                                        char stringified_student_id[5] = "";
                                        snprintf(stringified_student_id, sizeof(stringified_student_id), "%d", student.id);
                                        strcat(new_student_reps, stringified_student_id);
                                        strcat(new_student_reps, "]");
                                    }
                                }

                                student_rep_id = strtok(NULL, ", ");
                            }

                            if (club.student_rep_count > 0)
                            {
                                break;
                            }
                        }
                        free(student_rep_ids);
                    }

                    clubs[club_pos - 1].student_rep_count = club.student_rep_count;
                    save_data_to_file();
                    print_greeting();
                    printf("The student representatives of '%s' have been updated to %s (previously %s). ", club.name, new_student_reps, prev_student_reps);
                    break;
                }

                if (choice == '1' || choice == '2' || choice == '3')
                {
                    printf("Press [Backspace] at any time to return to the previous menu.");
                    char user_input = 0;
                    while (user_input != BACKSPACE_ASCII_CODE)
                    {
                        user_input = _getch();
                    };
                    break;
                }
            }
        }
    }
}

void delete_club(struct Club club, int club_pos)
{
    clear_console();
    printf("Please confirm whether you would like to permanently delete '%s'. (y/n)\n", club.name);
    printf("Warning: This is an irreversible action. All data associated with '%s' will be irrecoverably lost.\n", club.name);
    char confirmation = _getch();
    if (tolower(confirmation) != 'y')
    {
        printf("Deletion of club '%s' cancelled.");
    }
    else
    {
        if (club_count > club_pos)
        {
            for (int i = club_pos - 1; i < club_count - 1; i++)
            {
                clubs[i] = clubs[i + 1];
            }
        }
        struct Club placeholder_club = {0};
        clubs[club_count - 1] = placeholder_club;
        club_count--;
        save_data_to_file();

        printf("Successfully deleted club '%s'.", club.name);
    }

    printf(" Press [Backspace] at any time to return to the previous menu.");
    char user_input = 0;
    while (user_input != BACKSPACE_ASCII_CODE)
    {
        user_input = _getch();
    };
}
