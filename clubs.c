#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "accounting.h"
#include "auth.h"
#include "menu_system.h"
#include "clubs.h"
#include "core.h"
#include "students.h"
#include "util.h"

struct Club clubs[MAX_CLUBS] = {{0}};
struct Meeting *meetings;

int club_count = 0;
int prev_club_id = 0;
int meeting_count = 0;

void manage_clubs()
{
    while (true)
    {
        if (club_count == 0)
        {
            print_greeting();
            printf("No clubs have yet been registered. ");
            return prompt_return("the main menu");
        }

        struct Club club = {0};
        int club_pos = -1;
        while (club.id == 0)
        {
            print_greeting();
            printf("MANAGE CLUBS\n\n");
            printf("Type in the number indicated in brackets preceding the name and then press [Enter] to select the club. Otherwise, leave the field blank and press [Enter] to return to the main menu.\n\n");
            for (int i = 0; i < club_count; i++)
            {
                struct Club club = clubs[i];
                printf("[%d] %s\n", club.id, club.name);
            }
            printf("\n");

            char club_lookup_query[5];
            fgets(club_lookup_query, sizeof(club_lookup_query) + sizeof(char), stdin);
            club_lookup_query[strlen(club_lookup_query) - 1] = '\0';
            if (strcmp(club_lookup_query, "") == 0)
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
            continue;
        }

        char choice = '\0';
        while (choice != 'r')
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
            printf("[2] Manage Members\n");
            printf("[3] Update Information\n");
            printf("[4] Post Meeting\n");
            printf("[5] List Club Meetings\n");
            printf("[6] View Accounting Ledger\n");
            printf("[7] Delete Club\n");
            printf("[R] Return\n");

            choice = _getch();
            switch (choice)
            {
            case '1':
                view_club_info(club);
                break;

            case '2':
                manage_club_members(club_pos);
                break;

            case '3':
                update_club_info(club, club_pos);
                break;

            case '4':
                post_meeting(club, club_pos);
                break;

            case '5':
                list_club_meetings(club);
                break;

            case '6':
                view_club_ledger(club_pos);
                break;

            case '7':
                delete_club(club, club_pos);
                break;
            }
        }
    }
}

void register_club()
{
    print_greeting();
    printf("REGISTER NEW CLUB\n\n");

    if (club_count == MAX_CLUBS)
    {
        printf("A maximum of %d clubs has already been registered. ", MAX_CLUBS);
        return prompt_return("the main menu");
    }

    if (student_count == 0)
    {
        printf("Each club must have at least one student representative and there are 0 registered students. ");
        return prompt_return("the main menu");
    }

    struct Club club = {0};
    while (true)
    {
        print_greeting();
        printf("REGISTER NEW CLUB\n\n");
        printf("> Name: ");
        fgets(club.name, sizeof(club.name) + sizeof(char), stdin);
        club.name[strlen(club.name) - 1] = '\0';
        if (strlen(club.name) > 0)
        {
            break;
        }
    }

    print_greeting();
    printf("REGISTER NEW CLUB\n\n");
    printf("> Name: %s\n", club.name);

    strcpy(club.weekly_meeting_day, "");
    printf("> Weekly Meeting Day: Choose a number corresponding to the desired weekday from the options presented below.\n");
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
        printf("REGISTER NEW CLUB\n\n");
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
        printf("REGISTER NEW CLUB\n\n");
        printf("> Name: %s\n", club.name);
        printf("> Weekly Meeting Day: %s\n", club.weekly_meeting_day);
        printf("> Description: %s\n", club.description);
        printf("> Student Representatives' IDs: ");

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
        printf("REGISTER NEW CLUB\n\n");
        printf("> Name: %s\n", club.name);
        printf("> Weekly Meeting Day: %s\n", club.weekly_meeting_day);
        printf("> Description: %s\n", club.description);
        printf("> Student Representatives: %s\n", student_reps);

        printf("> Password (min %d characters): ", MIN_PASSWORD_LENGTH);
        char *password = accept_variable_length_input();
        if (strlen(password) < MIN_PASSWORD_LENGTH)
        {
            free(password);
            password = NULL;
            continue;
        }

        print_greeting();
        printf("REGISTER NEW CLUB\n\n");
        printf("> Name: %s\n", club.name);
        printf("> Weekly Meeting Day: %s\n", club.weekly_meeting_day);
        printf("> Description: %s\n", club.description);
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
    printf("REGISTER NEW CLUB\n\n");
    printf("> Name: %s\n", club.name);
    printf("> Weekly Meeting Day: %s\n", club.weekly_meeting_day);
    printf("> Description: %s\n", club.description);
    printf("> Student Representatives: %s\n", student_reps);
    printf("> Password: ");
    for (int i = 0; i < club.password_length; i++)
    {
        printf("*");
    }
    printf("\n\n");

    printf("Confirm registration of club? (y/n)", club.name);
    char confirmation = _getch();
    printf("\n");
    if (confirmation != 'y')
    {
        print_greeting();
        printf("Registration of club '%s' cancelled. ", club.name);
        return prompt_return("the main menu");
    }

    club.id = ++prev_club_id;
    club.registered_at = time(NULL);

    for (int i = 0; i < student_count; i++)
    {
        for (int j = 0; j < club.student_rep_count; j++)
        {
            if (students[i].id == club.student_rep_ids[j])
            {
                club.member_ids[club.member_count++] = club.student_rep_ids[j];
                struct Student student = students[i];
                for (int k = 0; k < sizeof(student.club_memberships) / sizeof(int); k++)
                {
                    if (student.club_memberships[k] == 0)
                    {
                        students[i].club_memberships[k] = club.id;
                        break;
                    }
                }
                break;
            }
        }
    }
    clubs[club_count++] = club;
    save_data_to_file();

    print_greeting();
    printf("New club successfully registered.\n\n");
    printf("> Name: %s\n", club.name);
    printf("> ID: %d\n", club.id);
    printf("> Description: %s\n", club.description);
    printf("> Registered At: %s\n", format_time_t(club.registered_at));
    printf("> Student Representatives: %s\n", student_reps);
    printf("> Member Count: %d\n", club.member_count);
    printf("> Weekly Meeting Day: %s\n\n", club.weekly_meeting_day);

    prompt_return("the main menu");
}

