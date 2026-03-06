#include <stdbool.h>
#include <time.h>
#include "students.h"

#ifndef AUTH_H_INCLUDED
#define AUTH_H_INCLUDED
#define MIN_PASSWORD_LENGTH 8
#define MAX_PASSWORD_LENGTH 128

extern char admin_password[MAX_PASSWORD_LENGTH];

enum AuthorizationLevel
{
    STANDARD,
    ADMIN
};

bool prompt_authorization(char *action_name, char *password, enum AuthorizationLevel authorization_level, char *return_to);
void change_admin_password();
#endif
