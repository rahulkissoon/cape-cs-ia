#include <conio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "auth.h"
#include "clubs.h"
#include "students.h"
#include "database.h"
#include "menu_system.h"

int student_count = 0;
int prev_student_id = 0;

struct Student students[MAX_STUDENTS] = {{0}};

void manage_students()
{
    bool is_authorized = prompt_authorization("Viewing student information", admin_password, ADMIN, "the main menu");
    if (!is_authorized)
    {
        return;
    }

    while (true)
    {
        if (student_count == 0)
        {
            print_greeting();
            printf("There are no students registered. Press any key to return to the previous menu.");
            _getch();
            return;
        }

        struct Student student = {0};
        int student_pos = -1;
        while (student.id == 0)
        {
            print_greeting();
            printf("MANAGE STUDENTS\n\n");
            printf("Type in either the student's full name or ID then press [Enter]. Otherwise, leave the field blank and press [Enter] to return to the main menu.\n\n");

            for (int i = 0; i < student_count; i++)
            {
                struct Student student = students[i];
                printf("[%d] %s\n", student.id, student.name);
            }
            printf("\n");

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
            break;
        }

        char choice = '\0';
        while (choice != 'r')
        {
            print_greeting();
            printf("MANAGE STUDENT '%s'\n\n", student.name);
            printf("Press the key (indicated in brackets) corresponding to one of the options presented below to continue.\n\n");

            printf("[1] View Information\n");
            printf("[2] Update Information\n");
            printf("[3] Delete\n");
            printf("[R] Return\n");

            choice = _getch();
            switch (choice)
            {
            case '1':
                view_student_info(student);
                break;

            case '2':
                break;

            case '3':
                delete_student(student, student_pos);
                choice = 'r';
                break;
            }
        }
    }
}

void view_student_info(struct Student student)
{
    print_greeting();

    printf("VIEW STUDENT INFORMATION\n\n");
    printf("> Name: %s\n", student.name);
    printf("> ID: %d\n", student.id);
    printf("> Email Address: %s\n", student.email_address);
    printf("> Registered At: %s", ctime(&student.registered_at));
    printf("> Club Memberships: ");
    if (student.club_memberships[0] == 0)
    {
        printf("None");
    }
    else
    {
        for (int i = 0; i < sizeof(student.club_memberships) / sizeof(int); i++)
        {
            int club_id = student.club_memberships[i];
            if (club_id == 0)
            {
                break;
            }

            struct Club club;
            for (int j = 0; j < club_count; j++)
            {
                if (clubs[j].id == club_id)
                {
                    club = clubs[j];
                    break;
                }
            }
            printf("%s", club.name);
            if (student.club_memberships[i + 1] != 0)
            {
                printf(", ");
            }
        }
    }
    printf("\n\n");

    prompt_return("the student menu");
}

void register_student()
{
    bool is_authorized = prompt_authorization("Registering a new student", admin_password, ADMIN, "the main menu");
    if (!is_authorized)
    {
        return;
    }

    struct Student student;
    while (true)
    {
        print_greeting();
        printf("REGISTER NEW STUDENT\n\n");
        printf("Name: ");
        fgets(student.name, sizeof(student.name) + sizeof(char), stdin);
        student.name[strlen(student.name) - 1] = '\0';
        if (strlen(student.name) > 0)
        {
            break;
        }
    }
    while (true)
    {
        print_greeting();
        printf("REGISTER NEW STUDENT\n\n");
        printf("Name: %s\n", student.name);
        printf("Class: ");

        fgets(student.class, sizeof(student.class) + sizeof(char), stdin);
        student.class[strlen(student.class) - 1] = '\0';
        if (strlen(student.class) > 0)
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
        printf("Email Address: ");

        fgets(student.email_address, sizeof(student.email_address) + sizeof(char), stdin);
        student.email_address[strlen(student.email_address) - 1] = '\0';
        if (strlen(student.email_address) > 0)
        {
            break;
        }
    }

    print_greeting();
    printf("REGISTER NEW STUDENT\n\n");
    printf("Name: %s\n", student.name);
    printf("Class: %s\n", student.class);
    printf("Email Address: %s\n\n", student.email_address);
    printf("Confirm registration of student? (y/n) ", student.name);
    char confirmation = _getch();
    if (confirmation != 'y')
    {
        print_greeting();
        printf("Registration of student '%s' cancelled. ", student.name);
        return prompt_return("the main menu");
    }
    student.is_active = true;
    student.registered_at = time(NULL);
    student.id = ++prev_student_id;
    students[student_count++] = student;
    save_data_to_file();

    print_greeting();
    printf("New student successfully registered.\n");
    printf("> ID: %d\n", student.id);
    printf("> Name: %s\n", student.name);
    printf("> Class: %s\n", student.class);
    printf("> Email Address: %s\n", student.email_address);
    printf("> Activity Status: Active\n");
    printf("> Registered At: %s", ctime(&student.registered_at));
    printf("> Club Memberships: None\n\n");
    printf("Press any key to return to the previous menu.");
    _getch();
}

