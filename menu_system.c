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
    printf("+--------------------------------------------------------------------+\n");
    printf("|    ________      __       __  ___                                  |\n");
    printf("|   / ____/ /_  __/ /_     /  |/  /___ _____  ____ _____ ____  _____ |\n");
    printf("|  / /   / / / / / __ \\   / /|_/ / __ `/ __ \\/ __ `/ __ `/ _ \\/ ___/ |\n");
    printf("| / /___/ / /_/ / /_/ /  / /  / / /_/ / / / / /_/ / /_/ /  __/ /     |\n");
    printf("| \\____/_/\\__,_/_.___/  /_/  /_/\\__,_/_/ /_/\\__,_/\\__, /\\___/_/      |\n");
    printf("|                                                /____/              |\n");
    printf("+--------------------------------------------------------------------+");

    if (strcmp(school_name, "") != 0)
    {
        printf("\n|");
        for (int i = 0; i < floor((double)(strlen("--------------------------------------------------------------------") - strlen(school_name)) / 2); i++)
        {
            printf(" ");
        }
        for (int i = 0; i < strlen(school_name); i++)
        {
            printf("%c", toupper(school_name[i]));
        }
        for (int i = 0; i < ceil((double)(strlen("--------------------------------------------------------------------") - strlen(school_name)) / 2); i++)
        {
            printf(" ");
        }
        printf("|\n");
        printf("+--------------------------------------------------------------------+");
    }
    printf("\n\n");
}

char *accept_variable_length_input()
{
    char *buffer = malloc(sizeof(char));
    if (buffer == NULL)
    {
        printf("Allocation of %lld bytes of memory failed", sizeof(char));
        exit(1);
    }

    size_t buffer_size = 0;
    size_t buffer_capacity = 1;
    char input = '\0';
    while ((input = getchar()) != '\n' && input != EOF)
    {
        if (buffer_size + 1 >= buffer_capacity)
        {
            buffer_capacity++;
            buffer = realloc(buffer, buffer_capacity);
            if (buffer == NULL)
            {
                printf("Reallocation of %lld bytes of memory failed", buffer_capacity);
                exit(1);
            }
        }
        buffer[buffer_size++] = input;
    }
    buffer[buffer_size] = '\0';

    return buffer;
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

void prompt_return(char *to)
{
    printf("Press [R] to return to %s.", to);
    char user_input = '\0';
    while (user_input != 'r')
    {
        user_input = _getch();
    };
}

void print_table(int total_columns, int total_rows, int *column_widths, int total_footer_rows, char *column_headings[], char ***rows, char *footer_rows[])
{
    int table_width = 2;
    for (int i = 0; i < total_columns; i++)
    {
        if (i == total_columns - 1)
        {
            table_width += column_widths[i];
        }
        else
        {
            table_width += column_widths[i] + 3;
        }
    }
    table_width += 2;

    printf("+");
    for (int j = 0; j < table_width - 2; j++)
    {
        printf("-");
    }
    printf("+\n");
    for (int i = 0; i < total_columns; i++)
    {
        if (i == 0)
        {
            printf("| ");
        }

        for (int j = 0; j < floor((double)(column_widths[i] - strlen(column_headings[i])) / 2); j++)
        {
            printf(" ");
        }
        printf(column_headings[i]);
        for (int j = 0; j < ceil((double)(column_widths[i] - strlen(column_headings[i])) / 2); j++)
        {
            printf(" ");
        }

        if (i == total_columns - 1)
        {
            printf(" |\n");
        }
        else
        {
            printf(" | ");
        }
    }
    printf("|");
    for (int j = 0; j < table_width - 2; j++)
    {
        printf("-");
    }
    printf("|\n");

    for (int i = 0; i < total_rows; i++)
    {
        for (int j = 0; j < total_columns; j++)
        {
            if (j == 0)
            {
                printf("| ");
            }

            for (int k = 0; k < floor((double)(column_widths[j] - strlen(rows[i][j])) / 2); k++)
            {
                printf(" ");
            }
            printf("%s", rows[i][j]);
            for (int k = 0; k < ceil((double)(column_widths[j] - strlen(rows[i][j])) / 2); k++)
            {
                printf(" ");
            }

            if (j == total_columns - 1)
            {
                printf(" |\n");
            }
            else
            {
                printf(" | ");
            }
        }

        if (i < total_rows - 1 || total_footer_rows > 0)
        {
            printf("|");
            for (int j = 0; j < table_width - 2; j++)
            {
                printf("-");
            }
            printf("|\n");
        }
    }

    for (int i = 0; i < total_footer_rows; i++)
    {
        printf("| ");
        for (int j = 0; j < table_width - strlen(footer_rows[i]) - 4; j++)
        {
            printf(" ");
        }
        printf("%s", footer_rows[i]);
        printf(" |\n");
    }

    printf("+");
    for (int j = 0; j < table_width - 2; j++)
    {
        printf("-");
    }
    printf("+");
}
