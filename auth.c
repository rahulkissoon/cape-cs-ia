#include <conio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auth.h"
#include "core.h"
#include "menu_system.h"

char admin_password[MAX_PASSWORD_LENGTH];

// prompt_authorization asks the user to input a password and compares it against the correct password.
// If the inputted password matches the correct password, return true to the caller. Otherwise, prompt the user to return to the specified menu and return false to the caller.
bool prompt_authorization(char *action_name, char *password, enum AuthorizationLevel authorization_level, char *return_to)
{
    print_greeting();
    printf("%s requires %sauthorization. Enter your password below to authenticate your identity.\n\n", action_name, authorization_level == ADMIN ? "administrator " : "");

    char inputted_password[MAX_PASSWORD_LENGTH];
    fgets(inputted_password, MAX_PASSWORD_LENGTH, stdin);
    inputted_password[strcspn(inputted_password, "\n")] = '\0';
    bool is_authorized = false;
    if (strcmp(inputted_password, admin_password) == 0)
    {
        is_authorized = true;
    }
    else if (strcmp(inputted_password, password) == 0)
    {
        if (authorization_level != ADMIN)
        {
            is_authorized = true;
        }
    }

    if (!is_authorized)
    {
        print_greeting();
        printf("Authentication failed; incorrect password. ");
        prompt_return(return_to);
    };
    return is_authorized;
}

// change_admin_password requires that the administrator first input the current administrator password before changing it.
void change_admin_password()
{
    while (true)
    {
        print_greeting();
        printf("Enter the new administrator password below. It must contain at least 8 characters.\n\n");
        printf("Password: ");
        char password[MAX_PASSWORD_LENGTH];
        fgets(password, MAX_PASSWORD_LENGTH, stdin);
        password[strcspn(password, "\n")] = '\0';
        if (strlen(password) < MIN_PASSWORD_LENGTH)
        {
            continue;
        }

        print_greeting();
        printf("Enter the new administrator password below. It must contain at least 8 characters.\n\n");
        printf("Password: ");
        for (int i = 0; i < strlen(password); i++)
        {
            printf("*");
        }
        printf("\n");
        printf("Confirm Password: ");
        char password_confirmation[MAX_PASSWORD_LENGTH];
        fgets(password_confirmation, MAX_PASSWORD_LENGTH, stdin);
        password_confirmation[strcspn(password_confirmation, "\n")] = '\0';
        if (strcmp(password, password_confirmation) == 0)
        {
            strcpy(admin_password, password); // Updates the global variable admin_password to the new password
            save_data_to_file();
            break;
        }
    }

    print_greeting();
    printf("The administrator password has been successfully changed. ");
    prompt_return("the main menu");
}
