#include <conio.h>
#include <math.h>
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include "auth.h"
#include "core.h"
#include "clubs.h"
#include "menu_system.h"
#include "students.h"

void clear_console()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void print_greeting()
{
    clear_console();

    // Prints ASCII art generated from https://patorjk.com/software/taag
    printf("+--------------------------------------------------------------------+\n");
    printf("|    ________      __       __  ___                                  |\n");
    printf("|   / ____/ /_  __/ /_     /  |/  /___ _____  ____ _____ ____  _____ |\n");
    printf("|  / /   / / / / / __ \\   / /|_/ / __ `/ __ \\/ __ `/ __ `/ _ \\/ ___/ |\n");
    printf("| / /___/ / /_/ / /_/ /  / /  / / /_/ / / / / /_/ / /_/ /  __/ /     |\n");
    printf("| \\____/_/\\__,_/_.___/  /_/  /_/\\__,_/_/ /_/\\__,_/\\__, /\\___/_/      |\n");
    printf("|                                                /____/              |\n");
    printf("+--------------------------------------------------------------------+");

    // Capitalizes the school name and centers it in the greeting
    if (strcmp(school_name, "") != 0)
    {
        printf("\n|");
        for (int i = 0; i < floor((double)(strlen("--------------------------------------------------------------------") - strlen(school_name)) / 2); i++) // Left padding
        {
            printf(" ");
        }
        for (int i = 0; i < strlen(school_name); i++)
        {
            printf("%c", toupper(school_name[i]));
        }
        for (int i = 0; i < ceil((double)(strlen("--------------------------------------------------------------------") - strlen(school_name)) / 2); i++) // Right padding
        {
            printf(" ");
        }
        printf("|\n");
        printf("+--------------------------------------------------------------------+");
    }
    printf("\n\n");
}

// read_variable_length_input reads input from the user until the user presses [Enter] (indicated by a newline character).
// It is used when the input is of an unknown/arbitrary length.
char *read_variable_length_input()
{
    char *buffer = malloc(sizeof(char)); // Allocates an initial buffer with a size of 1 char
    if (buffer == NULL)
    {
        printf("Allocation of %lld bytes of memory failed", sizeof(char));
        exit(1);
    }

    size_t buffer_size = 0;     // Stores the current size of the buffer
    size_t buffer_capacity = 1; // Stores the current capacity (max size) of the buffer
    char input = '\0';
    while ((input = getchar()) != '\n' && input != EOF) // Continues reading input until the user presses [Enter] (indicated by a newline character)
    {
        if (buffer_size + 1 >= buffer_capacity) // If buffer_size + 1 (corresponding to the size of the buffer after the character has been added) is greater than or equal to the capacity of the buffer
        {
            buffer_capacity += sizeof(char);           // Increments the capacity of the buffer
            buffer = realloc(buffer, buffer_capacity); // Reallocates the buffer with the new size
            if (buffer == NULL)
            {
                printf("Reallocation of %lld bytes of memory failed", buffer_capacity);
                exit(1);
            }
        }
        buffer[buffer_size++] = input;
    }
    buffer[buffer_size] = '\0';

    return buffer; // Returns the buffer containing the user's input
    // The buffer (dynamically allocated memory) must be manually freed by the caller of this function to prevent memory overflow.
}

// read_fixed_length_input reads a fixed length of input from the user until the user presses [Enter] (indicated by a newline character).
char *read_fixed_length_input(int size)
{
    char *buffer = malloc((size + 1) * sizeof(char)); // Dynamically allocate size + 1 for the null terminator
    if (buffer == NULL)
    {
        printf("Allocation of %lld bytes of memory failed", (size + 1) * sizeof(char));
        exit(1);
    }

    fgets(buffer, size + 1, stdin);
    buffer[strlen(buffer) - 1] = '\0'; // fgets sets the last character in the string to be '\n' which should be replaced with a null terminator '\0'.

    return buffer; // Returns the buffer containing the user's input
    // The buffer (dynamically allocated memory) must be manually freed by the caller of this function to prevent memory overflow.
}

void show_main_menu()
{
    char choice = '\0';
    while (choice != 'q')
    {
        print_greeting();
        printf("Press the key (indicated in brackets) corresponding to one of the options presented below to continue.\n\n");
        printf("[1] Manage Clubs\n");
        printf("[2] Administrator Panel\n");
        printf("[Q] Quit Program");

        choice = _getch();
        switch (choice)
        {
        case '1':
            manage_clubs();
            break;

        case '2':
            show_admin_panel();
            break;
        }
    }

    printf("\n\nQuitting program...");
}