void view_club_info(struct Club club)
{
    print_greeting();

    printf("VIEW CLUB INFORMATION\n\n");
    printf("> Name: %s\n", club.name);
    printf("> ID: %d\n", club.id);
    printf("> Description: %s\n", club.description);
    printf("> Registered At: %s\n", format_time_t(club.registered_at));
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
                printf("[%d] %s", student.id, student.name);
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

    prompt_return("the club menu");
}

void manage_club_members(int club_pos)
{
    char choice = '\0';
    while (choice != 'r')
    {
        struct Club club = clubs[club_pos];
        print_greeting();
        printf("MANAGE MEMBERS OF CLUB '%s'\n\n", club.name);

        struct Student *club_members = malloc(club.member_count * sizeof(struct Student));
        int k = 0;
        for (int i = 0; i < student_count; i++)
        {
            for (int j = 0; j < MAX_CLUB_MEMBERSHIPS; j++)
            {
                if (students[i].club_memberships[j] == club.id)
                {
                    club_members[k] = students[i];
                    k++;
                    if (k == club.member_count)
                    {
                        break;
                    }
                }
            }
        }

        char ***rows = malloc(club.member_count * sizeof(char **));
        if (rows == NULL)
        {
            printf("Allocation of %lld bytes of memory failed", club.transaction_count * sizeof(char **));
            exit(1);
        }
        int total_columns = 4;
        char *column_headings[] = {
            "ID",
            "Name",
            "Class",
            "Email Address",
        };
        int column_widths[4] = {strlen(column_headings[0]),
                                strlen(column_headings[1]),
                                strlen(column_headings[2]),
                                strlen(column_headings[3])};

        for (int i = 0; i < club.member_count; i++)
        {
            struct Student student = club_members[i];

            int student_id_length = snprintf(NULL, 0, "%d", student.id);
            if (column_widths[0] < student_id_length)
            {
                column_widths[0] = student_id_length;
            }

            int student_name_length = strlen(student.name);
            if (column_widths[1] < student_name_length)
            {
                column_widths[1] = student_name_length;
            }

            int student_class_length = strlen(student.class);
            if (column_widths[2] < student_class_length)
            {
                column_widths[2] = student_class_length;
            }

            int student_email_address_length = strlen(student.email_address);
            if (column_widths[3] < student_email_address_length)
            {
                column_widths[3] = student_email_address_length;
            }
        }

        for (int i = 0; i < club.member_count; i++)
        {
            struct Student student = club_members[i];

            rows[i] = malloc(total_columns * sizeof(char *));
            if (rows[i] == NULL)
            {
                printf("Allocation of %lld bytes of memory failed", total_columns * sizeof(char *));
                exit(1);
            }

            for (int j = 0; j < total_columns; j++)
            {
                rows[i][j] = malloc((column_widths[j] + 1) * sizeof(char));
                if (rows[i][j] == NULL)
                {
                    printf("Allocation of %lld bytes of memory failed", (column_widths[j] + 1) * sizeof(char));
                    exit(1);
                }

                switch (j)
                {
                case 0:
                    int student_id_length = snprintf(NULL, 0, "%d", student.id);
                    char *stringified_student_id = malloc((student_id_length + 1) * sizeof(char));
                    if (stringified_student_id == NULL)
                    {
                        printf("Allocation of %lld bytes of memory failed", (student_id_length + 1) * sizeof(char));
                        exit(1);
                    }
                    snprintf(stringified_student_id, student_id_length + 1, "%d", student.id);
                    strcpy(rows[i][j], stringified_student_id);
                    free(stringified_student_id);
                    break;

                case 1:
                    strcpy(rows[i][j], student.name);
                    break;

                case 2:
                    strcpy(rows[i][j], student.class);
                    break;

                case 3:
                    strcpy(rows[i][j], student.email_address);
                    break;
                }
            }
        }

        int member_count_length = snprintf(NULL, 0, "%d", club.member_count);
        int total_footer_rows = 1;
        char **footer_rows = malloc(total_footer_rows * sizeof(char *));
        footer_rows[0] = malloc((strlen("Member Count: ") + member_count_length + 1) * sizeof(char));
        sprintf(footer_rows[0], "Member Count: %d", club.member_count);

        print_table(total_columns, club.member_count, column_widths, 1, column_headings, rows, footer_rows);

        for (int i = 0; i < total_footer_rows; i++)
        {
            free(footer_rows[i]);
        }
        free(footer_rows);
        for (int i = 0; i < club.transaction_count; i++)
        {
            for (int j = 0; j < total_columns; j++)
            {
                free(rows[i][j]);
            }
            free(rows[i]);
        }
        free(club_members);

        printf("\n\n");
        printf("Press [E] to enroll a new member or [U] to rescind a membership. Otherwise, press [R] to return to the club menu.");

        choice = _getch();
        switch (choice)
        {
        case 'e':
            enroll_new_member(club, club_pos);
            break;

        case 'u':
            rescind_membership(club, club_pos);
            break;
        }
    }
}

