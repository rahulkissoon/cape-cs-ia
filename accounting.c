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
        print_greeting();
        printf("VIEW FINANCIAL LEDGER OF CLUB '%s'\n\n", club.name);
        if (club.transaction_count == 0)
        {
            printf("No transactions have yet been recorded for club '%s'.", club.name);
        }
        else
        {
            struct Transaction *club_transactions = malloc(club.transaction_count * sizeof(struct Transaction));
            int j = 0;
            for (int i = 0; i < transaction_count; i++)
            {
                if (transactions[i].club_id == club.id)
                {
                    club_transactions[j] = transactions[i];
                    j++;
                    if (j == club.transaction_count)
                    {
                        break;
                    }
                }
            }

            char ***rows = malloc(club.transaction_count * sizeof(char **));
            if (rows == NULL)
            {
                printf("Allocation of %lld bytes of memory failed", club.transaction_count * sizeof(char **));
                exit(1);
            }
            int total_columns = 5;
            char *column_headings[] = {"ID",
                                       "Date and Time",
                                       "Particulars",
                                       "Amount ($)",
                                       "Type"};
            int column_widths[5] = {strlen(column_headings[0]),
                                    strlen("YYYY/MM/DD HH:MM"),
                                    strlen(column_headings[2]),
                                    strlen(column_headings[3]),
                                    strlen(column_headings[4])};

            double account_balance = 0;
            for (int i = 0; i < club.transaction_count; i++)
            {
                struct Transaction transaction = club_transactions[i];

                int transaction_id_length = snprintf(NULL, 0, "%d", transaction.id);
                if (column_widths[0] < transaction_id_length)
                {
                    column_widths[0] = transaction_id_length;
                }

                int transaction_particulars_length = strlen(transaction.particulars);
                if (column_widths[2] < transaction_particulars_length)
                {
                    column_widths[2] = transaction_particulars_length;
                }

                int transaction_amount_length = snprintf(NULL, 0, "%.2lf", transaction.amount);
                if (column_widths[3] < transaction_amount_length)
                {
                    column_widths[3] = transaction_amount_length;
                }

                if (transaction.type == CREDIT)
                {
                    account_balance -= transaction.amount;
                    int transaction_type_length = strlen("Credit");
                    if (column_widths[4] < transaction_type_length)
                    {
                        column_widths[4] = transaction_type_length;
                    }
                }
                else
                {
                    account_balance += transaction.amount;
                    int transaction_type_length = strlen("Debit");
                    if (column_widths[4] < transaction_type_length)
                    {
                        column_widths[4] = transaction_type_length;
                    }
                }
            }

            for (int i = 0; i < club.transaction_count; i++)
            {
                struct Transaction transaction = club_transactions[i];

                rows[i] = malloc(total_columns * sizeof(char *));
                if (rows[i] == NULL)
                {
                    printf("Allocation of %lld bytes of memory failed", total_columns * sizeof(char *));
                    exit(1);
                }

                for (int j = 0; j < total_columns; j++)
                {
                    rows[i][j] = malloc((column_widths[j] + 1) * sizeof(char));
                    if (rows[i][j] == NULL)
                    {
                        printf("Allocation of %lld bytes of memory failed", (column_widths[j] + 1) * sizeof(char));
                        exit(1);
                    }

                    switch (j)
                    {
                    case 0:
                        int transaction_id_length = snprintf(NULL, 0, "%d", transaction.id);
                        char *stringified_transaction_id = malloc((transaction_id_length + 1) * sizeof(char));
                        if (stringified_transaction_id == NULL)
                        {
                            printf("Allocation of %lld bytes of memory failed", (transaction_id_length + 1) * sizeof(char));
                            exit(1);
                        }
                        snprintf(stringified_transaction_id, transaction_id_length + 1, "%d", transaction.id);
                        strcpy(rows[i][j], stringified_transaction_id);
                        free(stringified_transaction_id);
                        break;

                    case 1:
                        strcpy(rows[i][j], format_time_t(transaction.time));
                        break;

                    case 2:
                        strcpy(rows[i][j], transaction.particulars);
                        break;

                    case 3:
                        int transaction_amount_length = snprintf(NULL, 0, "%.2lf", transaction.amount);
                        char *stringified_transaction_amount = malloc((transaction_amount_length + 1) * sizeof(char));
                        if (stringified_transaction_amount == NULL)
                        {
                            printf("Allocation of %lld bytes of memory failed", (transaction_amount_length + 1) * sizeof(char));
                            exit(1);
                        }
                        snprintf(stringified_transaction_amount, (transaction_amount_length + 1), "%.2lf", transaction.amount);
                        strcpy(rows[i][j], stringified_transaction_amount);
                        free(stringified_transaction_amount);
                        break;

                    case 4:
                        if (transaction.type == CREDIT)
                        {
                            strcpy(rows[i][j], "Credit");
                        }
                        else
                        {
                            strcpy(rows[i][j], "Debit");
                        }
                        break;
                    }
                }
            }

            int account_balance_length = snprintf(NULL, 0, "%.2lf", account_balance);
            int total_footer_rows = 1;
            char **footer_rows = malloc(total_footer_rows * sizeof(char *));
            footer_rows[0] = malloc((strlen("Account Balance: ($): ") + account_balance_length + 1) * sizeof(char));
            sprintf(footer_rows[0], "Account Balance ($): %.2lf", account_balance);

            print_table(total_columns, club.transaction_count, column_widths, 1, column_headings, rows, footer_rows);

            for (int i = 0; i < total_footer_rows; i++)
            {
                free(footer_rows[i]);
            }
            free(footer_rows);
            for (int i = 0; i < club.transaction_count; i++)
            {
                for (int j = 0; j < total_columns; j++)
                {
                    free(rows[i][j]);
                }
                free(rows[i]);
            }
            free(rows);
            free(club_transactions);
        }

        printf("\n\n[A] Record Transaction  [R] Return");
        choice = _getch();
        if (choice == 'a')
        {
            record_transaction(club, club_pos);
        }
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
        printf("> Date and Time (DD/MM/YYYY HH:MM): %s\n", format_time_t(transaction.time));
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
        printf("> Date and Time (DD/MM/YYYY HH:MM): %s\n", format_time_t(transaction.time));
        printf("> Type: %s\n", transaction.type == DEBIT ? "Debit" : "Credit");
        printf("> Amount: $");
        scanf("%lf", &transaction.amount);
        if (transaction.amount > 0)
        {
            break;
        }
    }

    print_greeting();
    printf("RECORD FINANCIAL TRANSACTION FOR CLUB '%s'\n\n", club.name);
    printf("> Particulars (max %d characters): %s\n", MAX_PARTICULARS_LENGTH, transaction.particulars);
    printf("> Date and Time (DD/MM/YYYY HH:MM): %s\n", format_time_t(transaction.time));
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
        printf("Allocation of %lld bytes of memory failed", (transaction_count + 1) * sizeof(struct Transaction));
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
    printf("> Date and Time (DD/MM/YYYY HH:MM): %s\n", format_time_t(transaction.time));
    printf("> Type: %s\n", transaction.type == DEBIT ? "Debit" : "Credit");
    printf("> Amount: $%.2lf\n\n", transaction.amount);
    prompt_return("the club menu");
}