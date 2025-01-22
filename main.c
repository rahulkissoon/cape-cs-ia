#include <stdio.h>
#include "menu_system.h"
#include "clubs.h"
#include "auth.h"
#include "database.h"

int main()
{
    load_data_from_file();
    if (admin_password == NULL)
    {
        first_time_setup();
    }
    show_main_menu();
    return 0;
}