void enroll_new_member(struct Club club, int club_pos)
{
    struct Student student = {0};
    int student_pos = -1;

    while (student.id == 0)
    {
        print_greeting();
        printf("ENROLL NEW MEMBER TO CLUB '%s'\n\n", club.name);
        printf("Type in the full name or ID of the student you wish to enroll to the club '%s' and then press [Enter]. Otherwise, leave the field blank and press [Enter] to return to the club menu.\n\n", club.name);

        char student_lookup_query[MAX_STUDENT_NAME_LENGTH];
        fgets(student_lookup_query, sizeof(student_lookup_query) + sizeof(char), stdin);
        student_lookup_query[strlen(student_lookup_query) - 1] = '\0';
        if (strcmp(student_lookup_query, "") == 0)
        {
            break;
        }

        for (int i = 0; i < student_count; i++)
        {
            if (students[i].id == atoi(student_lookup_query))
            {
                student = students[i];
                student_pos = i;
                break;
            }

            for (int j = 0; j < sizeof(student_lookup_query) / sizeof(char); j++)
            {
                student_lookup_query[j] = tolower(student_lookup_query[j]);
            }
            char student_name[MAX_STUDENT_NAME_LENGTH];
            strcpy(student_name, students[i].name);
            for (int j = 0; j < sizeof(student_name) / sizeof(char); j++)
            {
                student_name[j] = tolower(student_name[j]);
            }
            if (strcmp(student_name, student_lookup_query) == 0)
            {
                student = students[i];
                student_pos = i;
                break;
            }
        }
    }
    if (student_pos == -1)
    {
        return;
    }

    bool is_member = false;
    for (int i = 0; i < club.member_count; i++)
    {
        if (club.member_ids[i] == student.id)
        {
            is_member = true;
            break;
        }
    }
    if (is_member)
    {
        print_greeting();
        printf("%s [%d] is already enrolled in the '%s' club. ", student.name, student.id, club.name);
        return prompt_return("the club menu");
    }

    if (student.club_memberships[MAX_CLUB_MEMBERSHIPS - 1] != 0)
    {
        print_greeting();
        printf("%s [%d] is already enrolled in a maximum %d clubs. ", student.name, student.id, MAX_CLUB_MEMBERSHIPS);
        return prompt_return("the club menu");
    }

    print_greeting();
    printf("Please confirm that you would like to enroll %s [%d] to the club '%s'. (y/n)", student.name, student.id, club.name);
    char confirmation = _getch();
    if (confirmation != 'y')
    {
        print_greeting();
        printf("Student enrollment cancelled. ");
        return prompt_return("the club menu");
    }

    clubs[club_pos].member_ids[club.member_count] = student.id;
    clubs[club_pos].member_count++;
    for (int i = 0; i < MAX_CLUB_MEMBERSHIPS; i++)
    {
        if (student.club_memberships[i] == 0)
        {
            students[student_pos].club_memberships[i] = club.id;
            break;
        }
    }
    save_data_to_file();
    print_greeting();
    printf("%s [%d] was successfully enrolled to the club '%s'. ", student.name, student.id, club.name);
    prompt_return("the club menu");
}

