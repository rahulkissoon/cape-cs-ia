#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <direct.h>
#include "accounting.h"
#include "auth.h"
#include "core.h"
#include "linked_lists.h"
#include "menu_system.h"
#include "clubs.h"
#include "students.h"

char school_name[MAX_SCHOOL_NAME_LENGTH] = "";

// save_data_to_file ensures that data persists across sessions.
// Once the process safely exits, data can be reloaded from the file during the next session.
void save_data_to_file()
{
    // Checks if data directory exists. If none exists, creates directory.
    _mkdir("data");

    // Opens the datafiles in write and binary mode
    FILE *core_datafile_ptr = fopen("data/core.bin", "wb");
    FILE *accounting_datafile_ptr = fopen("data/accounting.bin", "wb");
    FILE *clubs_datafile_ptr = fopen("data/clubs.bin", "wb");
    FILE *meetings_datafile_ptr = fopen("data/meetings.bin", "wb");
    FILE *students_datafile_ptr = fopen("data/students.bin", "wb");

    while (core_datafile_ptr == NULL || accounting_datafile_ptr == NULL || clubs_datafile_ptr == NULL || meetings_datafile_ptr == NULL || students_datafile_ptr == NULL)
    {
        clear_console();
        printf("Error in saving data to datafiles. Press [Q] to quit without saving data (not recommended) or any other key to try again.\n");
        char decision_on_err = _getch(); // In the event that the file cannot be opened, the user can choose to quit the program or try again.
        if (tolower(decision_on_err) == 'q')
        {
            printf("Quitting program...");
            exit(1);
        }
        else
        {
            core_datafile_ptr = fopen("data/core.bin", "wb");
            accounting_datafile_ptr = fopen("data/accounting.bin", "wb");
            clubs_datafile_ptr = fopen("data/clubs.bin", "wb");
            meetings_datafile_ptr = fopen("data/meetings.bin", "wb");
            students_datafile_ptr = fopen("data/students.bin", "wb");
        }
    }

    fwrite(&school_name, sizeof(char), MAX_SCHOOL_NAME_LENGTH, core_datafile_ptr);

    int admin_password_length = strlen(admin_password) + 1;
    fwrite(&admin_password_length, sizeof(int), 1, core_datafile_ptr);              // The admin password is of variable length so its length must be recorded.
    fwrite(admin_password, sizeof(char), admin_password_length, core_datafile_ptr); // Saves the admin password

    fwrite(&club_count, sizeof(int), 1, clubs_datafile_ptr);   // Saves the club count
    fwrite(&prev_club_id, sizeof(int), 1, clubs_datafile_ptr); // Saves the previous club ID
    for (int i = 0; i < club_count; i++)                       // Saves all club info
    {
        struct Club club = clubs[i];

        // Prevent writing pointers to the files
        char *club_password = club.password;
        struct Node *curr_member_id = club.member_ids;
        struct Node *curr_student_rep_id = club.student_rep_ids;
        club.password = NULL;
        club.member_ids = NULL;
        club.student_rep_ids = NULL;

        fwrite(&club, sizeof(struct Club), 1, clubs_datafile_ptr);
        fwrite(club_password, sizeof(char), clubs[i].password_length, clubs_datafile_ptr);

        while (curr_member_id != NULL)
        {
            fwrite(&curr_member_id->data, sizeof(int), 1, clubs_datafile_ptr);
            curr_member_id = curr_member_id->next;
        }
        while (curr_student_rep_id != NULL)
        {
            fwrite(&curr_student_rep_id->data, sizeof(int), 1, clubs_datafile_ptr);
            curr_student_rep_id = curr_student_rep_id->next;
        }
    }

    for (int i = 0; i < transaction_count; i++) // Saves all transactions
    {
        fwrite(&transactions[i], sizeof(struct Transaction), 1, accounting_datafile_ptr);
    }

    for (int i = 0; i < meeting_count; i++) // Saves all club meetings
    {
        struct Meeting meeting = meetings[i];

        // Prevent writing pointers to files
        struct Node *curr_present_member_id = meeting.present_member_ids;
        struct Node *curr_absent_member_id = meeting.absent_member_ids;
        meeting.absent_member_ids = NULL;
        meeting.present_member_ids = NULL;

        fwrite(&meeting, sizeof(struct Meeting), 1, meetings_datafile_ptr);

        while (curr_absent_member_id != NULL)
        {
            fwrite(&curr_absent_member_id->data, sizeof(int), 1, meetings_datafile_ptr);
            curr_absent_member_id = curr_absent_member_id->next;
        }
        while (curr_present_member_id != NULL)
        {
            fwrite(&curr_present_member_id->data, sizeof(int), 1, meetings_datafile_ptr);
            curr_present_member_id = curr_present_member_id->next;
        }
    }

    fwrite(&student_count, sizeof(int), 1, students_datafile_ptr);   // Saves the student count
    fwrite(&prev_student_id, sizeof(int), 1, students_datafile_ptr); // Saves the previous student ID
    for (int i = 0; i < student_count; i++)                          // Saves all student info
    {
        struct Student student = students[i];

        // Prevent writing pointers to files
        struct Node *club_memberships = student.club_memberships;
        student.club_memberships = NULL;
        fwrite(&student, sizeof(struct Student), 1, students_datafile_ptr);

        int club_membership_count = get_size(club_memberships);
        fwrite(&club_membership_count, sizeof(int), 1, students_datafile_ptr);
        while (club_memberships != NULL)
        {
            fwrite(&club_memberships->data, sizeof(int), 1, students_datafile_ptr);
            club_memberships = club_memberships->next;
        }
    }

    // Closes the datafiles
    fclose(core_datafile_ptr);
    fclose(accounting_datafile_ptr);
    fclose(clubs_datafile_ptr);
    fclose(meetings_datafile_ptr);
    fclose(students_datafile_ptr);
}

