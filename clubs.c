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

struct Club clubs[MAX_CLUBS] = {{0}}; // The clubs array is declared with MAX_CLUBS as its size. clubs is a global variable.
struct Meeting *meetings = NULL;      // Initializes meetings to be NULL, will be populated later. meetings is a global variable.

// These count variables are initialized to be 0, will be read later from the data file. These are global variables.
int club_count = 0;
int prev_club_id = 0;
int meeting_count = 0;

// manage_clubs allows the user and the administrator to manage clubs.
void manage_clubs()
{
    while (true)
    {
        if (club_count == 0) // Administrators have to register a club through the administrator panel.
        {
            print_greeting();
            printf("No clubs have yet been registered. ");
            return prompt_return("the main menu");
        }

        struct Club club = {0}; // Initializes the Club struct with default values
        int club_pos = -1;      // -1 is the "escape value" to exit the loop
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

            char *club_lookup_query = read_variable_length_input();
            if (strcmp(club_lookup_query, "") == 0) // If the user enters nothing, break the loop.
            {
                free(club_lookup_query);
                break;
            }

            for (int i = 0; i < club_count; i++)
            {
                if (clubs[i].id == atoi(club_lookup_query)) // Checks whether the IDs match
                {
                    club = clubs[i];
                    club_pos = i;
                    break;
                }

                for (int j = 0; j < strlen(club_lookup_query); j++)
                {
                    club_lookup_query[j] = tolower(club_lookup_query[j]); // Makes all the characters in the club lookup query lowercase
                }
                char club_name[MAX_CLUB_NAME_LENGTH];
                strcpy(club_name, clubs[i].name);
                for (int j = 0; j < sizeof(club_name) / sizeof(char); j++)
                {
                    club_name[j] = tolower(club_name[j]); // Makes all the characters in the club name lowercase
                }
                if (strcmp(club_name, club_lookup_query) == 0) // Checks whether the club lookup query and club name matches
                {
                    club = clubs[i];
                    club_pos = i;
                    break;
                }
            }

            free(club_lookup_query);
        }
        if (club_pos == -1) // Breaks the loop if the club position has been set (which happens when the user enters nothing)
        {
            break;
        }

        bool is_authorized = prompt_authorization("Managing a club", club.password, STANDARD, "the main menu");
        if (!is_authorized) // Returns to the club menu if the user is not authorized
        {
            continue;
        }

        char choice = '\0';
        while (choice != 'r')
        {
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

            choice = tolower(_getch());
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
                list_club_meetings(club_pos);
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

// register_club allows the administrator to register a new club.
void register_club()
{
    print_greeting();
    printf("REGISTER NEW CLUB\n\n");

    if (club_count == MAX_CLUBS) // If the number of registered clubs is at the club limit, inform the user and return to the administrator panel.
    {
        printf("A maximum of %d clubs has already been registered. ", MAX_CLUBS);
        return prompt_return("the main menu");
    }

    if (student_count == 0) // A club must have at least one student representative at all times.
    {
        printf("Each club must have at least one student representative and there are 0 registered students. ");
        return prompt_return("the main menu");
    }

    struct Club club = {0}; // Initializes the Club struct with default values
    while (true)
    {
        print_greeting();
        printf("REGISTER NEW CLUB\n\n");
        printf("> Name: ");
        char *club_name = read_variable_length_input();
        strcpy(club.name, club_name);
        free(club_name);
        if (strlen(club.name) > 0) // The club name must be at least 1 character long
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
    while (strcmp(club.weekly_meeting_day, "") == 0) // Runs as long as no valid weekly meeting day has been set
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
        char *club_description = read_variable_length_input();
        strcpy(club.description, club_description);
        free(club_description);
        if (strlen(club.description) > 0) // The club description must be at least 1 character long
        {
            break;
        }
    }

    char student_reps[1024] = ""; // The student representative IDs are collected but for human-readability, their names should be printed.
    club.student_rep_count = 0;
    while (true)
    {
        print_greeting();
        printf("REGISTER NEW CLUB\n\n");
        printf("> Name: %s\n", club.name);
        printf("> Weekly Meeting Day: %s\n", club.weekly_meeting_day);
        printf("> Description: %s\n", club.description);
        printf("> Student Representatives' IDs (comma-separated): ");

        char *student_rep_ids = read_variable_length_input();
        if (strcmp(student_rep_ids, "") != 0) // Parses the input only if input was given
        {
            char *student_rep_id = strtok(student_rep_ids, ", ");                                // Splits the user input by a comma delimiter
            while (student_rep_id != NULL && club.student_rep_count < MAX_STUDENT_REPS_PER_CLUB) // Ensures that the club student rep count limit is not exceeded
            {
                bool is_duplicate = false; // Checks if the student ID was already entered
                for (int i = 0; i < club.student_rep_count; i++)
                {
                    if (atoi(student_rep_id) == club.student_rep_ids[i]) // If the student ID was already entered, it is marked as a duplicate.
                    {
                        is_duplicate = true;
                        break;
                    }
                }
                if (!is_duplicate) // Ignores duplicates student reps
                {
                    for (int i = 0; i < student_count; i++)
                    {
                        struct Student student = students[i];
                        if (student.id == atoi(student_rep_id) && atoi(student_rep_id) != 0) // Checks for a valid match between the inputted student ID and the students in the database
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
                            break; // Stops searching once the student is found
                        }
                    }
                }
                student_rep_id = strtok(NULL, ", "); // Continues onto the next student ID
            }

            if (club.student_rep_count > 0) // Once there is at least 1 student rep, breaks the loop
            {
                break;
            }
        }
        free(student_rep_ids); // Since student_rep_ids was dynamically allocated, the memory must be freed once no longer in use.
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
        char *password = read_variable_length_input(); // A password can be of any (unknown) length, so the read_variable_length_input function is used.
        if (strlen(password) < MIN_PASSWORD_LENGTH)    // Ensures that the password meets the minimum criterion for security
        {
            free(password); // read_variable_length_input() returns a pointer to dynamically-allocated memory
            continue;
        }

        print_greeting();
        printf("REGISTER NEW CLUB\n\n");
        printf("> Name: %s\n", club.name);
        printf("> Weekly Meeting Day: %s\n", club.weekly_meeting_day);
        printf("> Description: %s\n", club.description);
        printf("> Student Representatives: %s\n", student_reps);

        printf("> Password: ");
        for (int i = 0; i < strlen(password); i++) // Reprints the original password with the contents blurred
        {
            printf("*");
        }
        printf("\n");
        printf("> Confirm Password: ");
        char *password_confirmation = read_variable_length_input();
        if (strcmp(password, password_confirmation) == 0) // Checks to see if the password confirmation matches the original password
        {
            club.password_length = strlen(password) + 1;                 // strlen does not account for the null terminator '\0'.
            club.password = malloc(club.password_length * sizeof(char)); // Allocates the necessary memory since the password is of arbitrary length
            strcpy(club.password, password);
            free(password);
            free(password_confirmation);
            password = NULL;
            password_confirmation = NULL;
            break;
        }
        // Otherwise, if the password does not match, the process restarts.
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

    // Prompts the user to confirm registration
    printf("Confirm registration of club? (y/n)", club.name);
    char confirmation = _getch();
    printf("\n");
    if (confirmation != 'y')
    {
        print_greeting();
        printf("Registration of club '%s' cancelled. ", club.name);
        return prompt_return("the main menu");
    }

    club.id = ++prev_club_id;        // Increments the previous club ID global variable and sets the club ID to it
    club.registered_at = time(NULL); // Sets the registration time to the current time

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
                    if (student.club_memberships[k] == 0) // Placeholder values assume a value of 0.
                    {
                        students[i].club_memberships[k] = club.id; // Sets the club membership at that position to the club ID
                        break;
                    }
                }
                break;
            }
        }
    }
    clubs[club_count++] = club; // Increments the number of clubs and in the clubs array, sets the previous value which is equivalent to the new club_count - 1 to the club
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

