#include <conio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "accounting.h"
#include "clubs.h"
#include "database.h"
#include "menu_system.h"
#include "util.h"

struct Transaction *transactions = NULL;

int transaction_count = 0;

void view_club_ledger(int club_pos)
{
    char choice = '\0';
    while (choice != 'r')
    {
        struct Club club = clubs[club_pos];
        struct Transaction *club_transactions = malloc(club.transaction_count * sizeof(struct Transaction));
        for (int i = 0; i < transaction_count; i++)
        {
            if (transactions[i].club_id == club.id)
            {
                club_transactions[i] = transactions[i];
            }
        }

        print_greeting();
        printf("VIEW FINANCIAL LEDGER OF CLUB '%s'\n\n", club.name);
        if (club.transaction_count == 0)
        {
            printf("No transactions have yet been recorded for club '%s'.", club.name);
        }
        else
        {
            char id_column_heading[] = "ID";
            char time_column_heading[] = "Date and Time";
            char particulars_column_heading[] = "Particulars";
            char type_column_heading[] = "Type";
            char amount_column_heading[] = "Amount ($)";
            int id_column_width = strlen(id_column_heading);
            int time_column_width = strlen("YYYY/MM/DD HH:MM");
            int particulars_column_width = strlen(particulars_column_heading);
            int type_column_width = strlen(type_column_heading);
            int amount_column_width = strlen(amount_column_heading);

            for (int i = 0; i < club.transaction_count; i++)
            {
                struct Transaction transaction = club_transactions[i];

                int transaction_id_length = snprintf(NULL, 0, "%d", transaction.id);
                if (id_column_width < transaction_id_length)
                {
                    id_column_width = transaction_id_length;
                }

                int transaction_particulars_length = strlen(transaction.particulars);
                if (particulars_column_width < transaction_particulars_length)
                {
                    particulars_column_width = transaction_particulars_length;
                }

                int transaction_amount_length = snprintf(NULL, 0, "%.2lf", transaction.amount);
                if (amount_column_width < transaction_amount_length)
                {
                    amount_column_width = transaction_amount_length;
                }

                if (transaction.type == CREDIT)
                {
                    int transaction_type_length = strlen("Credit");
                    if (type_column_width < transaction_type_length)
                    {
                        type_column_width = transaction_type_length;
                    }
                }
                else
                {
                    int transaction_type_length = strlen("Debit");
                    if (type_column_width < transaction_type_length)
                    {
                        type_column_width = transaction_type_length;
                    }
                }
            }

            int total_rows = club.transaction_count + 1;
            int total_column_width = 2 + id_column_width + 3 + time_column_width + 3 + particulars_column_width + 3 + amount_column_width + 3 + type_column_width + 2;
            double balance = 0;

            for (int i = 0; i < total_rows; i++)
            {
                if (i == 0)
                {
                    printf("+");
                    for (int j = 0; j < total_column_width - 2; j++)
                    {
                        printf("-");
                    }
                    printf("+\n");
                }

                printf("| ");
                if (i == 0)
                {
                    for (int j = 0; j < floor((double)(id_column_width - strlen(id_column_heading)) / 2); j++)
                    {
                        printf(" ");
                    }
                    printf(id_column_heading);
                    for (int j = 0; j < ceil((double)(id_column_width - strlen(id_column_heading)) / 2); j++)
                    {
                        printf(" ");
                    }
                    printf(" | ");

                    for (int j = 0; j < floor((double)(time_column_width - strlen(time_column_heading)) / 2); j++)
                    {
                        printf(" ");
                    }
                    printf(time_column_heading);
                    for (int j = 0; j < ceil((double)(time_column_width - strlen(time_column_heading)) / 2); j++)
                    {
                        printf(" ");
                    }
                    printf(" | ");

                    for (int j = 0; j < floor((double)(particulars_column_width - strlen(particulars_column_heading)) / 2); j++)
                    {
                        printf(" ");
                    }
                    printf(particulars_column_heading);
                    for (int j = 0; j < ceil((double)(particulars_column_width - strlen(particulars_column_heading)) / 2); j++)
                    {
                        printf(" ");
                    }
                    printf(" | ");

                    for (int j = 0; j < floor((double)(amount_column_width - strlen(amount_column_heading)) / 2); j++)
                    {
                        printf(" ");
                    }
                    printf(amount_column_heading);
                    for (int j = 0; j < ceil((double)(amount_column_width - strlen(amount_column_heading)) / 2); j++)
                    {
                        printf(" ");
                    }
                    printf(" | ");

                    for (int j = 0; j < floor((double)(type_column_width - strlen(type_column_heading)) / 2); j++)
                    {
                        printf(" ");
                    }
                    printf(type_column_heading);
                    for (int j = 0; j < ceil((double)(type_column_width - strlen(type_column_heading)) / 2); j++)
                    {
                        printf(" ");
                    }
                }
                else
                {
                    struct Transaction transaction = club_transactions[i - 1];

                    int transaction_id_length = snprintf(NULL, 0, "%d", transaction.id);
                    for (int j = 0; j < floor((double)(id_column_width - transaction_id_length) / 2); j++)
                    {
                        printf(" ");
                    }
                    char *stringified_transaction_id = malloc((transaction_id_length + 1) * sizeof(char));
                    if (stringified_transaction_id == NULL)
                    {
                        printf("Failed to allocate %d bytes of memory", transaction_id_length * sizeof(char));
                        exit(1);
                    }
                    snprintf(stringified_transaction_id, transaction_id_length + 1, "%d", transaction.id);
                    printf(stringified_transaction_id);
                    free(stringified_transaction_id);
                    for (int j = 0; j < ceil((double)(id_column_width - transaction_id_length) / 2); j++)
                    {
                        printf(" ");
                    }
                    printf(" | ");

                    char *formatted_time = format_time_t(transaction.time);
                    for (int j = 0; j < floor((double)(time_column_width - strlen(formatted_time)) / 2); j++)
                    {
                        printf(" ");
                    }
                    printf(formatted_time);
                    for (int j = 0; j < ceil((double)(time_column_width - strlen(formatted_time)) / 2); j++)
                    {
                        printf(" ");
                    }
                    printf(" | ");

                    for (int j = 0; j < floor((double)(particulars_column_width - strlen(transaction.particulars)) / 2); j++)
                    {
                        printf(" ");
                    }
                    printf(transaction.particulars);
                    for (int j = 0; j < ceil((double)(particulars_column_width - strlen(transaction.particulars)) / 2); j++)
                    {
                        printf(" ");
                    }
                    printf(" | ");

                    int transaction_amount_length = snprintf(NULL, 0, "%.2lf", transaction.amount);
                    for (int j = 0; j < floor((double)(amount_column_width - transaction_amount_length) / 2); j++)
                    {
                        printf(" ");
                    }
                    printf("%.2lf", transaction.amount);
                    for (int j = 0; j < ceil((double)(amount_column_width - transaction_amount_length) / 2); j++)
                    {
                        printf(" ");
                    }
                    printf(" | ");

                    char transaction_type[7];
                    if (transaction.type == DEBIT)
                    {
                        balance += transaction.amount;
                        strcpy(transaction_type, "Debit");
                    }
                    else
                    {
                        balance -= transaction.amount;
                        strcpy(transaction_type, "Credit");
                    }

                    for (int j = 0; j < floor((double)((type_column_width - strlen(transaction_type))) / 2); j++)
                    {
                        printf(" ");
                    }
                    printf(transaction_type);
                    for (int j = 0; j < ceil((double)(type_column_width - strlen(transaction_type)) / 2); j++)
                    {
                        printf(" ");
                    }
                }

                printf(" |\n");
                printf("|");
                for (int j = 0; j < total_column_width - 2; j++)
                {
                    printf("-");
                }
                printf("|\n");
            }

            int balance_length = snprintf(NULL, 0, "%.2lf", balance);
            char *balance_statement = malloc((strlen("Account Balance: ($): ") + balance_length + 1) * sizeof(char));
            sprintf(balance_statement, "Account Balance ($): %.2lf", balance);
            printf("| ");
            for (int i = 0; i < total_column_width - strlen(balance_statement) - 4; i++)
            {
                printf(" ");
            }
            printf("%s", balance_statement);
            printf(" |\n");
            free(balance_statement);

            printf("+");
            for (int j = 0; j < total_column_width - 2; j++)
            {
                printf("-");
            }
            printf("+");
        }

        printf("\n\n[A] Record Transaction  [R] Return");
        choice = _getch();
        if (choice == 'a')
        {
            record_transaction(club, club_pos);
        }

        free(club_transactions);
    }
}

