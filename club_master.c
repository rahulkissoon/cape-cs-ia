#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "menu_system.h"
#include "club_master.h"

struct Member
{
    char name[255];
    char class[255];
    time_t joined_since;
};

struct Club
{
    char name[255];
    char student_reps[255];
    int member_count;
    struct Member members[1024];
    char weekly_meeting_day[9];
    time_t registered_at;
};

struct Club clubs[1024] = {{0}};

int club_count = 0;

const char DATA_FILE_NAME[] = "data.bin";

void register_club()
{
    while (1)
    {
        struct Club club = {"", "", 0, {0}, "", time(NULL)};

        printf("Name: ");
        while (club.name[0] == '\0')
        {
            fgets(club.name, sizeof(club.name), stdin);
            club.name[strlen(club.name) - 1] = '\0';
        }
        clear_console();
        printf("Name: %s\n", club.name);

        printf("Weekly Meeting Day: Choose a number corresponding to the desired weekday from the list presented below.\n");
        printf("[1] Monday\n");
        printf("[2] Tuesday\n");
        printf("[3] Wednesday\n");
        printf("[4] Thursday\n");
        printf("[5] Friday\n");
        char weekly_meeting_day = '\0';
        while (club.weekly_meeting_day[0] == '\0')
        {
            weekly_meeting_day = _getch();
            switch (weekly_meeting_day)
            {
            case '1':
                strcpy(club.weekly_meeting_day, "Monday");
                break;
            case '2':
                strcpy(club.weekly_meeting_day, "Tuesday");
                break;
            case '3':
                strcpy(club.weekly_meeting_day, "Wednesday");
                break;
            case '4':
                strcpy(club.weekly_meeting_day, "Thursday");
                break;
            case '5':
                strcpy(club.weekly_meeting_day, "Friday");
                break;
            }
        }
        clear_console();
        printf("Name: %s\n", club.name);
        printf("Weekly Meeting Day: %s\n", club.weekly_meeting_day);

        printf("Student Representatives: ");
        while (club.student_reps[0] == '\0')
        {
            fgets(club.student_reps, sizeof(club.student_reps), stdin);
            club.student_reps[strlen(club.student_reps) - 1] = '\0';
        }
        clear_console();

        printf("Confirm registration of '%s'? (y/n) ", club.name);
        char confirmation = '\0';
        scanf("%c", &confirmation);
        printf("\n");
        if (tolower(confirmation) != 'y')
        {
            clear_console();
            printf("Registration of '%s' cancelled. ", club.name);
            prompt_return_to_main_menu();
            break;
        }

        clubs[club_count] = club;
        club_count++;
        save_data_to_file();

        clear_console();
        printf("New club successfully registered.\n\n");
        printf("> Name: %s\n", club.name);
        printf("> Registered At: %s", ctime(&club.registered_at));
        printf("> Member Count: %d\n", club.member_count);
        printf("> Weekly Meeting Day: %s\n", club.weekly_meeting_day);
        printf("> Student Representatives: %s\n\n", club.student_reps);
        prompt_return_to_main_menu();
        break;
    }
}

void view_clubs()
{
    while (1)
    {
        clear_console();
        if (club_count == 0)
        {
            printf("No clubs have yet been registered. ");
            return prompt_return_to_main_menu();
        }

        printf("Type in the number indicated in brackets preceding the name and then press [Enter] to view information about the club. Alternatively, enter \"0\" to return to the main menu.\n\n");
        for (int i = 0; i < club_count; i++)
        {
            struct Club club = clubs[i];
            printf("[%d] %s\n", i + 1, club.name);
        }
        printf("\n");

        int club_no = -1;
        while (1)
        {
            char user_input[5];
            fgets(user_input, sizeof(user_input), stdin);
            if (user_input[0] == '0')
            {
                break;
            }

            club_no = atoi(user_input);
            if (club_no <= club_count && club_no != 0)
            {
                break;
            }
        }
        if (club_no == -1)
        {
            break;
        }

        struct Club club = clubs[club_no - 1];
        clear_console();
        printf("Currently viewing club %d/%d\n\n", club_no, club_count);
        printf("> Name: %s\n", club.name);
        printf("> Registered At: %s", ctime(&club.registered_at));
        printf("> Member Count: %d\n", club.member_count);
        printf("> Weekly Meeting Day: %s\n", club.weekly_meeting_day);
        printf("> Student Representatives: %s\n\n", club.student_reps);

        printf("Press [Backspace] at any time to return to the previous menu.");
        char user_input = 0;
        while (user_input != BACKSPACE_ASCII_CODE)
        {
            user_input = _getch();
        };
    }
}