// view_club_info displays the information of a club.
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

    // Prints all the student reps
    for (int i = 0; i < student_count; i++)
    {
        struct Student student = students[i];
        for (int j = 0; j < club.student_rep_count; j++)
        {
            if (club.student_rep_ids[j] == student.id)
            {
                printf("[%d] %s", student.id, student.name);
                if (j < club.student_rep_count - 1) // If this student rep is not the last, print a comma
                {
                    printf(", ");
                }
            }
        }
    }
    printf("\n\n");

    prompt_return("the club menu");
}

// manage_club_members allows the administrator and club representatives to view members, enroll new members and rescind memberships.
void manage_club_members(int club_pos)
{
    char choice = '\0';
    while (choice != 'r')
    {
        struct Club club = clubs[club_pos];
        print_greeting();
        printf("MANAGE MEMBERS OF CLUB '%s'\n\n", club.name);

        // The number of clubs is variable, so a fixed-size array cannot be allocated.
        struct Student *club_members = malloc(club.member_count * sizeof(struct Student));
        int k = 0;
        for (int i = 0; i < student_count; i++)
        {
            for (int j = 0; j < MAX_CLUB_MEMBERSHIPS; j++)
            {
                if (students[i].club_memberships[j] == club.id)
                {
                    club_members[k] = students[i]; // Adds the student to the club_members array
                    k++;
                    if (k == club.member_count)
                    {
                        break;
                    }
                }
                if (k == club.member_count) // Breaks the loop once the number of club members have been added for efficiency savings
                {
                    break;
                }
            }
        }

        // contents[i][j] is such that i is the column index and j is the row index of a table.
        char ***contents = malloc(club.member_count * sizeof(char **));
        if (contents == NULL)
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
        // Initializes default column widths based on the length of the heading. Column widths may be dynamically resized later depending on the size of the row contents.
        int column_widths[4] = {strlen(column_headings[0]),
                                strlen(column_headings[1]),
                                strlen(column_headings[2]),
                                strlen(column_headings[3])};

        // The club members are iterated over twice. This first time is to adjust the column_widths accordingly.
        for (int i = 0; i < club.member_count; i++)
        {
            struct Student student = club_members[i];

            int student_id_length = snprintf(NULL, 0, "%d", student.id);
            if (column_widths[0] < student_id_length)
            {
                column_widths[0] = student_id_length; // Increases student_id_length if the row content length exceeds the current student_id_length
            }

            int student_name_length = strlen(student.name);
            if (column_widths[1] < student_name_length)
            {
                column_widths[1] = student_name_length; // Increases student_name_length if the row content length exceeds the current student_name_length
            }

            int student_class_length = strlen(student.class);
            if (column_widths[2] < student_class_length)
            {
                column_widths[2] = student_class_length; // Increases student_class_length if the row content length exceeds the current student_class_length
            }

            int student_email_address_length = strlen(student.email_address);
            if (column_widths[3] < student_email_address_length)
            {
                column_widths[3] = student_email_address_length; // Increases student_email_address_length if the row content length exceeds the current student_email_address_length
            }
        }

        // Now that the column widths have been properly calculated, the contents of the table will be populated.
        for (int i = 0; i < club.member_count; i++)
        {
            struct Student student = club_members[i];

            contents[i] = malloc(total_columns * sizeof(char *)); // contents[i] is the ith column of the table.
            if (contents[i] == NULL)
            {
                printf("Allocation of %lld bytes of memory failed", total_columns * sizeof(char *));
                exit(1);
            }

            for (int j = 0; j < total_columns; j++)
            {
                contents[i][j] = malloc((column_widths[j] + 1) * sizeof(char)); // contents[i][j] represents the cell defined by the jth row and ith column.
                if (contents[i][j] == NULL)
                {
                    printf("Allocation of %lld bytes of memory failed", (column_widths[j] + 1) * sizeof(char));
                    exit(1);
                }

                switch (j) // Formats the cell contents based on the type of data (indicated by j, that is the column)
                {
                case 0:
                    int student_id_length = snprintf(NULL, 0, "%d", student.id);
                    char *stringified_student_id = malloc((student_id_length + 1) * sizeof(char)); // The student ID length is variable, so a fixed-size array cannot be declared.
                    if (stringified_student_id == NULL)
                    {
                        printf("Allocation of %lld bytes of memory failed", (student_id_length + 1) * sizeof(char));
                        exit(1);
                    }
                    snprintf(stringified_student_id, student_id_length + 1, "%d", student.id); // strcpy accepts a string input, not an integer input, so student.id must be converted to a string.
                    strcpy(contents[i][j], stringified_student_id);
                    free(stringified_student_id);
                    break;

                case 1:
                    strcpy(contents[i][j], student.name);
                    break;

                case 2:
                    strcpy(contents[i][j], student.class);
                    break;

                case 3:
                    strcpy(contents[i][j], student.email_address);
                    break;
                }
            }
        }

        int member_count_length = snprintf(NULL, 0, "%d", club.member_count);
        int total_footer_rows = 1;                                       // There is 1 footer row for the member count.
        char **footer_rows = malloc(total_footer_rows * sizeof(char *)); // The account balance length is variable, so a fixed-size array cannot be declared.
        footer_rows[0] = malloc((strlen("Member Count: ") + member_count_length + 1) * sizeof(char));
        sprintf(footer_rows[0], "Member Count: %d", club.member_count);

        print_table(total_columns, club.member_count, column_widths, 1, column_headings, contents, footer_rows);

        for (int i = 0; i < total_footer_rows; i++)
        {
            free(footer_rows[i]);
        }
        free(footer_rows);
        for (int i = 0; i < club.transaction_count; i++)
        {
            for (int j = 0; j < total_columns; j++)
            {
                free(contents[i][j]);
            }
            free(contents[i]);
        }
        free(club_members);

        printf("\n\n");
        printf("Press [E] to enroll a new member or [U] to rescind a membership. Otherwise, press [R] to return to the club menu.");

        choice = tolower(_getch());
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

// enroll_new_member allows the administrator and club representatives to enroll a new member to the club.
void enroll_new_member(struct Club club, int club_pos)
{
    struct Student student = {0};
    int student_pos = -1;

    while (student.id == 0)
    {
        print_greeting();
        printf("ENROLL NEW MEMBER TO CLUB '%s'\n\n", club.name);
        printf("Type in the full name or ID of the student you wish to enroll to the club '%s' and then press [Enter]. Otherwise, leave the field blank and press [Enter] to return to the club menu.\n\n", club.name);

        char *student_lookup_query = read_variable_length_input();
        if (strcmp(student_lookup_query, "") == 0)
        {
            free(student_lookup_query);
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

            for (int j = 0; j < strlen(student_lookup_query); j++)
            {
                student_lookup_query[j] = tolower(student_lookup_query[j]); // Makes every character in the student lookup query lowercase
            }
            char student_name[MAX_STUDENT_NAME_LENGTH];
            strcpy(student_name, students[i].name);
            for (int j = 0; j < sizeof(student_name) / sizeof(char); j++)
            {
                student_name[j] = tolower(student_name[j]); // Makes every character in the student name lowercase
            }
            if (strcmp(student_name, student_lookup_query) == 0) // Checks if the student name and student lookup query (now case insensitive) match
            {
                student = students[i];
                student_pos = i;
                break;
            }
        }

        free(student_lookup_query);
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

    clubs[club_pos].member_ids[club.member_count] = student.id; // Adds the student's ID to the club member list
    clubs[club_pos].member_count++;                             // Increments the club's member count
    for (int i = 0; i < MAX_CLUB_MEMBERSHIPS; i++)
    {
        if (student.club_memberships[i] == 0)
        {
            students[student_pos].club_memberships[i] = club.id; // Adds the club ID to the student's club memberships
            break;
        }
    }
    save_data_to_file();
    print_greeting();
    printf("%s [%d] was successfully enrolled to the club '%s'. ", student.name, student.id, club.name);
    prompt_return("the club menu");
}

// rescind_membership allows the administrator and club representatives to rescind a student's membership from a club.
void rescind_membership(struct Club club, int club_pos)
{
    struct Student student = {0};
    int student_pos = -1;

    while (student.id == 0)
    {
        print_greeting();
        printf("RESCIND MEMBERSHIP FROM CLUB '%s'\n\n", club.name);
        printf("Type in the full name or ID of the student whose membership of '%s' you wish to rescind and then press [Enter]. Otherwise, leave the field blank and press [Enter] to return to the club menu.\n\n", club.name);

        char *student_lookup_query = read_variable_length_input();
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

            for (int j = 0; j < strlen(student_lookup_query); j++)
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
                free(student_lookup_query);
                break;
            }
            free(student_lookup_query);
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

        choice = tolower(_getch());

        switch (choice)
        {
        case '1':
            print_greeting();

            printf("After typing the new name for '%s', press [Enter] to confirm.\n\n", club.name);
            char *new_name = read_variable_length_input();
            while (strcmp(new_name, "") == 0)
            {
                free(new_name); // Frees old pointer to new_name
                char *new_name = read_variable_length_input();
            }
            char old_name[MAX_CLUB_NAME_LENGTH];
            strcpy(old_name, club.name);
            strcpy(clubs[club_pos].name, new_name);
            free(new_name);
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
            char *new_description = read_variable_length_input();
            while (strcmp(new_description, "") == 0)
            {
                free(new_description); // Frees old pointer to new_description
                char *new_description = read_variable_length_input();
            }
            strcpy(clubs[club_pos].description, new_description);
            free(new_description);
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

                char *student_rep_ids = read_variable_length_input();
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
                char *password = read_variable_length_input();
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
                char *password_confirmation = read_variable_length_input();
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
    for (int i = 0; i < meeting_count; i++)
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
        char *meeting_topic = read_variable_length_input();
        strcpy(meeting.topic, meeting_topic);
        if (strlen(meeting.topic) > 0)
        {
            free(meeting_topic);
            break;
        }
        free(meeting_topic);
    }

    while (true)
    {
        print_greeting();
        printf("POST MEETING FOR CLUB '%s'\n\n", club.name);
        printf("> Meeting Topic (max %d characters): %s\n", MAX_MEETING_TOPIC_LENGTH, meeting.topic);
        printf("> Convened At (DD/MM/YYYY HH:MM): ");

        meeting.convened_at = parse_time_input();
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

        meeting.adjourned_at = parse_time_input();
        if (meeting.adjourned_at == (time_t)-1 || meeting.adjourned_at > time(NULL) || meeting.adjourned_at <= meeting.convened_at)
        {
            continue;
        }
        else
        {
            break;
        }
    }

    // Marks all club members as absent by default
    // User will have the option to manually mark members as present later.
    meeting.absent_member_count = club.member_count;
    for (int i = 0; i < club.member_count; i++)
    {
        if (club.member_ids[i] == 0)
        {
            break;
        }
        meeting.absent_member_ids[i] = club.member_ids[i];
    }

    meeting = edit_attendance_sheet(club, meeting);                             // User may edit the attendance sheet, marking members present as appropriate.
    meeting.id = ++clubs[club_pos].prev_meeting_id;                             // Increments the club's previous meeting ID
    clubs[club_pos].meeting_count++;                                            // Increments the club's meeting count
    meetings = realloc(meetings, (meeting_count + 1) * sizeof(struct Meeting)); // meetings is dynamically allocated memory so it must be resized in order to store the new meeting
    if (meetings == NULL)
    {
        printf("Reallocation of %lld bytes of memory failed", (meeting_count + 1) * sizeof(struct Meeting));
        exit(1);
    }

    // Saves the new meeting
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

// Users can interactively edit the attendance sheet, marking members present or absent as appropriate.
// Returns the modified meeting struct
struct Meeting edit_attendance_sheet(struct Club club, struct Meeting meeting)
{
    int attendee_count = meeting.present_member_count + meeting.absent_member_count;
    if (attendee_count == 0) // Ensures that the attendance sheet consists of members who still exist in the system
    {
        print_greeting();
        printf("None of the students who attended this meeting currently exist in the system. The attendance sheet is blank. ");
        prompt_return("the meeting summary");
        return meeting;
    }

    int *attendee_ids = malloc(attendee_count * sizeof(int)); // The number of attendees varies from meeting to meeting, so attendee_ids must be dynamically allocated.
    if (attendee_ids == NULL)
    {
        printf("Allocation of %lld bytes of memory failed", attendee_count * sizeof(int));
        exit(1);
    }

    // Adds the absent members' IDs and the present members' IDs to the attendee_ids array
    for (int i = 0; i < meeting.absent_member_count; i++)
    {
        attendee_ids[i] = meeting.absent_member_ids[i];
    }
    for (int i = 0; i < meeting.present_member_count; i++)
    {
        attendee_ids[meeting.absent_member_count + i] = meeting.present_member_ids[i];
    }

    // The attendance sheet is interactive and updates with every keystroke.
    int cursor_position = 0; // The cursor indicates the currently selected attendee.
    char choice = '\0';
    while (choice != 'r')
    {
        print_greeting();
        printf("EDIT MEETING ATTENDANCE SHEET FOR CLUB '%s'\n\n", club.name);
        printf("- The \">\" symbol indicates which member is currently selected. Press the [W] and [S] keys to amend your current selection.\n");
        printf("- All members are initially marked as absent. Press [P] and [A] to mark the currently selected member as present and absent respectively.\n");
        printf("- Press [R] to confirm and save the attendance sheet.\n\n");

        struct Student selected_member = {0};
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
            if (i == cursor_position) // The cursor position is identical to the position of the currently selected member.
            {
                selected_member = member;
                selected_member_is_present = is_present;
                printf("> "); // The ">" symbol indicates the cursor position.
            }
            else
            {
                printf("  "); // Adds padding to ensure that the currently selected member aligns with other members
            }
            printf("[%d] %s: %s\n", member.id, member.name, is_present ? "P" : "A"); // Displays the member's ID, name, and attendance status
        }

        choice = tolower(_getch());
        switch (choice)
        {
        case 's':                                     // An input of "s" moves the cursor down the list of attendees.
            if (cursor_position < attendee_count - 1) // Ensures that the cursor does not move past the last attendee
            {
                cursor_position++;
            }
            break;

        case 'w':                    // An input of "w" moves the cursor up the list of attendees.
            if (cursor_position > 0) // Ensures that the cursor does not move past the first attendee.
            {
                cursor_position--;
            }
            break;

        case 'p':
            if (selected_member_is_present) // Ensures that the selected member is not double marked as present
            {
                continue;
            }

            meeting.present_member_ids[meeting.present_member_count++] = selected_member.id; // Adds the selected member to the present member ID list
            // Removes the selected member from the absent list and places them in the present list
            for (int i = 0; i < meeting.absent_member_count; i++)
            {
                if (meeting.absent_member_ids[i] == selected_member.id)
                {
                    if (meeting.absent_member_count > i + 1) // Checks if the absent member is the last member in the absent list
                    {
                        // Shifts all members after the selected member one position down one position
                        for (int j = i; j < meeting.absent_member_count - 1; j++)
                        {
                            meeting.absent_member_ids[j] = meeting.absent_member_ids[j + 1];
                        }
                    }
                    meeting.absent_member_ids[--meeting.absent_member_count] = 0; // Decrements the absent member count and removes the last member
                    break;
                }
            }
            break;

        case 'a':
            if (!selected_member_is_present) // Ensures that the selected member is not double marked as absent
            {
                continue;
            }

            meeting.absent_member_ids[meeting.absent_member_count++] = selected_member.id; // Adds the selected member to the absent member ID list
            // Removes the selected member from the present list and places them in the absent list
            for (int i = 0; i < meeting.present_member_count; i++)
            {
                if (meeting.present_member_ids[i] == selected_member.id)
                {
                    if (meeting.present_member_count > i + 1) // Checks if the present member is the last member in the present list
                    {
                        for (int j = i; j < meeting.present_member_count - 1; j++) // Shifts all members after the selected member one position down one position
                        {
                            meeting.present_member_ids[j] = meeting.present_member_ids[j + 1];
                        }
                    }
                    meeting.present_member_ids[--meeting.present_member_count] = 0; // Decrements the present member count and removes the last member
                    break;
                }
            }
            break;
        }
    }

    free(attendee_ids);
    return meeting;
}

// list_club_meetings allows the administrator and club representatives to view a list of all meetings for a specific club.
// The user can select a meeting to view its details.
void list_club_meetings(int club_pos)
{
    print_greeting();

    struct Club club = clubs[club_pos];
    if (club.meeting_count == 0) // Returns to the club menu if there are no posted meetings for the club
    {
        printf("No meetings have yet been posted for club '%s'. ", club.name);
        prompt_return("the club menu");
        return;
    }

    struct Meeting *club_meetings = malloc(club.meeting_count * sizeof(struct Meeting)); // The number of club meetings is variable, so club_meetings must be dynamically allocated
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
                break; // Breaks the loop once the number of club meetings have been added for efficiency savings
            }
        }
    }

    while (true)
    {
        print_greeting();
        printf("CLUB MEETING LISTING FOR '%s'\n\n", club.name);
        printf("Type in the meeting ID then press [Enter]. Otherwise, leave the field blank and press [Enter] to return to the club menu.\n\n");

        // contents[i][j] is such that i is the column index and j is the row index of a table.
        char ***contents = malloc(club.meeting_count * sizeof(char **));
        if (contents == NULL)
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
        // Initializes default column widths based on the length of the heading. Column widths may be dynamically resized later depending on the size of the row contents.
        int column_widths[] = {strlen(column_headings[0]),
                               strlen("YYYY/MM/DD HH:MM"),
                               strlen(column_headings[2]),
                               strlen("YYYY/MM/DD HH:MM"),
                               strlen(column_headings[4])};

        // The club meetings are iterated over twice. This first time is to adjust the column_widths accordingly.
        for (int i = 0; i < club.meeting_count; i++)
        {
            struct Meeting meeting = club_meetings[i];

            int meeting_id_length = snprintf(NULL, 0, "%d", meeting.id);
            if (column_widths[0] < meeting_id_length)
            {
                column_widths[0] = meeting_id_length; // Increases meeting_id_length if the row content length exceeds the current meeting_id_length
            }

            int meeting_topic_length = strlen(meeting.topic);
            if (column_widths[2] < meeting_topic_length)
            {
                column_widths[2] = meeting_topic_length; // Increases meeting_topic_length if the row content length exceeds the current meeting_topic_length
            }

            double meeting_attendance_rate;
            int total_attendees = meeting.present_member_count + meeting.absent_member_count;
            if (total_attendees == 0) // Division by 0 is undefined so the attendance rate is just set to 0 if there are 0 total attendees.
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
                column_widths[4] = meeting_attendance_rate_length; // Increases meeting_attendance_rate_length if the row content length exceeds the current meeting_attendance_rate_length
            }
        }

        // Now that the column widths have been properly calculated, the contents of the table will be populated.
        for (int i = 0; i < club.meeting_count; i++)
        {
            struct Meeting meeting = club_meetings[i];

            contents[i] = malloc(total_columns * sizeof(char *)); // contents[i] is the ith column of the table.
            if (contents[i] == NULL)
            {
                printf("Allocation of %lld bytes of memory failed", total_columns * sizeof(char *));
                exit(1);
            }

            for (int j = 0; j < total_columns; j++) // Formats the cell contents based on the type of data (indicated by j, that is the column)
            {
                contents[i][j] = malloc((column_widths[j] + 1) * sizeof(char)); // contents[i][j] represents the cell defined by the jth row and ith column.
                if (contents[i][j] == NULL)
                {
                    printf("Allocation of %lld bytes of memory failed", (column_widths[j] + 1) * sizeof(char));
                    exit(1);
                }

                switch (j) // Formats the cell contents based on the type of data (indicated by j, that is the column)
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
                    strcpy(contents[i][j], stringified_meeting_id); // strcpy accepts a string input, not an integer input, so meeting.id must be converted to a string.
                    free(stringified_meeting_id);
                    break;

                case 1:
                    strcpy(contents[i][j], format_time_t(meeting.convened_at));
                    break;

                case 2:
                    strcpy(contents[i][j], meeting.topic);
                    break;

                case 3:
                    strcpy(contents[i][j], format_time_t(meeting.adjourned_at));
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
                    strcpy(contents[i][j], stringified_attendance_rate);
                    free(stringified_attendance_rate);
                    break;
                }
            }
        }

        print_table(total_columns, club.meeting_count, column_widths, 0, column_headings, contents, NULL);

        for (int i = 0; i < club.meeting_count; i++)
        {
            for (int j = 0; j < total_columns; j++)
            {
                free(contents[i][j]);
            }
            free(contents[i]);
        }
        free(contents);

        printf("\n\n");

        struct Meeting meeting = {0};
        int meeting_pos = -1;
        while (meeting.id == 0)
        {
            char *meeting_lookup_query = read_variable_length_input();
            if (strcmp(meeting_lookup_query, "") == 0)
            {
                free(meeting_lookup_query);
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
            free(meeting_lookup_query);
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

            choice = tolower(_getch());
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
