#include <stdio.h>
#include <string.h>
#include "auth.h"
#include "clubs.h"
#include "core.h"
#include "menu_system.h"

int main()
{
    load_data_from_file();
    if (strcmp(admin_password, "") == 0)
    {
        first_time_setup();
    }
    show_main_menu();
    return 0;
}