void update_club()
{
    while (1)
    {
        clear_console();
        if (club_count == 0)
        {
            printf("No clubs have yet been registered. ");
            return prompt_return_to_main_menu();
        }

        printf("Type in the number indicated in brackets preceding the name and then press [Enter] to update the club's information. Alternatively, enter \"0\" to return to the main menu.\n\n");
        for (int i = 0; i < club_count; i++)
        {
            struct Club club = clubs[i];
            printf("[%d] %s\n", i + 1, club.name);
        }
        printf("\n");

        int club_no = -1;
        while (1)
        {
            char user_input[5];
            fgets(user_input, sizeof(user_input), stdin);
            if (user_input[0] == '0')
            {
                break;
            }

            club_no = atoi(user_input);
            if (club_no <= club_count && club_no != 0)
            {
                break;
            }
        }
        if (club_no == -1)
        {
            break;
        }

        while (1)
        {
            struct Club club = clubs[club_no - 1];
            clear_console();
            printf("Currently updating '%s' (%d/%d)...\n\n", club.name, club_no, club_count);
            printf("Press the key (indicated in brackets) corresponding to one of the options presented below to continue.\n");
            printf("[1] Name\n");
            printf("[2] Weekly Meeting Day\n");
            printf("[3] Student Representatives\n\n");
            printf("Press [Backspace] at any time to return to the previous menu.");

            char choice = _getch();
            if (choice == BACKSPACE_ASCII_CODE)
            {
                break;
            }
            else
            {
                while (1)
                {
                    switch (choice)
                    {
                    case '1':
                        clear_console();
                        printf("After typing the new name for '%s', press [Enter] to confirm your choice.\n\n", club.name);
                        char new_name[255] = "";
                        while (new_name[0] == '\0')
                        {
                            fgets(new_name, sizeof(new_name), stdin);
                            new_name[strlen(new_name) - 1] = '\0';
                        }
                        char old_name[155];
                        strcpy(old_name, club.name);
                        strcpy(club.name, new_name);
                        clubs[club_no - 1] = club;
                        save_data_to_file();
                        clear_console();
                        printf("Successfully updated the name of '%s' (previously '%s'). ", club.name, old_name);
                        break;

                    case '2':
                        clear_console();
                        printf("Choose a number corresponding to the desired weekday from the list presented below to update the weekly meeting day of '%s'.\n", club.name);
                        printf("[1] Monday\n");
                        printf("[2] Tuesday\n");
                        printf("[3] Wednesday\n");
                        printf("[4] Thursday\n");
                        printf("[5] Friday\n");
                        char old_weekly_meeting_day[9] = "";
                        strcpy(old_weekly_meeting_day, club.weekly_meeting_day);
                        char new_weekly_meeting_day = '\0';
                        while (new_weekly_meeting_day == '\0')
                        {
                            new_weekly_meeting_day = _getch();
                            switch (new_weekly_meeting_day)
                            {
                            case '1':
                                strcpy(club.weekly_meeting_day, "Monday");
                                break;
                            case '2':
                                strcpy(club.weekly_meeting_day, "Tuesday");
                                break;
                            case '3':
                                strcpy(club.weekly_meeting_day, "Wednesday");
                                break;
                            case '4':
                                strcpy(club.weekly_meeting_day, "Thursday");
                                break;
                            case '5':
                                strcpy(club.weekly_meeting_day, "Friday");
                                break;
                            }
                        }
                        clubs[club_no - 1] = club;
                        save_data_to_file();
                        clear_console();
                        printf("Successfully updated the weekly meeting day of '%s' from %s to %s. ", club.name, old_weekly_meeting_day, club.weekly_meeting_day);
                        break;

                    case '3':
                        clear_console();
                        printf("Enter the full names of the new student representatives of '%s' below. To confirm your choice, press [Enter].\n\n", club.name);
                        char new_student_reps[1024] = "";
                        while (new_student_reps[0] == '\0')
                        {
                            fgets(new_student_reps, sizeof(new_student_reps), stdin);
                            new_student_reps[strlen(new_student_reps) - 1] = '\0';
                        }
                        strcpy(club.student_reps, new_student_reps);
                        clubs[club_no - 1] = club;
                        save_data_to_file();
                        clear_console();
                        printf("Successfully updated student representatives of '%s'. ", club.name);
                        break;
                    }

                    if (choice == '1' || choice == '2' || choice == '3')
                    {
                        printf("Press [Backspace] at any time to return to the previous menu.");
                        char user_input = 0;
                        while (user_input != BACKSPACE_ASCII_CODE)
                        {
                            user_input = _getch();
                        };
                        break;
                    }
                }
            }
        }
    }
}