void rescind_membership(struct Club club, int club_pos)
{
    struct Student student = {0};
    int student_pos = -1;

    while (student.id == 0)
    {
        print_greeting();
        printf("RESCIND MEMBERSHIP FROM CLUB '%s'\n\n", club.name);
        printf("Type in the full name or ID of the student whose membership of '%s' you wish to rescind and then press [Enter]. Otherwise, leave the field blank and press [Enter] to return to the club menu.\n\n", club.name);

        char student_lookup_query[MAX_STUDENT_NAME_LENGTH];
        fgets(student_lookup_query, sizeof(student_lookup_query) + sizeof(char), stdin);
        student_lookup_query[strlen(student_lookup_query) - 1] = '\0';
        if (strcmp(student_lookup_query, "") == 0)
        {
            break;
        }

        for (int i = 0; i < student_count; i++)
        {
            if (students[i].id == atoi(student_lookup_query))
            {
                student = students[i];
                student_pos = i;
                break;
            }

            for (int j = 0; j < sizeof(student_lookup_query) / sizeof(char); j++)
            {
                student_lookup_query[j] = tolower(student_lookup_query[j]);
            }
            char student_name[MAX_STUDENT_NAME_LENGTH];
            strcpy(student_name, students[i].name);
            for (int j = 0; j < sizeof(student_name) / sizeof(char); j++)
            {
                student_name[j] = tolower(student_name[j]);
            }
            if (strcmp(student_name, student_lookup_query) == 0)
            {
                student = students[i];
                break;
            }
        }
    }
    if (student_pos == -1)
    {
        return;
    }

    bool is_member = false;
    for (int i = 0; i < club.member_count; i++)
    {
        if (club.member_ids[i] == student.id)
        {
            is_member = true;
            break;
        }
    }
    if (!is_member)
    {
        print_greeting();
        printf("%s [%d] is not a member of the '%s' club. ", student.name, student.id, club.name);
        return prompt_return("the club menu");
    }

    bool is_student_rep = false;
    for (int i = 0; i < club.student_rep_count; i++)
    {
        if (club.student_rep_ids[i] == student.id)
        {
            is_student_rep = true;
            break;
        }
    }
    if (is_student_rep)
    {
        print_greeting();
        printf("%s [%d] is a student representative of the '%s' club. This status must first be revoked before their membership may be rescinded. ", student.name, student.id, club.name);
        return prompt_return("the club menu");
    }

    print_greeting();
    printf("Please confirm that you would like to rescind the membership of %s [%d] from the club '%s'. (y/n)", student.name, student.id, club.name);
    char confirmation = _getch();
    if (confirmation != 'y')
    {
        print_greeting();
        printf("Membership rescindment cancelled. ");
        return prompt_return("the club menu");
    }

    for (int i = 0; i < club.member_count; i++)
    {
        if (club.member_ids[i] == student.id)
        {
            if (club.member_count > i + 1)
            {
                for (int j = i; j < club.member_count - 1; j++)
                {
                    clubs[club_pos].member_ids[j] = clubs[club_pos].member_ids[j + 1];
                }
            }
            break;
        }
    }
    clubs[club_pos].member_ids[club.member_count - 1] = 0;
    clubs[club_pos].member_count--;
    for (int i = 0; i < MAX_CLUB_MEMBERSHIPS; i++)
    {
        if (student.club_memberships[i] == club.id)
        {
            if (MAX_CLUB_MEMBERSHIPS > i + 1)
            {
                for (int j = i; j < MAX_CLUB_MEMBERSHIPS - 1; j++)
                {
                    students[student_pos].club_memberships[j] = students[student_pos].club_memberships[j + 1];
                }
            }
        }
    }
    students[student_pos].club_memberships[MAX_CLUB_MEMBERSHIPS - 1] = 0;
    save_data_to_file();
    print_greeting();
    printf("The membership of %s [%d] was successfully rescinded from the club '%s'. ", student.name, student.id, club.name);
    prompt_return("the club menu");
}

