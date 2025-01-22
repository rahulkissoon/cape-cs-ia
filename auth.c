#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "auth.h"
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
        printf("Authentication failed; incorrect password. Press any key to return to %s.", return_to);
        _getch();
    };
    return is_authorized;
}
