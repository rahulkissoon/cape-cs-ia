#include <time.h>
#include <stdbool.h>
#include "students.h"

#ifndef AUTH_H
#define AUTH_H
extern char *admin_password;

enum AuthorizationLevel
{
    STANDARD,
    ADMIN
};

bool prompt_authorization(char *action_name, char *password, enum AuthorizationLevel authorization_level, char *return_to);
#endif