void update_club_info(struct Club club, int club_pos)
{
    char choice = '\0';
    while (choice != 'r')
    {
        print_greeting();
        printf("UPDATE CLUB INFORMATION\n\n");
        printf("Press the key (indicated in brackets) corresponding to one of the options presented below to continue.\n");
        printf("[1] Name\n");
        printf("[2] Weekly Meeting Day\n");
        printf("[3] Description\n");
        printf("[4] Student Representatives\n");
        printf("[5] Password\n");
        printf("[R] Return");

        choice = _getch();

        switch (choice)
        {
        case '1':
            print_greeting();

            printf("After typing the new name for '%s', press [Enter] to confirm.\n\n", club.name);
            char new_name[MAX_CLUB_NAME_LENGTH] = "";
            while (strcmp(new_name, "") == 0)
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
                printf("Enter the new password for club '%s' below. It must contain at least %d characters.\n\n", club.name, MIN_PASSWORD_LENGTH);
                printf("Password: ");
                char *password = accept_variable_length_input();
                if (strlen(password) < MIN_PASSWORD_LENGTH)
                {
                    free(password);
                    password = NULL;
                    continue;
                }

                print_greeting();
                printf("Enter the new password for club '%s' below. It must contain at least %d characters.\n\n", club.name, MIN_PASSWORD_LENGTH);
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
            prompt_return("the club menu");
            break;
        }
    }
}

void delete_club(struct Club club, int club_pos)
{
    print_greeting();
    printf("Please confirm whether you would like to permanently delete club '%s'. (y/n)\n", club.name);
    printf("Warning: This is an irreversible action. All data associated with club '%s' will be irrecoverably lost.\n", club.name);
    char confirmation = _getch();
    if (confirmation != 'y')
    {
        print_greeting();
        printf("Deletion of club '%s' cancelled.", club.name);
    }
    else
    {
        for (int i = 0; i < meeting_count; i++)
        {
            struct Meeting meeting = meetings[i];
            if (meeting.club_id == club.id)
            {
                for (int j = i; j < meeting_count - 1; j++)
                {
                    meetings[j] = meetings[j + 1];
                }
                meetings[--meeting_count] = (struct Meeting){0};
            }
        }
        meetings = realloc(meetings, (meeting_count == 0 ? 1 : meeting_count) * sizeof(struct Meeting));
        if (meetings == NULL)
        {
            printf("Reallocation of %lld bytes of memory failed", meeting_count * sizeof(struct Meeting));
            exit(1);
        }

        for (int i = 0; i < transaction_count; i++)
        {
            struct Transaction transaction = transactions[i];
            if (transaction.club_id == club.id)
            {
                for (int j = i; j < transaction_count - 1; j++)
                {
                    transactions[j] = transactions[j + 1];
                }
                struct Transaction placeholder_transaction = {0};
                transactions[--transaction_count] = placeholder_transaction;
            }
        }
        transactions = realloc(transactions, (transaction_count == 0 ? 1 : transaction_count) * sizeof(struct Transaction));
        if (transactions == NULL)
        {
            printf("Reallocation of %lld bytes of memory failed", transaction_count * sizeof(struct Transaction));
            exit(1);
        }

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
        clubs[--club_count] = (struct Club){0};
        save_data_to_file();
        print_greeting();
        printf("Successfully deleted club '%s'. ", club.name);
        prompt_return("the main menu");
    }
}

