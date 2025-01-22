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

int last_student_id = 0;

struct Student students[MAX_STUDENTS] = {{0}};

void manage_students()
{
    bool is_authorized = prompt_authorization("Viewing student information", admin_password, ADMIN, "the main menu");
    if (!is_authorized)
    {
        return;
    }

    if (student_count == 0)
    {
        print_greeting();
        printf("There are no students registered. Press any key to return to the previous menu.");
        _getch();
        return;
    }

    while (true)
    {
        print_greeting();
        printf("MANAGE STUDENTS\n\n");
        printf("Type in the number indicated in brackets preceding the name and then press [Enter] to select the student. Alternatively, enter \"0\" to return to the main menu.\n\n");
        for (int i = 0; i < student_count; i++)
        {
            struct Student student = students[i];
            printf("[%d] [%s]\n", student.id, student.name);
        }

        struct Student student = {0};
        int student_pos = 0;
        while (student.id == 0)
        {
            char inputted_student_id[5];
            fgets(inputted_student_id, sizeof(inputted_student_id), stdin);
            if (strcmp(inputted_student_id, "0\n") == 0)
            {
                break;
            }

            student_pos = atoi(inputted_student_id);
            if (student_pos <= last_student_id && student_pos != 0)
            {
                for (int i = 0; i < student_count; i++)
                {
                    if (students[i].id == student_pos)
                    {
                        student = students[i];
                        break;
                    }
                }
            }
        }
        if (student_pos == 0)
        {
            break;
        }

        char choice = '\0';
        while (choice != BACKSPACE_ASCII_CODE)
        {
            print_greeting();
            printf("MANAGE STUDENT '%s'\n\n", student.name);
            printf("Press the key (indicated in brackets) corresponding to one of the options presented below to continue.\n\n");

            printf("[1] View Information\n");
            printf("[2] Update Information\n");
            printf("[3] Delete\n");
            printf("[Backspace] Return\n");

            choice = _getch();
            switch (choice)
            {
            case '1':
                view_student_info(student);
                break;

            case '2':
                break;

            case '3':
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
    printf("> Registered At: %s", ctime(&student.registered_at));
    printf("> Club Memberships: ");
    if (student.club_memberships[0] == 0)
    {
        printf("None");
    }
    else
    {
        for (int i = 0; i < sizeof(student.club_memberships); i++)
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

    printf("Press [Backspace] at any time to return to the club menu.");
    char user_input = 0;
    while (user_input != BACKSPACE_ASCII_CODE)
    {
        user_input = _getch();
    };
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
        printf("Name: ");
        fgets(student.name, sizeof(student.name), stdin);
        student.name[strlen(student.name) - 1] = '\0';
        if (strlen(student.name) > 0)
        {
            break;
        }
    }
    while (true)
    {
        print_greeting();
        printf("Name: %s\n", student.name);
        printf("Class: ");

        fgets(student.class, sizeof(student.class), stdin);
        student.class[strlen(student.class) - 1] = '\0';
        if (strlen(student.class) > 0)
        {
            break;
        }
    }

    print_greeting();
    printf("Confirm registration of student '%s'? (y/n) ", student.name);
    char confirmation = '\0';
    scanf("%c", &confirmation);
    printf("\n");
    if (tolower(confirmation) != 'y')
    {
        clear_console();
        printf("Registration of student '%s' cancelled. ", student.name);
        return prompt_return_to_main_menu();
    }
    student.is_active = true;
    student.registered_at = time(NULL);
    student.id = ++last_student_id;
    students[student.id - 1] = student;
    student_count++;
    save_data_to_file();

    print_greeting();
    printf("Student successfully registered.\n");
    printf("> ID: %d\n", student.id);
    printf("> Name: %s\n", student.name);
    printf("> Class: %s\n", student.class);
    printf("> Activity Status: Active\n");
    printf("> Registered At: %s", ctime(&student.registered_at));
    printf("> Club Memberships: None\n\n");
    printf("Press any key at any time to return to the previous menu.");
    _getch();
}
