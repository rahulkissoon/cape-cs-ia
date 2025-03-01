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

// save_data_to_file ensures that data persists across sessions.
// Once the process safely exits, data can be reloaded from the file during the next session.
void save_data_to_file()
{
    FILE *dataFilePtr = fopen(DATA_FILENAME, "wb");
    while (dataFilePtr == NULL)
    {
        printf("Error in saving data to '%s'. Press [Q] to quit without saving data (not recommended) or any other key to try again.\n", DATA_FILENAME);
        char decision_on_err = _getch(); // In the event that the file cannot be opened, the user can choose to quit the program or try again.
        if (tolower(decision_on_err) == 'q')
        {
            printf("Quitting program...");
            exit(1);
        }
        else
        {
            clear_console();
            dataFilePtr = fopen(DATA_FILENAME, "wb"); // Opens the file in write and binary mode
        }
    }

    fwrite(&school_name, sizeof(char), MAX_SCHOOL_NAME_LENGTH, dataFilePtr);

    int admin_password_length = strlen(admin_password) + 1;
    fwrite(&admin_password_length, sizeof(int), 1, dataFilePtr);              // The admin password is of variable length so its length must be recorded.
    fwrite(admin_password, sizeof(char), admin_password_length, dataFilePtr); // Saves the admin password

    fwrite(&club_count, sizeof(int), 1, dataFilePtr);   // Saves the club count
    fwrite(&prev_club_id, sizeof(int), 1, dataFilePtr); // Saves the previous club ID
    for (int i = 0; i < club_count; i++)                // Saves all club info
    {
        fwrite(&clubs[i], sizeof(struct Club), 1, dataFilePtr);
        fwrite(clubs[i].password, sizeof(char), clubs[i].password_length, dataFilePtr);
    }

    for (int i = 0; i < transaction_count; i++) // Saves all transactions
    {
        fwrite(&transactions[i], sizeof(struct Transaction), 1, dataFilePtr);
    }

    for (int i = 0; i < meeting_count; i++) // Saves all club meetings
    {
        fwrite(&meetings[i], sizeof(struct Meeting), 1, dataFilePtr);
    }

    fwrite(&student_count, sizeof(int), 1, dataFilePtr);   // Saves the student count
    fwrite(&prev_student_id, sizeof(int), 1, dataFilePtr); // Saves the previous student ID
    for (int i = 0; i < student_count; i++)                // Saves all student info
    {
        fwrite(&students[i], sizeof(struct Student), 1, dataFilePtr);
    }

    fclose(dataFilePtr); // Closes the data file
}

// load_data_from_file reads the database, loading all data into the active memory of the program.
void load_data_from_file()
{
    FILE *dataFilePtr = fopen(DATA_FILENAME, "rb"); // Opens the file in read and binary mode
    // If the file does not exist, that means that this is the first time the program is being run.
    // Active memory with default values will be used and stored later.
    if (dataFilePtr == NULL)
    {
        return;
    }
    else
    {
        fread(&school_name, sizeof(char), MAX_SCHOOL_NAME_LENGTH, dataFilePtr); // Reads the school name

        int admin_password_length = 0;
        fread(&admin_password_length, sizeof(int), 1, dataFilePtr);
        admin_password = malloc(admin_password_length * sizeof(char)); // Allocates enough memory based on the length of the admin password
        fread(admin_password, sizeof(char), admin_password_length, dataFilePtr);

        fread(&club_count, sizeof(int), 1, dataFilePtr);   // Reads the club count
        fread(&prev_club_id, sizeof(int), 1, dataFilePtr); // Reads the previous club ID
        for (int i = 0; i < club_count; i++)               // Reads all club info
        {
            fread(&clubs[i], sizeof(struct Club), 1, dataFilePtr);
            clubs[i].password = malloc(clubs[i].password_length * sizeof(char)); // Allocates enough memory based on the length of the club password
            fread(clubs[i].password, sizeof(char), clubs[i].password_length, dataFilePtr);

            meeting_count += clubs[i].meeting_count;         // Increments the global meeting count based on meeting count of the club
            transaction_count += clubs[i].transaction_count; // Increments the global transaction count based on transaction count of the club
        }

        // The reason for the ternary operator is to prevent allocating 0 bytes of memory. This can cause undefined behaviour in C.
        transactions = malloc((transaction_count == 0 ? 1 : transaction_count) * sizeof(struct Transaction));
        for (int i = 0; i < transaction_count; i++) // Reads all transactions
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

        fread(&student_count, sizeof(int), 1, dataFilePtr);   // Reads the student count
        fread(&prev_student_id, sizeof(int), 1, dataFilePtr); // Reads the previous student ID
        // The reason for the ternary operator is to prevent allocating 0 bytes of memory. This can cause undefined behaviour in C.
        students = malloc((student_count == 0 ? 1 : student_count) * sizeof(struct Student));
        if (students == NULL)
        {
            printf("Allocation of %lld bytes of memory failed", student_count * sizeof(struct Student));
            exit(1);
        }
        for (int i = 0; i < student_count; i++) // Reads all student info
        {
            fread(&students[i], sizeof(struct Student), 1, dataFilePtr);
        }
    }

    fclose(dataFilePtr);
}

// first_time_setup is called when the program is run for the first time.
// It prompts the user to set an administrator password and the name of the school.
void first_time_setup()
{
    char *admin_password_confirmation = NULL;

    // admin_password and admin_password_confirmation are used to ensure that the user enters the same password twice.
    // The loop condition is only true when the passwords match and both passwords satisfy the relevant criteria.
    while (admin_password == NULL || admin_password_confirmation == NULL)
    {
        print_greeting();
        printf("You must set an administrator password which contains at least 8 characters. You can change your password after setting it later.\n\n");

        if (admin_password == NULL)
        {
            printf("Set Administrator Password: ");
            admin_password = read_variable_length_input();
            if (strlen(admin_password) < MIN_PASSWORD_LENGTH)
            {
                free(admin_password);
                admin_password = NULL;
            }
        }
        else
        {
            printf("Set Administrator Password: ");
            for (int i = 0; i < strlen(admin_password); i++) // Prints asterisks instead of the actual password
            {
                printf("*");
            }
            printf("\n");

            printf("Confirm Administrator Password: ");
            admin_password_confirmation = read_variable_length_input();
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
    while (strcmp(school_name, "") == 0)
    {
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
    print_greeting();
    printf("Please enter the new name of your school. (Input will be truncated to first %d characters)\n\n", MAX_SCHOOL_NAME_LENGTH);
    char prev_school_name[MAX_SCHOOL_NAME_LENGTH] = "";
    strcpy(prev_school_name, school_name);
    while (true)
    {
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
