#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include "accounting.h"
#include "auth.h"
#include "core.h"
#include "menu_system.h"
#include "clubs.h"
#include "students.h"

const char *DATA_FILENAME = "data.bin";

char school_name[MAX_SCHOOL_NAME_LENGTH] = "";

void save_data_to_file()
{
    FILE *dataFilePtr = fopen(DATA_FILENAME, "wb");
    char decision_on_err;
    while (dataFilePtr == NULL)
    {
        printf("Error in saving data to '%s'. Press [Q] to quit without saving data (not recommended) or any other key to try again.\n", DATA_FILENAME);
        decision_on_err = _getch();
        if (tolower(decision_on_err) == 'q')
        {
            printf("Quitting program...");
            exit(1);
        }
        else
        {
            clear_console();
            dataFilePtr = fopen(DATA_FILENAME, "wb");
        }
    }

    fwrite(&school_name, sizeof(char), MAX_SCHOOL_NAME_LENGTH, dataFilePtr);

    int admin_password_length = strlen(admin_password) + 1;
    fwrite(&admin_password_length, sizeof(int), 1, dataFilePtr);
    fwrite(admin_password, sizeof(char), admin_password_length, dataFilePtr);

    fwrite(&club_count, sizeof(int), 1, dataFilePtr);
    fwrite(&prev_club_id, sizeof(int), 1, dataFilePtr);
    for (int i = 0; i < club_count; i++)
    {
        fwrite(&clubs[i], sizeof(struct Club), 1, dataFilePtr);
        fwrite(clubs[i].password, sizeof(char), clubs[i].password_length, dataFilePtr);
    }

    for (int i = 0; i < transaction_count; i++)
    {
        fwrite(&transactions[i], sizeof(struct Transaction), 1, dataFilePtr);
    }

    for (int i = 0; i < meeting_count; i++)
    {
        fwrite(&meetings[i], sizeof(struct Meeting), 1, dataFilePtr);
    }

    fwrite(&student_count, sizeof(int), 1, dataFilePtr);
    fwrite(&prev_student_id, sizeof(int), 1, dataFilePtr);
    for (int i = 0; i < student_count; i++)
    {
        fwrite(&students[i], sizeof(struct Student), 1, dataFilePtr);
    }

    fclose(dataFilePtr);
}

void load_data_from_file()
{
    FILE *dataFilePtr = fopen(DATA_FILENAME, "rb");
    if (dataFilePtr == NULL)
    {
        return;
    }
    else
    {
        fread(&school_name, sizeof(char), MAX_SCHOOL_NAME_LENGTH, dataFilePtr);

        int admin_password_length = 0;
        fread(&admin_password_length, sizeof(int), 1, dataFilePtr);
        admin_password = malloc(admin_password_length);
        fread(admin_password, sizeof(char), admin_password_length, dataFilePtr);

        fread(&club_count, sizeof(int), 1, dataFilePtr);
        fread(&prev_club_id, sizeof(int), 1, dataFilePtr);
        for (int i = 0; i < club_count; i++)
        {
            fread(&clubs[i], sizeof(struct Club), 1, dataFilePtr);
            clubs[i].password = malloc(clubs[i].password_length * sizeof(char));
            fread(clubs[i].password, sizeof(char), clubs[i].password_length, dataFilePtr);

            meeting_count += clubs[i].meeting_count;
            transaction_count += clubs[i].transaction_count;
        }

        transactions = malloc((transaction_count == 0 ? 1 : transaction_count) * sizeof(struct Transaction));
        for (int i = 0; i < transaction_count; i++)
        {
            fread(&transactions[i], sizeof(struct Transaction), 1, dataFilePtr);
        }

        meetings = malloc((meeting_count == 0 ? 1 : meeting_count) * sizeof(struct Meeting));
        if (meetings == NULL)
        {
            printf("Allocation of %lld bytes of memory failed", meeting_count * sizeof(struct Meeting));
            exit(1);
        }
        for (int i = 0; i < meeting_count; i++)
        {
            fread(&meetings[i], sizeof(struct Meeting), 1, dataFilePtr);
        }

        fread(&student_count, sizeof(int), 1, dataFilePtr);
        fread(&prev_student_id, sizeof(int), 1, dataFilePtr);
        students = malloc((student_count == 0 ? 1 : student_count) * sizeof(struct Student));
        if (students == NULL)
        {
            printf("Allocation of %lld bytes of memory failed", student_count * sizeof(struct Student));
            exit(1);
        }
        for (int i = 0; i < student_count; i++)
        {
            fread(&students[i], sizeof(struct Student), 1, dataFilePtr);
        }
    }

    fclose(dataFilePtr);
}

void first_time_setup()
{
    char *admin_password_confirmation = NULL;

    while (admin_password == NULL || admin_password_confirmation == NULL)
    {
        print_greeting();
        printf("You must set an administrator password which contains at least 8 characters. You can change your password after setting it later.\n\n");

        if (admin_password == NULL)
        {
            printf("Set Administrator Password: ");
            admin_password = accept_variable_length_input();
            if (strlen(admin_password) < MIN_PASSWORD_LENGTH)
            {
                free(admin_password);
                admin_password = NULL;
            }
        }
        else
        {
            printf("Set Administrator Password: ");
            for (int i = 0; i < strlen(admin_password); i++)
            {
                printf("*");
            }
            printf("\n");

            printf("Confirm Administrator Password: ");
            admin_password_confirmation = accept_variable_length_input();
            if (strcmp(admin_password, admin_password_confirmation) != 0)
            {
                free(admin_password);
                free(admin_password_confirmation);
                admin_password = NULL;
                admin_password_confirmation = NULL;
            }
        }
    }
    free(admin_password_confirmation);

    print_greeting();
    printf("Administrator password successfully set. Press any key to continue.");
    _getch();

    print_greeting();
    printf("Please enter the name of your school. (Input will be truncated to first %d characters)\n\n", MAX_SCHOOL_NAME_LENGTH);
    while (true)
    {
        fgets(school_name, sizeof(school_name) + sizeof(char), stdin);
        school_name[strlen(school_name) - 1] = '\0';
        if (strlen(school_name) > 0 && strlen(school_name) <= MAX_SCHOOL_NAME_LENGTH)
        {
            break;
        }
    }

    print_greeting();
    printf("School name successfully set to '%s'. Press any key to continue to complete setup and continue to the main menu.", school_name);
    _getch();

    save_data_to_file();
}

void change_school_name()
{
    print_greeting();
    printf("Please enter the new name of your school. (Input will be truncated to first %d characters)\n\n", MAX_SCHOOL_NAME_LENGTH);
    char prev_school_name[MAX_SCHOOL_NAME_LENGTH] = "";
    strcpy(prev_school_name, school_name);
    while (true)
    {
        fgets(school_name, sizeof(school_name) + sizeof(char), stdin);
        school_name[strlen(school_name) - 1] = '\0';
        if (strlen(school_name) > 0 && strlen(school_name) <= MAX_SCHOOL_NAME_LENGTH)
        {
            break;
        }
    }
    print_greeting();
    printf("School name successfully set to '%s' (previously '%s'). ", school_name, prev_school_name);
    prompt_return("the main menu");
}