// load_data_from_file reads the database, loading all data into the active memory of the program.
void load_data_from_file()
{
    // Opens the datafiles in read and binary mode
    FILE *core_datafile_ptr = fopen("data/core.bin", "rb");
    FILE *accounting_datafile_ptr = fopen("data/accounting.bin", "rb");
    FILE *clubs_datafile_ptr = fopen("data/clubs.bin", "rb");
    FILE *meetings_datafile_ptr = fopen("data/meetings.bin", "rb");
    FILE *students_datafile_ptr = fopen("data/students.bin", "rb");

    // If the file does not exist, that means that this is the first time the program is being run.
    // Active memory with default values will be used and stored later.

    if (core_datafile_ptr != NULL)
    {
        fread(&school_name, sizeof(char), MAX_SCHOOL_NAME_LENGTH, core_datafile_ptr); // Reads the school name

        int admin_password_length = 0;
        fread(&admin_password_length, sizeof(int), 1, core_datafile_ptr);
        admin_password = malloc(admin_password_length * sizeof(char)); // Allocates enough memory based on the length of the admin password
        fread(admin_password, sizeof(char), admin_password_length, core_datafile_ptr);
    }

    if (clubs_datafile_ptr != NULL)
    {
        fread(&club_count, sizeof(int), 1, clubs_datafile_ptr);   // Reads the club count
        fread(&prev_club_id, sizeof(int), 1, clubs_datafile_ptr); // Reads the previous club ID
        for (int i = 0; i < club_count; i++)                      // Reads all club info
        {
            struct Club *club = &clubs[i];
            fread(club, sizeof(struct Club), 1, clubs_datafile_ptr);
            club->password = malloc(clubs[i].password_length * sizeof(char)); // Allocates enough memory based on the length of the club password
            fread(club->password, sizeof(char), club->password_length, clubs_datafile_ptr);

            club->member_ids = NULL;
            for (int j = 0; j < club->member_count; j++)
            {
                int member_id = 0;
                fread(&member_id, sizeof(member_id), 1, clubs_datafile_ptr);
                insert_after_last_node(&club->member_ids, member_id);
            }

            club->student_rep_ids = NULL;
            for (int j = 0; j < club->student_rep_count; j++)
            {
                int student_rep_id = 0;
                fread(&student_rep_id, sizeof(student_rep_id), 1, clubs_datafile_ptr);
                insert_after_last_node(&club->student_rep_ids, student_rep_id);
            }

            meeting_count += clubs[i].meeting_count;         // Increments the global meeting count based on meeting count of the club
            transaction_count += clubs[i].transaction_count; // Increments the global transaction count based on transaction count of the club
        }
    }

    // The reason for the ternary operator is to prevent allocating 0 bytes of memory. This can cause undefined behaviour in C.
    if (accounting_datafile_ptr != NULL)
    {
        transactions = malloc((transaction_count == 0 ? 1 : transaction_count) * sizeof(struct Transaction));
        for (int i = 0; i < transaction_count; i++) // Reads all transactions
        {
            fread(&transactions[i], sizeof(struct Transaction), 1, accounting_datafile_ptr);
        }
    }

    if (meetings_datafile_ptr != NULL)
    {
        meetings = malloc((meeting_count == 0 ? 1 : meeting_count) * sizeof(struct Meeting));
        if (meetings == NULL)
        {
            report_alloc_error((meeting_count == 0 ? 1 : meeting_count) * sizeof(struct Meeting));
        }
        for (int i = 0; i < meeting_count; i++)
        {
            struct Meeting *meeting = &meetings[i];
            fread(meeting, sizeof(struct Meeting), 1, meetings_datafile_ptr);

            meeting->absent_member_ids = NULL;
            meeting->present_member_ids = NULL;

            for (int k = 0; k < meeting->absent_member_count; k++)
            {
                int absent_member_id = 0;
                fread(&absent_member_id, sizeof(absent_member_id), 1, meetings_datafile_ptr);
                insert_after_last_node(&meeting->absent_member_ids, absent_member_id);
            }
            for (int k = 0; k < meeting->present_member_count; k++)
            {
                int present_member_id = 0;
                fread(&present_member_id, sizeof(present_member_id), 1, meetings_datafile_ptr);
                insert_after_last_node(&meeting->present_member_ids, present_member_id);
            }
        }
    }

    if (students_datafile_ptr != NULL)
    {
        fread(&student_count, sizeof(int), 1, students_datafile_ptr);   // Reads the student count
        fread(&prev_student_id, sizeof(int), 1, students_datafile_ptr); // Reads the previous student ID
        // The reason for the ternary operator is to prevent allocating 0 bytes of memory. This can cause undefined behaviour in C.
        students = malloc((student_count == 0 ? 1 : student_count) * sizeof(struct Student));
        if (students == NULL)
        {
            report_alloc_error((student_count == 0 ? 1 : student_count) * sizeof(struct Student));
        }
        for (int i = 0; i < student_count; i++) // Reads all student info
        {
            struct Student *student = &students[i];
            fread(student, sizeof(struct Student), 1, students_datafile_ptr);

            int club_membership_count = 0;
            fread(&club_membership_count, sizeof(int), 1, students_datafile_ptr);
            student->club_memberships = NULL;
            for (int j = 0; j < club_membership_count; j++)
            {
                int club_id = 0;
                fread(&club_id, sizeof(club_id), 1, students_datafile_ptr);
                insert_after_last_node(&student->club_memberships, club_id);
            }
        }
    }

    // Closes the datafiles
    fclose(core_datafile_ptr);
    fclose(accounting_datafile_ptr);
    fclose(clubs_datafile_ptr);
    fclose(meetings_datafile_ptr);
    fclose(students_datafile_ptr);
}

