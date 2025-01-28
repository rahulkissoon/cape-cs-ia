#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "auth.h"
#include "database.h"
#include "menu_system.h"

char *admin_password = NULL;

bool prompt_authorization(char *action_name, char *password, enum AuthorizationLevel authorization_level, char *return_to)
{
    print_greeting();
    printf("%s requires %sauthorization. Enter your password below to authenticate your identity.\n\n", action_name, authorization_level == ADMIN ? "administrator " : "");

    char *inputted_password = accept_variable_length_input();
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
    free(inputted_password);
    inputted_password = NULL;

    if (!is_authorized)
    {
        print_greeting();
        printf("Authentication failed; incorrect password. Press [R] to return to %s.", return_to);
        char user_input = '\0';
        while (user_input != 'r')
        {
            user_input = _getch();
        }
    };
    return is_authorized;
}

void change_admin_password()
{
    bool is_authorized = prompt_authorization("Changing the administrator password", admin_password, ADMIN, "the main menu");
    if (!is_authorized)
    {
        return;
    }

    while (true)
    {
        print_greeting();
        printf("Enter the new administrator password below. It must contain at least 8 characters.\n\n");
        printf("Password: ");
        char *password = accept_variable_length_input();
        if (strlen(password) < MIN_PASSWORD_LENGTH)
        {
            free(password);
            password = NULL;
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
        char *password_confirmation = accept_variable_length_input();
        if (strcmp(password, password_confirmation) == 0)
        {
            strcpy(admin_password, password);
            save_data_to_file();
            free(password);
            free(password_confirmation);
            password = NULL;
            password_confirmation = NULL;
            break;
        }
        free(password);
        free(password_confirmation);
        password = NULL;
        password_confirmation = NULL;
    }

    print_greeting();
    printf("The administrator password has been successfully changed. ");
    prompt_return("the main menu");
}