void show_admin_panel()
{
    bool is_authorized = prompt_authorization("Accessing the administrator panel", admin_password, ADMIN, "the main menu");
    if (!is_authorized)
    {
        return;
    }

    char choice = '\0';
    while (choice != 'r')
    {
        print_greeting();
        printf("ADMINISTRATOR PANEL\n\n");
        printf("Press the key (indicated in brackets) corresponding to one of the options presented below to continue.\n\n");
        printf("[1] Register New Club\n");
        printf("[2] Register New Student\n");
        printf("[3] Manage Students\n");
        printf("[4] Change Administrator Password\n");
        printf("[5] Change School Name\n");
        printf("[R] Return");

        choice = _getch();
        switch (choice)
        {
        case '1':
            register_club();
            break;
        case '2':
            register_student();
            break;
        case '3':
            manage_students();
            break;
        case '4':
            change_admin_password();
            break;
        case '5':
            change_school_name();
            break;
        }
    }
}

// prompt_return prompts the user to press [R] to return to a specific location.
void prompt_return(char *to)
{
    printf("Press [R] to return to %s.", to);
    char user_input = '\0';
    while (user_input != 'r')
    {
        user_input = _getch();
    };
}

// print_table prints a table with the specified number of columns and rows, column widths, column headings, contents, and footer rows.
// It ensures that the contents are properly formatted with adequate padding, spacing and alignment.
void print_table(int total_columns, int total_rows, int *column_widths, int total_footer_rows, char *column_headings[], char ***contents, char *footer_rows[])
{
    int table_width = 2; // table_width is initialized to 2 to account for the left border and space before the first column.
    for (int i = 0; i < total_columns; i++)
    {
        if (i == total_columns - 1)
        {
            table_width += column_widths[i] + 2; // 2 is added to account for the right border and space after the last column.
        }
        else
        {
            table_width += column_widths[i] + 3; // 3 is added to account for the column separators and spaces between columns.
        }
    }

    // Prints the top border of the table
    printf("+");
    for (int j = 0; j < table_width - 2; j++) // -2 to account for the left and right borders
    {
        printf("-");
    }
    printf("+\n");

    for (int i = 0; i < total_columns; i++) // Iterates through and prints every column
    {
        if (i == 0) // If it is the first column, print the left border.
        {
            printf("| ");
        }

        for (int j = 0; j < floor((double)(column_widths[i] - strlen(column_headings[i])) / 2); j++) // Prints the right padding required to center the column heading
        {
            printf(" ");
        }
        printf(column_headings[i]);
        for (int j = 0; j < ceil((double)(column_widths[i] - strlen(column_headings[i])) / 2); j++) // Prints the left padding required to center the column heading
        {
            printf(" ");
        }

        if (i == total_columns - 1) // If it is the last column, print the right border and a newline.
        {
            printf(" |\n");
        }
        else // If it is not the last column, print the column separator.
        {
            printf(" | ");
        }
    }

    // Prints the separator between the column headings and the contents
    printf("|");
    for (int j = 0; j < table_width - 2; j++)
    {
        printf("-");
    }
    printf("|\n");

    for (int i = 0; i < total_rows; i++) // Iterates through and prints the content of every row
    {
        for (int j = 0; j < total_columns; j++)
        {
            if (j == 0) // If it is the first column, print the left border.
            {
                printf("| ");
            }

            for (int k = 0; k < floor((double)(column_widths[j] - strlen(contents[i][j])) / 2); k++) // Prints the right padding required to center the content
            {
                printf(" ");
            }
            printf("%s", contents[i][j]);
            for (int k = 0; k < ceil((double)(column_widths[j] - strlen(contents[i][j])) / 2); k++) // Prints the left padding required to center the content
            {
                printf(" ");
            }

            if (j == total_columns - 1) // If it is the last column, print the right border and a newline.
            {
                printf(" |\n");
            }
            else // If it is not the last column, print the column separator.
            {
                printf(" | ");
            }
        }

        if (i < total_rows - 1 || total_footer_rows > 0) // If it is not the last row or there are footer rows, print the separator between rows.
        {
            printf("|");
            for (int j = 0; j < table_width - 2; j++)
            {
                printf("-");
            }
            printf("|\n");
        }
    }

    for (int i = 0; i < total_footer_rows; i++) // Iterates through and prints every footer row
    {
        printf("| ");
        for (int j = 0; j < table_width - strlen(footer_rows[i]) - 4; j++)
        {
            printf(" ");
        }
        printf("%s", footer_rows[i]);
        printf(" |\n");
    }

    // Prints the bottom border of the table
    printf("+");
    for (int j = 0; j < table_width - 2; j++)
    {
        printf("-");
    }
    printf("+");
}