void delete_student(struct Student student, int student_pos)
{
    print_greeting();
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
        for (int i = 0; i < sizeof(student.club_memberships) / sizeof(int); i++)
        {
            if (student.club_memberships[i] == 0)
            {
                break;
            }

            struct Club club;
            int club_pos;
            for (int j = 0; j < club_count; j++)
            {
                if (clubs[j].id == student.club_memberships[i])
                {
                    club_pos = j;
                    club = clubs[j];
                    break;
                }
            }

            bool is_student_rep = false;
            for (int j = 0; j < club.student_rep_count; j++)
            {
                if (club.student_rep_ids[j] == student.id)
                {
                    is_student_rep = true;
                    break;
                }
            }

            if (is_student_rep && club.student_rep_count == 1)
            {
                print_greeting();
                printf("Deletion failed because '%s' is currently the only student representative of the club '%s'. Please designate another student representative and try again. ", student.name, club.name);
                return prompt_return("the student menu");
            }

            for (int j = 0; j < club.member_count; j++)
            {
                if (club.member_ids[j] == student.id)
                {
                    if (club.member_count > j + 1)
                    {
                        for (int k = j; k < club.member_count - 1; k++)
                        {
                            clubs[club_pos].member_ids[k] = clubs[club_pos].member_ids[k + 1];
                        }
                    }
                    break;
                }
            }
            clubs[club_pos].member_ids[club.member_count - 1] = 0;
            clubs[club_pos].member_count--;

            for (int j = 0; j < meeting_count; j++)
            {
                struct Meeting meeting = meetings[j];
                bool is_absent = false;
                for (int k = 0; k < meeting.absent_member_count; k++)
                {
                    if (meeting.absent_member_ids[k] == student.id)
                    {
                        is_absent = true;
                        if (meeting.absent_member_count > k + 1)
                        {
                            for (int l = k; l < meeting.absent_member_count - 1; l++)
                            {
                                meetings[j].absent_member_ids[l] = meeting.absent_member_ids[l + 1];
                            }
                        }
                        meetings[j].absent_member_ids[--meetings[j].absent_member_count] = 0;
                        break;
                    }
                }
                if (is_absent)
                {
                    continue;
                }
                for (int k = 0; k < meeting.present_member_count; k++)
                {
                    if (meeting.present_member_ids[k] == student.id)
                    {
                        if (meeting.present_member_count > k + 1)
                        {
                            for (int l = k; l < meeting.present_member_count - 1; l++)
                            {
                                meetings[j].present_member_ids[l] = meeting.present_member_ids[l + 1];
                            }
                        }
                        meetings[j].present_member_ids[--meetings[j].present_member_count] = 0;
                        break;
                    }
                }
            }

            for (int j = 0; j < club.student_rep_count; j++)
            {
                if (club.student_rep_ids[j] == student.id)
                {
                    if (club.student_rep_count > j + 1)
                    {
                        for (int k = j; k < club.student_rep_count - 1; k++)
                        {
                            clubs[club_pos].student_rep_ids[k] = clubs[club_pos].student_rep_ids[k + 1];
                        }
                    }
                    break;
                }
            }
            clubs[club_pos].student_rep_ids[club.student_rep_count - 1] = 0;
            clubs[club_pos].student_rep_count--;
        }

        if (student_count > student_pos + 1)
        {
            for (int i = student_pos; i < student_count; i++)
            {
                students[i] = students[i + 1];
            }
        }

        struct Student placeholder_student = {0};
        students[--student_count] = placeholder_student;
        save_data_to_file();

        print_greeting();
        printf("Successfully deleted student '%s'.", student.name);
    }

    prompt_return("the student menu");
}