void delete_club()
{
    while (1)
    {
        clear_console();
        if (club_count == 0)
        {
            printf("There are no registered clubs to be deleted. ");
            return prompt_return_to_main_menu();
        }

        printf("Type in the number indicated in brackets preceding the name and then press [Enter] to delete the club. Alternatively, enter \"0\" to return to the main menu.\n\n");
        for (int i = 0; i < club_count; i++)
        {
            struct Club club = clubs[i];
            printf("[%d] %s\n", i + 1, club.name);
        }
        printf("\n");

        int club_no = -1;
        while (1)
        {
            char user_input[5];
            fgets(user_input, sizeof(user_input), stdin);
            if (user_input[0] == '0')
            {
                break;
            }

            club_no = atoi(user_input);
            if (club_no <= club_count && club_no != 0)
            {
                break;
            }
        }
        if (club_no == -1)
        {
            break;
        }

        struct Club club = clubs[club_no - 1];
        clear_console();
        printf("Please confirm whether you would like to permanently delete '%s'. (y/n)\n", club.name);
        printf("Warning: This is an irreversible action. All data associated with '%s' will be irrecoverably lost.\n", club.name);
        char confirmation = _getch();
        if (tolower(confirmation) != 'y')
        {
            break;
        }
        if (club_count > club_no)
        {
            for (int i = club_no - 1; i < club_count - 1; i++)
            {
                clubs[i] = clubs[i + 1];
            }
        }
        struct Club placeholder_club = {0};
        clubs[club_count - 1] = placeholder_club;
        club_count -= 1;
        save_data_to_file();

        clear_console();
        printf("Successfully deleted '%s'. Press [Backspace] at any time to return to the previous menu.", club.name);
        char user_input = 0;
        while (user_input != BACKSPACE_ASCII_CODE)
        {
            user_input = _getch();
        };
    }
}

void save_data_to_file()
{
    FILE *dataFilePtr = fopen(DATA_FILE_NAME, "wb");
    char decision_on_err;
    while (dataFilePtr == NULL)
    {
        printf("Error in creating data storage file. Press [Q] to quit without saving data (not recommended) or any other key to try again.\n");
        decision_on_err = _getch();
        if (tolower(decision_on_err) == 'q')
        {
            exit(1);
        }
        else
        {
            dataFilePtr = fopen(DATA_FILE_NAME, "wb");
        }
    }

    fwrite(&club_count, sizeof(int), 1, dataFilePtr);
    for (int i = 0; i < club_count; i++)
    {
        fwrite(&clubs[i], sizeof(struct Club), 1, dataFilePtr);
    }
    fclose(dataFilePtr);
}

void load_data_from_file()
{
    FILE *dataFilePtr = fopen(DATA_FILE_NAME, "rb");
    if (dataFilePtr == NULL)
    {
        return;
    }
    else
    {
        fread(&club_count, sizeof(int), 1, dataFilePtr);
        for (int i = 0; i < club_count; i++)
        {
            fread(&clubs[i], sizeof(struct Club), 1, dataFilePtr);
        }
    }
    fclose(dataFilePtr);
}