void record_transaction(struct Club club, int club_pos)
{
    struct Transaction transaction = {0};
    transaction.club_id = club.id;

    while (true)
    {
        print_greeting();
        printf("RECORD FINANCIAL TRANSACTION FOR CLUB '%s'\n\n", club.name);
        printf("> Particulars (max %d characters): ", MAX_PARTICULARS_LENGTH);
        fgets(transaction.particulars, sizeof(transaction.particulars) + sizeof(char), stdin);
        transaction.particulars[strlen(transaction.particulars) - 1] = '\0';
        if (strlen(transaction.particulars) > 0)
        {
            break;
        }
    }

    while (true)
    {
        print_greeting();
        printf("RECORD FINANCIAL TRANSACTION FOR CLUB '%s'\n\n", club.name);
        printf("> Particulars (max %d characters): %s\n", MAX_PARTICULARS_LENGTH, transaction.particulars);
        printf("> Date and Time (DD/MM/YYYY HH:MM): ");

        char user_input[17];
        fgets(user_input, sizeof(user_input) + sizeof(char), stdin);
        user_input[strlen(user_input) - 1] = '\0';

        struct tm tm;
        sscanf(user_input, "%d/%d/%d %d:%d", &tm.tm_mday, &tm.tm_mon, &tm.tm_year, &tm.tm_hour, &tm.tm_min);
        tm.tm_year -= 1900;
        tm.tm_mon -= 1;
        transaction.time = mktime(&tm);

        if (transaction.time == (time_t)-1 || transaction.time > time(NULL))
        {
            continue;
        }
        else
        {
            break;
        }
    }

    while (true)
    {
        print_greeting();
        printf("RECORD FINANCIAL TRANSACTION FOR CLUB '%s'\n\n", club.name);
        printf("> Particulars (max %d characters): %s\n", MAX_PARTICULARS_LENGTH, transaction.particulars);
        printf("> Date and Time (DD/MM/YYYY HH:MM): %s", ctime(&transaction.time));
        printf("> Type: Choose a number corresponding to the type of the transaction from the options presented below.\n");
        printf("[1] Debit\n");
        printf("[2] Credit\n");
        char transaction_type = _getch();
        switch (transaction_type)
        {
        case '1':
            transaction.type = DEBIT;
            break;
        case '2':
            transaction.type = CREDIT;
            break;
        }
        if (transaction_type == '1' || transaction_type == '2')
        {
            break;
        }
    }

    while (true)
    {
        print_greeting();
        printf("RECORD FINANCIAL TRANSACTION FOR CLUB '%s'\n\n", club.name);
        printf("> Particulars (max %d characters): %s\n", MAX_PARTICULARS_LENGTH, transaction.particulars);
        printf("> Date and Time (DD/MM/YYYY HH:MM): %s", ctime(&transaction.time));
        printf("> Type: %s\n", transaction.type == DEBIT ? "Debit" : "Credit");
        printf("> Amount: $");
        scanf("%lf", &transaction.amount);
        clear_input_buffer();
        if (transaction.amount > 0)
        {
            break;
        }
    }

    print_greeting();
    printf("RECORD FINANCIAL TRANSACTION FOR CLUB '%s'\n\n", club.name);
    printf("> Particulars (max %d characters): %s\n", MAX_PARTICULARS_LENGTH, transaction.particulars);
    printf("> Date and Time (DD/MM/YYYY HH:MM): %s", ctime(&transaction.time));
    printf("> Type: %s\n", transaction.type == DEBIT ? "Debit" : "Credit");
    printf("> Amount: $%.2lf\n\n", transaction.amount);

    printf("Please confirm whether you would like to save this transaction for club '%s'. (y/n)\n", club.name);
    printf("Warning: Transactions are immutable records which cannot be modified or deleted.\n");
    char confirmation = _getch();
    if (confirmation != 'y')
    {
        print_greeting();
        printf("Transaction discarded. ");
        return prompt_return("the club menu");
    }

    transactions = realloc(transactions, (transaction_count + 1) * sizeof(struct Transaction));
    if (transactions == NULL)
    {
        printf("Reallocation of %d bytes of memory failed", (transaction_count + 1) * sizeof(struct Transaction));
        exit(1);
    }
    transaction.id = ++clubs[club_pos].prev_transaction_id;
    clubs[club_pos].transaction_count++;
    transactions[transaction_count++] = transaction;
    save_data_to_file();

    print_greeting();
    printf("Transaction successfully recorded for club '%s'.\n\n", club.name);
    printf("> ID: %d\n", transaction.id);
    printf("> Particulars (max %d characters): %s\n", MAX_PARTICULARS_LENGTH, transaction.particulars);
    printf("> Date and Time (DD/MM/YYYY HH:MM): %s", ctime(&transaction.time));
    printf("> Type: %s\n", transaction.type == DEBIT ? "Debit" : "Credit");
    printf("> Amount: $%.2lf\n\n", transaction.amount);
    prompt_return("the club menu");
}