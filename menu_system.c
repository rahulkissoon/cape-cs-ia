#include <conio.h>
#include <math.h>
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
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
        printf("Allocation of %d bytes of memory failed", sizeof(char));
        exit(1);
    }

    size_t buffer_size = 0;
    size_t buffer_capacity = 1;
    char input;
    while ((input = getchar()) != '\n' && input != EOF)
    {
        if (buffer_size + 1 >= buffer_capacity)
        {
            buffer_capacity++;
            buffer = realloc(buffer, buffer_capacity);
            if (buffer == NULL)
            {
                printf("Reallocation of %d bytes of memory failed", buffer_capacity);
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
        printf("[1] Register New Club\n");
        printf("[2] Manage Clubs\n");
        printf("[3] Register New Student\n");
        printf("[4] Manage Students\n");
        printf("[Q] Quit Program");

        choice = _getch();
        switch (choice)
        {
        case '1':
            clear_console();
            register_club();
            break;
        case '2':
            clear_console();
            manage_clubs();
            break;
        case '3':
            clear_console();
            register_student();
            break;
        case '4':
            clear_console();
            manage_students();
            break;
        }
    }

    printf("\n\nQuitting program...");
}

void prompt_return(char *to)
{
    printf("Press [R] to return to %s.", to);
    char user_input;
    while (user_input != 'r')
    {
        user_input = _getch();
    };
}
