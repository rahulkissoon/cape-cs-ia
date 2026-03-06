#include <conio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "auth.h"
#include "clubs.h"
#include "core.h"
#include "linked_lists.h"
#include "menu_system.h"
#include "students.h"

// These count variables are initialized to be 0, will be read later from the data file. These are global variables.
int student_count = 0;
int prev_student_id = 0;

struct Student *students = NULL; // Initializes students to be NULL, will be populated later. students is a global variable.

// manage_students allows the user and the administrator to view, update the info of, and delete students.
void manage_students()
{
    while (true)
    {
        if (student_count == 0) // Returns to the main menu if there are no students registered.
        {
            print_greeting();
            printf("There are no students registered. Register a student through the administrator panel. ");
            return prompt_return("the main menu");
        }

        struct Student student = {0};
        int student_pos = -1; // Initializes the student position to -1 to escape the loop
        while (student.id == 0)
        {
            print_greeting();
            printf("MANAGE STUDENTS\n\n");
            printf("Type in either the student's full name or ID then press [Enter]. Otherwise, leave the field blank and press [Enter] to return to the main menu.\n\n");

            for (int i = 0; i < student_count; i++) // Prints all students
            {
                struct Student student = students[i];
                printf("[%d] %s\n", student.id, student.name);
            }
            printf("\n");

            char *student_lookup_query = read_variable_length_input();
            if (strcmp(student_lookup_query, "") == 0) // Exits if nothing was entered
            {
                free(student_lookup_query);
                break;
            }

            for (int i = 0; i < student_count; i++)
            {
                if (students[i].id == atoi(student_lookup_query)) // If the student's ID matches the lookup query, the student is found.
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
                if (strcmp(student_name, student_lookup_query) == 0) // If the student's name matches the lookup query (case insensitive), the student is found.
                {
                    student = students[i];
                    student_pos = i;
                    break;
                }
            }
            free(student_lookup_query);
        }
        if (student_pos == -1) // If student_pos is still -1 (implying that the user chose to exit), the loop is broken.
        {
            break;
        }

        char choice = '\0';
        while (choice != 'r')
        {
            student = students[student_pos]; // Refreshes student info in case any changes were made
            print_greeting();
            printf("MANAGE STUDENT '%s'\n\n", student.name);
            printf("Press the key (indicated in brackets) corresponding to one of the options presented below to continue.\n\n");

            printf("[1] View Information\n");
            printf("[2] Update Information\n");
            printf("[3] Delete\n");
            printf("[R] Return\n");

            choice = tolower(_getch());
            switch (choice)
            {
            case '1':
                view_student_info(student);
                break;

            case '2':
                update_student_info(student, student_pos);
                break;

            case '3':
                delete_student(student, student_pos);
                if (students[student_pos].id != student.id) // If the ID of the student at the same position has changed, the student has been deleted.
                {
                    choice = 'r';
                }
                break;
            }
        }
    }
}

// view_student_info displays the information of a student, including their full name, ID, class, email address, registration date, and active club memberships.
void view_student_info(struct Student student)
{
    print_greeting();

    printf("VIEW STUDENT INFORMATION\n\n");
    printf("> Name: %s\n", student.name);
    printf("> ID: %d\n", student.id);
    printf("> Class: %s\n", student.class);
    printf("> Email Address: %s\n", student.email_address);
    printf("> Registered At: %s\n", format_time_t(student.registered_at));
    printf("> Club Memberships: ");
    if (student.club_memberships == NULL)
    {
        printf("None");
    }
    else
    {
        struct Node *curr_club_membership = student.club_memberships;
        while (curr_club_membership != NULL)
        {
            struct Club club = {0};
            for (int j = 0; j < club_count; j++)
            {
                if (clubs[j].id == curr_club_membership->data)
                {
                    club = clubs[j];
                    break;
                }
            }
            printf("%s", club.name);
            if (curr_club_membership->next != NULL) // If this club membership is not the last, print a comma.
            {
                printf(", ");
            }

            curr_club_membership = curr_club_membership->next;
        }
    }
    printf("\n\n");

    prompt_return("the student menu");
}