void post_meeting(struct Club club, int club_pos)
{
    print_greeting();

    struct Meeting meeting = {0};
    meeting.club_id = club.id;

    struct Meeting prev_meeting = {0};
    for (int i = meeting_count - 1; i >= 0; i++)
    {
        if (meetings[i].club_id == club.id)
        {
            prev_meeting = meetings[i];
            break;
        }
    }

    while (true)
    {
        print_greeting();
        printf("POST MEETING FOR CLUB '%s'\n\n", club.name);
        printf("> Meeting Topic (max %d characters): ", MAX_MEETING_TOPIC_LENGTH);
        fgets(meeting.topic, sizeof(meeting.topic) + sizeof(char), stdin);
        meeting.topic[strlen(meeting.topic) - 1] = '\0';
        if (strlen(meeting.topic) > 0)
        {
            break;
        }
    }

    while (true)
    {
        print_greeting();
        printf("POST MEETING FOR CLUB '%s'\n\n", club.name);
        printf("> Meeting Topic (max %d characters): %s\n", MAX_MEETING_TOPIC_LENGTH, meeting.topic);
        printf("> Convened At (DD/MM/YYYY HH:MM): ");

        char user_input[17];
        fgets(user_input, sizeof(user_input) + sizeof(char), stdin);
        user_input[strlen(user_input) - 1] = '\0';

        struct tm tm;
        sscanf(user_input, "%d/%d/%d %d:%d", &tm.tm_mday, &tm.tm_mon, &tm.tm_year, &tm.tm_hour, &tm.tm_min);
        tm.tm_year -= 1900;
        tm.tm_mon -= 1;
        meeting.convened_at = mktime(&tm);

        if (meeting.convened_at == (time_t)-1 || meeting.convened_at > time(NULL) || (prev_meeting.id > 0 && meeting.convened_at <= prev_meeting.convened_at))
        {
            continue;
        }
        else
        {
            break;
        }
    }

    while (true)
    {
        print_greeting();
        printf("POST MEETING FOR CLUB '%s'\n\n", club.name);
        printf("> Meeting Topic (max %d characters): %s\n", MAX_MEETING_TOPIC_LENGTH, meeting.topic);
        printf("> Convened At (DD/MM/YYYY HH:MM): %s\n", format_time_t(meeting.convened_at));
        printf("> Adjourned At (DD/MM/YYYY HH:MM): ");

        char user_input[17];
        fgets(user_input, sizeof(user_input) + sizeof(char), stdin);
        user_input[strlen(user_input) - 1] = '\0';

        struct tm tm;
        sscanf(user_input, "%d/%d/%d %d:%d", &tm.tm_mday, &tm.tm_mon, &tm.tm_year, &tm.tm_hour, &tm.tm_min);
        tm.tm_year -= 1900;
        tm.tm_mon -= 1;
        meeting.adjourned_at = mktime(&tm);

        if (meeting.adjourned_at == (time_t)-1 || meeting.adjourned_at > time(NULL) || meeting.adjourned_at <= meeting.convened_at)
        {
            continue;
        }
        else
        {
            break;
        }
    }

    meeting.absent_member_count = club.member_count;
    for (int i = 0; i < club.member_count; i++)
    {
        if (club.member_ids[i] == 0)
        {
            break;
        }
        meeting.absent_member_ids[i] = club.member_ids[i];
    }

    meeting = edit_attendance_sheet(club, meeting);
    meeting.id = ++clubs[club_pos].prev_meeting_id;
    clubs[club_pos].meeting_count++;
    meetings = realloc(meetings, (meeting_count + 1) * sizeof(struct Meeting));
    if (meetings == NULL)
    {
        printf("Reallocation of %lld bytes of memory failed", (meeting_count + 1) * sizeof(struct Meeting));
        exit(1);
    }
    meetings[meeting_count++] = meeting;
    save_data_to_file();

    print_greeting();
    printf("Meeting successfully posted for club '%s'.\n\n", club.name);
    printf("> ID: %d\n", meeting.id);
    printf("> Convened At: %s\n", format_time_t(meeting.convened_at));
    printf("> Topic: %s\n", meeting.topic);
    printf("> Adjourned At: %s\n", format_time_t(meeting.adjourned_at));
    printf("> Attendance: %d/%d P, %d/%d A\n\n", meeting.present_member_count, club.member_count, meeting.absent_member_count, club.member_count);
    prompt_return("the club menu");
}

struct Meeting edit_attendance_sheet(struct Club club, struct Meeting meeting)
{
    int attendee_count = meeting.present_member_count + meeting.absent_member_count;
    if (attendee_count == 0)
    {
        print_greeting();
        printf("The students who attended this meeting no longer exist in the system. The attendance sheet is blank. ");
        prompt_return("the meeting summary");
        return meeting;
    }

    int *attendee_ids = malloc(attendee_count * sizeof(int));
    if (attendee_ids == NULL)
    {
        printf("Allocation of %lld bytes of memory failed", attendee_count * sizeof(int));
        exit(1);
    }
    for (int i = 0; i < meeting.absent_member_count; i++)
    {
        attendee_ids[i] = meeting.absent_member_ids[i];
    }
    for (int i = 0; i < meeting.present_member_count; i++)
    {
        attendee_ids[meeting.absent_member_count + i] = meeting.present_member_ids[i];
    }