// first_time_setup is called when the program is run for the first time.
// It prompts the user to set an administrator password and the name of the school.
void first_time_setup()
{
    // admin_password and admin_password_confirmation are used to ensure that the user enters the same password twice.
    while (true)
    {
        print_greeting();
        printf("You must set an administrator password which contains at least 8 characters. You can change your password after setting it later.\n\n");
        printf("Set Administrator Password: ");
        admin_password = read_variable_length_input();
        if (strlen(admin_password) < MIN_PASSWORD_LENGTH)
        {
            free(admin_password);
            continue;
        }

        print_greeting();
        printf("You must set an administrator password which contains at least 8 characters. You can change your password after setting it later.\n\n");
        printf("Set Administrator Password: ");
        for (int i = 0; i < strlen(admin_password); i++) // Prints asterisks instead of the actual password
        {
            printf("*");
        }
        printf("\n");

        printf("Confirm Administrator Password: ");
        char *admin_password_confirmation = read_variable_length_input();
        if (strcmp(admin_password, admin_password_confirmation) != 0)
        {
            free(admin_password);
            free(admin_password_confirmation);
        }
        else
        {
            free(admin_password_confirmation);
            break;
        }
    }

    print_greeting();
    printf("Administrator password successfully set. Press any key to continue.");
    _getch();

    while (strcmp(school_name, "") == 0)
    {
        print_greeting();
        printf("Please enter the name of your school. (Max %d characters)\n\n", MAX_SCHOOL_NAME_LENGTH);

        char *chosen_school_name = read_variable_length_input();
        if (strlen(chosen_school_name) > 0 && strlen(chosen_school_name) <= MAX_SCHOOL_NAME_LENGTH)
        {
            strcpy(school_name, chosen_school_name);
        }
        free(chosen_school_name);
    }

    print_greeting();
    printf("School name successfully set to '%s'. Press any key to continue to complete setup and continue to the main menu.", school_name);
    _getch();

    save_data_to_file();
}

// change_school_name lets the administrator change the name of the school.
void change_school_name()
{
    char prev_school_name[MAX_SCHOOL_NAME_LENGTH] = "";
    strcpy(prev_school_name, school_name);

    while (true)
    {
        print_greeting();
        printf("Please enter the new name of your school. (Max %d characters)\n\n", MAX_SCHOOL_NAME_LENGTH);
        char *new_school_name = read_variable_length_input();
        if (strlen(new_school_name) > 0 && strlen(new_school_name) <= MAX_SCHOOL_NAME_LENGTH)
        {
            strcpy(school_name, new_school_name);
            save_data_to_file();
            free(new_school_name);
            break;
        }
        free(new_school_name);
    }

    print_greeting();
    printf("School name successfully set to '%s' (previously '%s'). ", school_name, prev_school_name);
    prompt_return("the main menu");
}