// update_student_info allows administrators to update the information of a student, including their full name, class, and email address.
void update_student_info(struct Student student, int student_pos)
{
    char choice = '\0';
    while (choice != 'r')
    {
        print_greeting();
        printf("UPDATE STUDENT INFORMATION\n\n");
        printf("Press the key (indicated in brackets) corresponding to one of the options presented below to continue.\n");
        printf("[1] Name\n");
        printf("[2] Class\n");
        printf("[3] Email Address\n");
        printf("[R] Return\n");

        choice = tolower(_getch());

        switch (choice)
        {
        case '1': // Updates the student's name
            print_greeting();

            printf("After typing the new name for %s (max %d characters), press [Enter] to confirm.\n\n", student.name, MAX_STUDENT_NAME_LENGTH);
            char old_name[MAX_STUDENT_NAME_LENGTH] = "";
            while (true)
            {
                char *new_name = read_variable_length_input();
                if (strlen(new_name) > 0 && strlen(new_name) <= MAX_STUDENT_NAME_LENGTH)
                {
                    strcpy(old_name, student.name);
                    strcpy(students[student_pos].name, new_name);
                    save_data_to_file();
                    free(new_name);
                    break;
                }
                free(new_name); // Frees the old pointer to new_name
            }
            print_greeting();
            printf("Successfully updated the name of %s (previously '%s'). ", students[student_pos].name, old_name);
            break;

        case '2': // Updates the student's class
            print_greeting();

            printf("After typing the new class for %s (max %d characters), press [Enter] to confirm.\n\n", student.name, MAX_CLASS_NAME_LENGTH);
            char old_class[MAX_CLASS_NAME_LENGTH] = "";
            while (true)
            {
                char *new_class = read_variable_length_input();
                if (strlen(new_class) > 0 && strlen(new_class) <= MAX_CLASS_NAME_LENGTH)
                {
                    strcpy(old_class, student.class);
                    strcpy(students[student_pos].class, new_class);
                    save_data_to_file();
                    free(new_class);
                    break;
                }
                free(new_class); // Frees the old pointer to new_class
            }
            print_greeting();
            printf("Successfully updated the %s's class to %s (previously '%s'). ", student.name, students[student_pos].class, old_class);
            break;

        case '3': // Updates the student's email address
            print_greeting();

            printf("After typing the new email address for %s (max %d characters), press [Enter] to confirm.\n\n", student.name, MAX_EMAIL_ADDRESS_LENGTH);
            char old_email_address[MAX_EMAIL_ADDRESS_LENGTH] = "";
            while (true)
            {
                char *new_email_address = read_variable_length_input();
                if (strlen(new_email_address) > 0 && strlen(new_email_address) <= MAX_EMAIL_ADDRESS_LENGTH)
                {
                    strcpy(old_email_address, student.email_address);
                    strcpy(students[student_pos].email_address, new_email_address);
                    save_data_to_file();
                    free(new_email_address);
                    break;
                }
                free(new_email_address); // Frees the old pointer to new_email_address
            }
            print_greeting();
            printf("Successfully updated the %s's email address to %s (previously %s). ", student.name, students[student_pos].email_address, old_email_address);
            break;
        }

        if (choice == '1' || choice == '2' || choice == '3') // If the user entered a valid choice, prompt them to return to the student menu.
        {
            prompt_return("the student menu");
            break;
        }
    }
}

// register_student allows the administrator to register a new student.
void register_student()
{
    struct Student student = {0}; // Initializes a new, empty student struct
    while (true)
    {
        print_greeting();
        printf("REGISTER NEW STUDENT\n\n");
        printf("Name (max %d characters): ", MAX_STUDENT_NAME_LENGTH);
        char *student_name = read_variable_length_input();
        strcpy(student.name, student_name);
        free(student_name);
        if (strlen(student.name) > 0 && strlen(student.name) <= MAX_STUDENT_NAME_LENGTH) // If the student's name is not empty, break out of the loop.
        {
            break;
        }
    }
    while (true)
    {
        print_greeting();
        printf("REGISTER NEW STUDENT\n\n");
        printf("Name: %s\n", student.name);
        printf("Class (max %d characters): ", MAX_CLASS_NAME_LENGTH);

        char *student_class = read_variable_length_input();
        strcpy(student.class, student_class);
        free(student_class);
        if (strlen(student.class) > 0 && strlen(student.class) <= MAX_CLASS_NAME_LENGTH) // If the student's class is not empty, break out of the loop.
        {
            break;
        }
    }
    while (true)
    {
        print_greeting();
        printf("REGISTER NEW STUDENT\n\n");
        printf("Name: %s\n", student.name);
        printf("Class: %s\n", student.class);
        printf("Email Address (max %d characters): ", MAX_EMAIL_ADDRESS_LENGTH);

        char *student_email_address = read_variable_length_input();
        strcpy(student.email_address, student_email_address);
        free(student_email_address);
        if (strlen(student.email_address) > 0 && strlen(student.email_address) <= MAX_EMAIL_ADDRESS_LENGTH) // If the student's email address is not empty and satisfies the maximum length condition, break out of the loop.
        {
            break;
        }
    }

    print_greeting();
    printf("REGISTER NEW STUDENT\n\n");
    printf("Name: %s\n", student.name);
    printf("Class: %s\n", student.class);
    printf("Email Address: %s\n\n", student.email_address);
    printf("Confirm registration of student? (y/n)", student.name);
    char confirmation = _getch();
    if (confirmation != 'y')
    {
        print_greeting();
        printf("Registration of student '%s' cancelled. ", student.name);
        return prompt_return("the main menu");
    }
    student.registered_at = time(NULL); // Sets the student's registration date to the current time
    student.id = ++prev_student_id;     // Increments the previous student ID global variable and sets the student ID
    student.club_memberships = NULL;
    students = realloc(students, (student_count + 1) * sizeof(struct Student)); // Reallocates memory for the students array to accommodate the new student
    if (students == NULL)
    {
        report_alloc_error((student_count + 1) * sizeof(struct Student));
    }
    students[student_count++] = student; // Saves the new student
    save_data_to_file();

    print_greeting();
    printf("New student successfully registered.\n");
    printf("> ID: %d\n", student.id);
    printf("> Name: %s\n", student.name);
    printf("> Class: %s\n", student.class);
    printf("> Email Address: %s\n", student.email_address);
    printf("> Registered At: %s\n", format_time_t(student.registered_at));
    printf("> Club Memberships: None\n\n");
    prompt_return("the main menu");
}

