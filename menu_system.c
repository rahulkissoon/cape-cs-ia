#include "club_master.h"
#include "menu_system.h"
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>

const int BACKSPACE_ASCII_CODE = 8;

void clear_console()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void show_main_menu()
{
    while (1)
    {
        clear_console();

        printf("+-------------------------------------------------+\n");
        printf("|    ____    __             __   _______     __   |\n");
        printf("|   / __/___/ /  ___  ___  / /  / ___/ /_ __/ /   |\n");
        printf("|  _\\ \\/ __/ _ \\/ _ \\/ _ \\/ /  / /__/ / // / _ \\  |\n");
        printf("| /___/\\__/_//_/\\___/\\___/_/   \\___/_/\\_,_/_.__/  |\n");
        printf("|   /  |/  /__ ____  ___ ____ ____ ____           |\n");
        printf("|  / /|_/ / _ `/ _ \\/ _ `/ _ `/ -_) __/           |\n");
        printf("| /_/  /_/\\_,_/_//_/\\_,_/\\_, /\\__/_/              |\n");
        printf("|                      /___/                      |\n");
        printf("+-------------------------------------------------+\n\n");

        char choice = '\0';
        printf("Press the key (indicated in brackets) corresponding to one of the options presented below to continue.\n\n");
        printf("[1] Register New Club\n");
        printf("[2] View Clubs\n");
        printf("[3] Update Existing Club\n");
        printf("[4] Delete Club\n");
        printf("[Q] Quit Program");

        choice = tolower(_getch());
        switch (choice)
        {
        case '1':
            clear_console();
            register_club();
            break;
        case '2':
            clear_console();
            view_clubs();
            break;
        case '3':
            clear_console();
            break;
        case '4':
            clear_console();
            delete_club();
            break;
        case 'q':
            printf("\n\nQuitting program...");
            exit(0);
            break;
        }
    }
}

void prompt_return_to_main_menu()
{
    printf("Press [Backspace] at any time to return to the main menu.");
    char keyboard_input;
    while (keyboard_input != BACKSPACE_ASCII_CODE)
    {
        keyboard_input = _getch();
    };
}