    int cursor_position = 0;
    char choice = '\0';
    while (choice != 'r')
    {
        print_greeting();
        printf("EDIT MEETING ATTENDANCE SHEET FOR CLUB '%s'\n\n", club.name);
        printf("- The \">\" symbol indicates which member is currently selected. Press the [W] and [S] keys to amend your current selection.\n");
        printf("- All members are initially marked as absent. Press [P] and [A] to mark the currently selected member as present and absent respectively.\n");
        printf("- Press [R] to confirm and save the attendance sheet.\n\n");

        struct Student selected_member;
        bool selected_member_is_present = false;

        for (int i = 0; i < attendee_count; i++)
        {
            struct Student member = {0};
            bool is_present = false;
            for (int j = 0; j < student_count; j++)
            {
                if (students[j].id == attendee_ids[i])
                {
                    member = students[j];
                    for (int k = 0; k < meeting.present_member_count; k++)
                    {
                        if (meeting.present_member_ids[k] == member.id)
                        {
                            is_present = true;
                            break;
                        }
                    }
                    break;
                }
            }
            if (i == cursor_position)
            {
                selected_member = member;
                selected_member_is_present = is_present;
                printf("> ");
            }
            else
            {
                printf("  ");
            }
            printf("[%d] %s: %s\n", member.id, member.name, is_present ? "P" : "A");
        }

        choice = _getch();
        switch (choice)
        {
        case 's':
            if (cursor_position < attendee_count - 1)
            {
                cursor_position++;
            }
            break;

        case 'w':
            if (cursor_position > 0)
            {
                cursor_position--;
            }
            break;

        case 'p':
            if (!selected_member_is_present)
            {
                meeting.present_member_ids[meeting.present_member_count++] = selected_member.id;
            }
            for (int i = 0; i < meeting.absent_member_count; i++)
            {
                if (meeting.absent_member_ids[i] == selected_member.id)
                {
                    if (meeting.absent_member_count > i + 1)
                    {
                        for (int j = i; j < meeting.absent_member_count - 1; j++)
                        {
                            meeting.absent_member_ids[j] = meeting.absent_member_ids[j + 1];
                        }
                    }
                    meeting.absent_member_ids[--meeting.absent_member_count] = 0;
                    break;
                }
            }
            break;

        case 'a':
            if (selected_member_is_present)
            {
                meeting.absent_member_ids[meeting.absent_member_count++] = selected_member.id;
            }
            for (int i = 0; i < meeting.present_member_count; i++)
            {
                if (meeting.present_member_ids[i] == selected_member.id)
                {
                    if (meeting.present_member_count > i + 1)
                    {
                        for (int j = i; j < meeting.present_member_count - 1; j++)
                        {
                            meeting.present_member_ids[j] = meeting.present_member_ids[j + 1];
                        }
                    }
                    meeting.present_member_ids[--meeting.present_member_count] = 0;
                    break;
                }
            }
            break;
        }
    }

    free(attendee_ids);
    return meeting;
}