// delete_student allows the administrator to delete a student.
// This function also handles the removal of the student from any clubs they are a member of, as well as the removal of the student from any meetings they are scheduled to attend.
void delete_student(struct Student student, int student_pos)
{
    print_greeting();
    // Prompts the user to confirm whether they want to delete the student permanently
    printf("Please confirm whether you would like to permanently delete student '%s'. (y/n)\n", student.name);
    printf("Warning: This is an irreversible action. All data associated with student '%s' will be irrecoverably lost.\n", student.name);
    char confirmation = _getch();
    if (confirmation != 'y')
    {
        print_greeting();
        printf("Deletion of student '%s' cancelled.", student.name);
    }
    else
    {
        struct Node *curr_club_membership = student.club_memberships;
        while (curr_club_membership != NULL)
        {
            struct Club club = {0};
            int club_pos;
            for (int i = 0; i < club_count; i++) // Iterates through the clubs to find the corresponding club
            {
                if (clubs[i].id == curr_club_membership->data)
                {
                    club_pos = i;
                    club = clubs[i];
                    break;
                }
            }

            bool is_student_rep = false; // Indicates whether the student is a student representative of the club
            for (int i = 0; i < club.student_rep_count; i++)
            {
                if (does_node_exist(club.student_rep_ids, student.id))
                {
                    is_student_rep = true;
                    break;
                }
            }

            if (is_student_rep && club.student_rep_count == 1) // Deletion may only occur if the student is not representing any clubs.
            {
                print_greeting();
                printf("Deletion failed because '%s' is currently the only student representative of the club '%s'. Please designate another student representative and try again. ", student.name, club.name);
                return prompt_return("the student menu");
            }

            delete_node_by_data(&club.member_ids, student.id); // Deletes the student's ID from the club's member IDs
            clubs[club_pos].member_count--;                    // Decrements the club's member count

            for (int i = 0; i < meeting_count; i++)
            {
                struct Meeting meeting = meetings[i];
                if (meeting.club_id != club.id)
                {
                    continue;
                }
                int deleted_absentee = delete_node_by_data(&meetings[i].absent_member_ids, student.id); // Attempts to remove the student's ID from the absent record
                if (deleted_absentee == 1)
                {
                    meetings[i].absent_member_count--; // Decrements the meeting's absent member count
                }
                else
                {
                    meetings[i].present_member_count--;                               // Decrements the meeting's present member count
                    delete_node_by_data(&meetings[i].present_member_ids, student.id); // Removes the student's ID from the present record
                }
            }

            int deleted_student_rep = delete_node_by_data(&clubs[club_pos].student_rep_ids, student.id);
            if (deleted_student_rep == 1)
            {
                clubs[club_pos].student_rep_count--;
            }

            curr_club_membership = curr_club_membership->next;
        }

        if (student_count > student_pos + 1) // If the student is not the last student in the list, shift the following students to the left
        {
            for (int i = student_pos; i < student_count; i++)
            {
                students[i] = students[i + 1];
            }
        }

        students[--student_count] = (struct Student){0};                                                 // Decrements the student count and sets the last student to an empty student struct
        students = realloc(students, (student_count == 0 ? 1 : student_count) * sizeof(struct Student)); // Reallocates memory for the students array to accommodate the new student count
        if (students == NULL)
        {
            report_alloc_error((student_count == 0 ? 1 : student_count) * sizeof(struct Student));
        }
        save_data_to_file();

        print_greeting();
        printf("Successfully deleted student '%s'. ", student.name);
    }

    prompt_return("the student menu");
}
