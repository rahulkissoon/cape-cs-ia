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

int prev_club_id = 0;

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

        struct Club club = {0};
        int club_pos = -1;
        while (club.id == 0)
        {
            char club_lookup_query[5];
            fgets(club_lookup_query, sizeof(club_lookup_query) + sizeof(char), stdin);
            if (strcmp(club_lookup_query, "0\n") == 0)
            {
                break;
            }

            for (int i = 0; i < club_count; i++)
            {
                if (clubs[i].id == atoi(club_lookup_query))
                {
                    club = clubs[i];
                    club_pos = i;
                    break;
                }

                for (int j = 0; j < sizeof(club_lookup_query) / sizeof(char); j++)
                {
                    club_lookup_query[j] = tolower(club_lookup_query[j]);
                }
                char club_name[MAX_CLUB_NAME_LENGTH];
                strcpy(club_name, clubs[i].name);
                for (int j = 0; j < sizeof(club_name) / sizeof(char); j++)
                {
                    club_name[j] = tolower(club_name[j]);
                }
                if (strcmp(club_name, club_lookup_query) == 0)
                {
                    club = clubs[i];
                    club_pos = i;
                    break;
                }
            }
        }
        if (club_pos == -1)
        {
            break;
        }

        bool is_authorized = prompt_authorization("Managing a club", club.password, STANDARD, "the main menu");
        if (!is_authorized)
        {
            return;
        }

        char choice = '\0';
        while (choice != BACKSPACE_ASCII_CODE)
        {
            club = clubs[club_pos];
            if (club.id == 0)
            {
                break;
            }

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
        printf("Each club must have at least one student representative and there are 0 registered students. ");
        return prompt_return_to_main_menu();
    }
    struct Club club = {0};

    while (true)
    {
        print_greeting();
        printf("> Name: ");
        fgets(club.name, sizeof(club.name) + sizeof(char), stdin);
        club.name[strlen(club.name) - 1] = '\0';
        if (strlen(club.name) > 0)
        {
            break;
        }
    }

    print_greeting();
    printf("> Name: %s\n", club.name);

    strcpy(club.weekly_meeting_day, "");
    printf("> Weekly Meeting Day: Choose a number corresponding to the desired weekday from the list presented below.\n");
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

    while (true)
    {
        print_greeting();
        printf("> Name: %s\n", club.name);
        printf("> Weekly Meeting Day: %s\n", club.weekly_meeting_day);
        printf("> Description: ");
        fgets(club.description, sizeof(club.description) + sizeof(char), stdin);
        club.description[strlen(club.description) - 1] = '\0';
        if (strlen(club.description) > 0)
        {
            break;
        }
    }

    char student_reps[1024] = "";
    club.student_rep_count = 0;
    while (true)
    {
        print_greeting();
        printf("> Name: %s\n", club.name);
        printf("> Weekly Meeting Day: %s\n", club.weekly_meeting_day);
        printf("> Description: %s\n", club.description);
        printf("> Student Representatives: ");

        char *student_rep_ids = accept_variable_length_input();
        if (strcmp(student_rep_ids, "") != 0)
        {
            char *student_rep_id = strtok(student_rep_ids, ", ");
            while (student_rep_id != NULL && club.student_rep_count < MAX_STUDENT_REPS_PER_CLUB)
            {
                bool is_duplicate = false;
                for (int i = 0; i < club.student_rep_count; i++)
                {
                    if (atoi(student_rep_id) == club.student_rep_ids[i])
                    {
                        is_duplicate = true;
                        break;
                    }
                }
                if (!is_duplicate)
                {
                    for (int i = 0; i < student_count; i++)
                    {
                        struct Student student = students[i];
                        if (student.id == atoi(student_rep_id) && atoi(student_rep_id) != 0)
                        {
                            club.student_rep_ids[club.student_rep_count++] = student.id;

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
        printf("> Name: %s\n", club.name);
        printf("> Weekly Meeting Day: %s\n", club.weekly_meeting_day);
        printf("> Student Representatives: %s\n", student_reps);

        printf("> Password: ");
        char *password = accept_variable_length_input();
        if (strlen(password) < 8)
        {
            free(password);
            password = NULL;
            continue;
        }

        print_greeting();
        printf("> Name: %s\n", club.name);
        printf("> Weekly Meeting Day: %s\n", club.weekly_meeting_day);
        printf("> Student Representatives: %s\n", student_reps);

        printf("> Password: ");
        for (int i = 0; i < strlen(password); i++)
        {
            printf("*");
        }
        printf("\n");
        printf("> Confirm Password: ");
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

    club.id = ++prev_club_id;
    club.registered_at = time(NULL);
    for (int i = 0; i < club.student_rep_count; i++)
    {
        for (int j = 0; j < student_count; j++)
        {
            if (students[j].id == club.student_rep_ids[i])
            {
                club.member_ids[club.member_count++] = club.student_rep_ids[i];
                struct Student student = students[j];
                for (int k = 0; k < sizeof(student.club_memberships) / sizeof(int); k++)
                {
                    if (student.club_memberships[k] == 0)
                    {
                        students[j].club_memberships[k] = club.id;
                        break;
                    }
                }
            }
            break;
        }
    }
    clubs[club_count++] = club;
    save_data_to_file();

    print_greeting();
    printf("New club successfully registered.\n\n");
    printf("> Name: %s\n", club.name);
    printf("> ID: %d\n", club.id);
    printf("> Description: %s\n", club.description);
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
    printf("> Description: %s\n", club.description);
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
                printf("%s [%d]", student.name, student.id);
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
        printf("[3] Description\n");
        printf("[4] Student Representatives\n");
        printf("[5] Password\n");
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

                    printf("After typing the new name for '%s', press [Enter] to confirm.\n\n", club.name);
                    char new_name[MAX_CLUB_NAME_LENGTH] = "";
                    while (strcmp(new_name, ""))
                    {
                        fgets(new_name, sizeof(new_name) + sizeof(char), stdin);
                        new_name[strlen(new_name) - 1] = '\0';
                    }
                    char old_name[MAX_CLUB_NAME_LENGTH];
                    strcpy(old_name, club.name);
                    strcpy(clubs[club_pos].name, new_name);
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
                    char old_weekly_meeting_day[9];
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
                    strcpy(clubs[club_pos].weekly_meeting_day, club.weekly_meeting_day);
                    save_data_to_file();
                    print_greeting();
                    printf("Successfully updated the weekly meeting day of '%s' from %s to %s. ", club.name, old_weekly_meeting_day, club.weekly_meeting_day);
                    break;

                case '3':
                    print_greeting();

                    printf("After typing the new description for '%s', press [Enter] to confirm.\n\n", club.name);
                    char new_description[MAX_CLUB_DESCRIPTION_LENGTH] = "";
                    while (strcmp(new_description, "") == 0)
                    {
                        fgets(new_description, sizeof(new_description) + sizeof(char), stdin);
                        new_description[strlen(new_description) - 1] = '\0';
                    }
                    strcpy(clubs[club_pos].description, new_description);
                    save_data_to_file();
                    print_greeting();
                    printf("Successfully updated the description of '%s'. ", club.name);
                    break;

                case '4':
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
                                bool is_duplicate = false;
                                for (int i = 0; i < club.student_rep_count; i++)
                                {
                                    if (atoi(student_rep_id) == club.student_rep_ids[i])
                                    {
                                        is_duplicate = true;
                                        break;
                                    }
                                }
                                if (!is_duplicate)
                                {
                                    for (int i = 0; i < student_count; i++)
                                    {
                                        struct Student student = students[i];

                                        if (student.id == atoi(student_rep_id))
                                        {
                                            club.student_rep_ids[club.student_rep_count++] = student.id;

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
                                                clubs[club_pos].member_ids[club.member_count++] = student.id;
                                                for (int j = 0; j < sizeof(student.club_memberships) / sizeof(int); j++)
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
                            }

                            if (club.student_rep_count > 0)
                            {
                                break;
                            }
                        }
                        free(student_rep_ids);
                    }

                    for (int i = 0; i < club.student_rep_count; i++)
                    {
                        clubs[club_pos].student_rep_ids[i] = club.student_rep_ids[i];
                    }
                    clubs[club_pos].member_count = club.member_count;
                    clubs[club_pos].student_rep_count = club.student_rep_count;
                    save_data_to_file();
                    print_greeting();
                    printf("The student representatives of '%s' have been updated to %s (previously %s). ", club.name, new_student_reps, prev_student_reps);
                    break;

                case '5':
                    while (true)
                    {
                        print_greeting();
                        printf("Enter the new password for club '%s' below. It must contain at least 8 characters.\n\n", club.name);
                        printf("Password: ");
                        char *password = accept_variable_length_input();
                        if (strlen(password) < 8)
                        {
                            free(password);
                            password = NULL;
                            continue;
                        }

                        print_greeting();
                        printf("Enter the new password for club '%s' below. It must contain at least 8 characters.\n\n", club.name);
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
                            clubs[club_pos].password_length = strlen(password) + 1;
                            clubs[club_pos].password = malloc(club.password_length * sizeof(char));
                            strcpy(clubs[club_pos].password, password);
                            save_data_to_file();
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
                    printf("The password of club '%s' has been successfully updated. ", club.name);
                    break;
                }

                if (choice == '1' || choice == '2' || choice == '3' || choice == '4' || choice == '5')
                {
                    printf("Press [Backspace] at any time to return to the club menu.");
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
    print_greeting();
    printf("Please confirm whether you would like to permanently delete club '%s'. (y/n)\n", club.name);
    printf("Warning: This is an irreversible action. All data associated with club '%s' will be irrecoverably lost.\n", club.name);
    char confirmation = _getch();
    if (tolower(confirmation) != 'y')
    {
        print_greeting();
        printf("Deletion of club '%s' cancelled.", club.name);
    }
    else
    {
        for (int i = 0; i < club.member_count; i++)
        {
            for (int j = 0; j < student_count; j++)
            {
                struct Student student = students[j];
                if (student.id == 0)
                {
                    break;
                }
                if (student.id == club.member_ids[i])
                {
                    for (int k = 0; k < sizeof(student.club_memberships) / sizeof(int); k++)
                    {
                        if (student.club_memberships[k] == club.id)
                        {

                            for (int l = k; l < sizeof(student.club_memberships) / sizeof(int) - 1; l++)
                            {
                                students[j].club_memberships[l] = students[j].club_memberships[l + 1];
                            }
                            students[j].club_memberships[sizeof(student.club_memberships) / sizeof(int) - 1] = 0;
                        }
                        break;
                    }
                }
            }
        }
        if (club_count > club_pos + 1)
        {
            for (int i = club_pos; i < club_count - 1; i++)
            {
                clubs[i] = clubs[i + 1];
            }
        }
        struct Club placeholder_club = {0};
        clubs[--club_count] = placeholder_club;
        save_data_to_file();
        print_greeting();
        printf("Successfully deleted club '%s'.", club.name);

        printf(" Press [Backspace] at any time to return to the previous menu.");
        char user_input = 0;
        while (user_input != BACKSPACE_ASCII_CODE)
        {
            user_input = _getch();
        };
    }
}