void list_club_meetings(struct Club club)
{
    print_greeting();

    if (club.meeting_count == 0)
    {
        printf("No meetings have yet been posted for club '%s'. ", club.name);
        prompt_return("the club menu");
        return;
    }

    struct Meeting *club_meetings = malloc(club.meeting_count * sizeof(struct Meeting));
    if (club_meetings == NULL)
    {
        printf("Allocation of %lld bytes of memory failed", club.meeting_count * sizeof(struct Meeting));
        exit(1);
    }
    int j = 0;
    for (int i = 0; i < meeting_count; i++)
    {
        if (meetings[i].club_id == club.id)
        {
            club_meetings[j] = meetings[i];
            j++;
            if (j == club.meeting_count)
            {
                break;
            }
        }
    }

    while (true)
    {
        print_greeting();
        printf("CLUB MEETING LISTING FOR '%s'\n\n", club.name);
        printf("Type in the meeting ID then press [Enter]. Otherwise, leave the field blank and press [Enter] to return to the club menu.\n\n");

        char ***rows = malloc(club.meeting_count * sizeof(char **));
        if (rows == NULL)
        {
            printf("Allocation of %lld bytes of memory failed", club.meeting_count * sizeof(char **));
            exit(1);
        }
        int total_columns = 5;
        char *column_headings[] = {"ID",
                                   "Convened At",
                                   "Topic",
                                   "Adjourned At",
                                   "Attendance Rate"};
        int column_widths[] = {strlen(column_headings[0]),
                               strlen("YYYY/MM/DD HH:MM"),
                               strlen(column_headings[2]),
                               strlen("YYYY/MM/DD HH:MM"),
                               strlen(column_headings[4])};

        for (int i = 0; i < club.meeting_count; i++)
        {
            struct Meeting meeting = club_meetings[i];

            int meeting_id_length = snprintf(NULL, 0, "%d", meeting.id);
            if (column_widths[0] < meeting_id_length)
            {
                column_widths[0] = meeting_id_length;
            }

            int meeting_topic_length = strlen(meeting.topic);
            if (column_widths[2] < meeting_topic_length)
            {
                column_widths[2] = meeting_topic_length;
            }

            double meeting_attendance_rate;
            int total_attendees = meeting.present_member_count + meeting.absent_member_count;
            if (total_attendees == 0)
            {
                meeting_attendance_rate = 0;
            }
            else
            {
                meeting_attendance_rate = ((double)meeting.present_member_count / total_attendees) * 100;
            }
            int meeting_attendance_rate_length = snprintf(NULL, 0, "%.1lf%%", meeting_attendance_rate);
            if (column_widths[4] < meeting_attendance_rate_length)
            {
                column_widths[4] = meeting_attendance_rate_length;
            }
        }

        for (int i = 0; i < club.meeting_count; i++)
        {
            struct Meeting meeting = club_meetings[i];

            rows[i] = malloc(total_columns * sizeof(char *));
            if (rows[i] == NULL)
            {
                printf("Allocation of %lld bytes of memory failed", total_columns * sizeof(char *));
                exit(1);
            }

            for (int j = 0; j < total_columns; j++)
            {
                rows[i][j] = malloc((column_widths[j] + 1) * sizeof(char));
                if (rows[i][j] == NULL)
                {
                    printf("Allocation of %lld bytes of memory failed", (column_widths[j] + 1) * sizeof(char));
                    exit(1);
                }

                switch (j)
                {
                case 0:
                    int meeting_id_length = snprintf(NULL, 0, "%d", meeting.id);
                    char *stringified_meeting_id = malloc((meeting_id_length + 1) * sizeof(char));
                    if (stringified_meeting_id == NULL)
                    {
                        printf("Allocation of %lld bytes of memory failed", (meeting_id_length + 1) * sizeof(char));
                        exit(1);
                    }
                    snprintf(stringified_meeting_id, meeting_id_length + 1, "%d", meeting.id);
                    strcpy(rows[i][j], stringified_meeting_id);
                    free(stringified_meeting_id);
                    break;

                case 1:
                    strcpy(rows[i][j], format_time_t(meeting.convened_at));
                    break;

                case 2:
                    strcpy(rows[i][j], meeting.topic);
                    break;

                case 3:
                    strcpy(rows[i][j], format_time_t(meeting.adjourned_at));
                    break;

                case 4:
                    double meeting_attendance_rate;
                    int total_attendees = meeting.present_member_count + meeting.absent_member_count;
                    if (total_attendees == 0)
                    {
                        meeting_attendance_rate = 0;
                    }
                    else
                    {
                        meeting_attendance_rate = ((double)meeting.present_member_count / total_attendees) * 100;
                    }

                    int meeting_attendance_rate_length = snprintf(NULL, 0, "%.1lf%%", meeting_attendance_rate);
                    char *stringified_attendance_rate = malloc((meeting_attendance_rate_length + 1) * sizeof(char));
                    if (stringified_attendance_rate == NULL)
                    {
                        printf("Allocation of %lld bytes of memory failed", (meeting_attendance_rate_length + 1) * sizeof(char));
                        exit(1);
                    }
                    snprintf(stringified_attendance_rate, meeting_attendance_rate_length + 1, "%.1lf%%", meeting_attendance_rate);
                    strcpy(rows[i][j], stringified_attendance_rate);
                    free(stringified_attendance_rate);
                    break;
                }
            }
        }

        print_table(total_columns, club.meeting_count, column_widths, 0, column_headings, rows, NULL);

        for (int i = 0; i < club.meeting_count; i++)
        {
            for (int j = 0; j < total_columns; j++)
            {
                free(rows[i][j]);
            }
            free(rows[i]);
        }
        free(rows);

        printf("\n\n");

        struct Meeting meeting = {0};
        int meeting_pos = -1;
        while (meeting.id == 0)
        {
            char meeting_lookup_query[5];
            fgets(meeting_lookup_query, sizeof(meeting_lookup_query) + sizeof(char), stdin);
            meeting_lookup_query[strlen(meeting_lookup_query) - 1] = '\0';
            if (strcmp(meeting_lookup_query, "") == 0)
            {
                break;
            }

            for (int i = 0; i < meeting_count; i++)
            {
                if (meetings[i].club_id != club.id)
                {
                    continue;
                }

                if (meetings[i].id == atoi(meeting_lookup_query))
                {
                    meeting = meetings[i];
                    meeting_pos = i;
                    break;
                }
            }
        }
        if (meeting_pos == -1)
        {
            break;
        }

        char choice = '\0';
        while (choice != 'r')
        {
            print_greeting();
            printf("MEETING INFORMATION\n\n", club.name);
            printf("> ID: %d\n", meeting.id);
            printf("> Convened At: %s\n", format_time_t(meeting.convened_at));
            printf("> Topic: %s\n", meeting.topic);
            printf("> Adjourned At: %s\n", format_time_t(meeting.adjourned_at));
            printf("> Attendance: %d/%d P; %d/%d A\n\n", meeting.present_member_count, club.member_count, meeting.absent_member_count, club.member_count);
            printf("Press [A] to view and edit the attendance sheet. Press [R] to return to the club menu.");

            choice = _getch();
            if (choice == 'a')
            {
                meeting = edit_attendance_sheet(club, meeting);
                meetings[meeting_pos] = meeting;
                save_data_to_file();
            }
        }
    }

    free(club_meetings);
}
