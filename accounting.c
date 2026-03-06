#include <conio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "accounting.h"
#include "clubs.h"
#include "core.h"
#include "menu_system.h"

struct Transaction *transactions = NULL; // Initializes transactions to be NULL, will be populated later. transactions is a global variable.

int transaction_count = 0; // transaction_count is initialized to 0, will be read later from data file. transaction_count is a global variable.

// view_club_ledger shows the complete transaction history for a club and allows the user to create new transactions.
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
            struct Transaction *club_transactions = malloc(club.transaction_count * sizeof(struct Transaction)); // The number of transactions any club has is a variable value, so a fixed-size array cannot be declared.
            int j = 0;
            for (int i = 0; i < transaction_count; i++) // Loops through all transactions and adds transactions belonging to the club to the club_transactions array
            {
                if (transactions[i].club_id == club.id)
                {
                    club_transactions[j] = transactions[i];
                    j++;
                    if (j == club.transaction_count)
                    {
                        break; // Breaks the loop once the number of items in the array is equal to the club's transaction count
                    }
                }
            }

            // contents[i][j] is such that i is the column index and j is the row index of a table.
            char ***contents = malloc(club.transaction_count * sizeof(char **)); // Each transaction will be displayed on a separate cell.
            if (contents == NULL)
            {
                report_alloc_error(club.transaction_count * sizeof(char **));
            }
            int total_columns = 5;
            char *column_headings[] = {"ID",
                                       "Date and Time",
                                       "Particulars",
                                       "Amount ($)",
                                       "Type"};
            // Initializes default column widths based on the length of the heading. Column widths may be dynamically resized later depending on the size of the row contents.
            int column_widths[5] = {strlen(column_headings[0]),
                                    strlen("YYYY/MM/DD HH:MM"),
                                    strlen(column_headings[2]),
                                    strlen(column_headings[3]),
                                    strlen(column_headings[4])};

            double account_balance = 0; // account_balance is initialized to be 0 and will be later altered based on the type and amount of every transaction

            // The club_transactions are iterated over twice. This first time is to adjust the column_widths accordingly.
            for (int i = 0; i < club.transaction_count; i++)
            {
                struct Transaction transaction = club_transactions[i];

                int transaction_id_length = snprintf(NULL, 0, "%d", transaction.id);
                if (column_widths[0] < transaction_id_length)
                {
                    column_widths[0] = transaction_id_length; // Increases transaction_id_length if the row content length exceeds the current transaction_id_length
                }

                int transaction_particulars_length = strlen(transaction.particulars);
                if (column_widths[2] < transaction_particulars_length)
                {
                    column_widths[2] = transaction_particulars_length; // Increases transaction_particulars_length if the row content length exceeds the current transaction_particulars_length
                }

                int transaction_amount_length = snprintf(NULL, 0, "%.2lf", transaction.amount);
                if (column_widths[3] < transaction_amount_length)
                {
                    column_widths[3] = transaction_amount_length; // Increases transaction_amount_length if the row content length exceeds the current transaction_amount_length
                }

                if (transaction.type == CREDIT)
                {
                    account_balance -= transaction.amount;
                    int transaction_type_length = strlen("Credit");
                    if (column_widths[4] < transaction_type_length)
                    {
                        column_widths[4] = transaction_type_length; // Increases transaction_type_length if the row content length exceeds the current transaction_type_length
                    }
                }
                else
                {
                    account_balance += transaction.amount;
                    int transaction_type_length = strlen("Debit");
                    if (column_widths[4] < transaction_type_length)
                    {
                        column_widths[4] = transaction_type_length; // Increases transaction_type_length if the row content length exceeds the current transaction_type_length
                    }
                }
            }

            // Now that the column widths have been properly calculated, the contents of the table will be populated.
            for (int i = 0; i < club.transaction_count; i++)
            {
                struct Transaction transaction = club_transactions[i];

                contents[i] = malloc(total_columns * sizeof(char *)); // contents[i] is the ith column of the table.
                if (contents[i] == NULL)
                {
                    report_alloc_error(total_columns * sizeof(char *));
                }

                for (int j = 0; j < total_columns; j++)
                {
                    contents[i][j] = malloc((column_widths[j] + 1) * sizeof(char)); // contents[i][j] represents the cell defined by the jth row and ith column.
                    if (contents[i][j] == NULL)
                    {
                        report_alloc_error((column_widths[j] + 1) * sizeof(char));
                    }

                    switch (j) // Formats the cell contents based on the type of data (indicated by j, that is the column)
                    {
                    case 0:
                        int transaction_id_length = snprintf(NULL, 0, "%d", transaction.id);
                        char *stringified_transaction_id = malloc((transaction_id_length + 1) * sizeof(char)); // The transaction ID length is variable, so a fixed-size array cannot be declared.
                        if (stringified_transaction_id == NULL)
                        {
                            report_alloc_error((transaction_id_length + 1) * sizeof(char));
                        }
                        snprintf(stringified_transaction_id, transaction_id_length + 1, "%d", transaction.id); // strcpy accepts a string input, not an integer input, so transaction.id must be converted to a string.
                        strcpy(contents[i][j], stringified_transaction_id);
                        free(stringified_transaction_id);
                        break;

                    case 1:
                        strcpy(contents[i][j], format_time_t(transaction.time));
                        break;

                    case 2:
                        strcpy(contents[i][j], transaction.particulars);
                        break;

                    case 3:
                        int transaction_amount_length = snprintf(NULL, 0, "%.2lf", transaction.amount);
                        char *stringified_transaction_amount = malloc((transaction_amount_length + 1) * sizeof(char)); // The transaction amount length is variable, so a fixed-size array cannot be declared.
                        if (stringified_transaction_amount == NULL)
                        {
                            report_alloc_error((transaction_amount_length + 1) * sizeof(char));
                        }
                        snprintf(stringified_transaction_amount, (transaction_amount_length + 1), "%.2lf", transaction.amount); // strcpy accepts a string input, not a double input, so transaction.amount must be converted to a string
                        strcpy(contents[i][j], stringified_transaction_amount);
                        free(stringified_transaction_amount);
                        break;

                    case 4:
                        if (transaction.type == CREDIT)
                        {
                            strcpy(contents[i][j], "Credit");
                        }
                        else
                        {
                            strcpy(contents[i][j], "Debit");
                        }
                        break;
                    }
                }
            }

            int account_balance_length = snprintf(NULL, 0, "%.2lf", account_balance);
            int total_footer_rows = 1;                                       // There is 1 footer row for the account balance.
            char **footer_rows = malloc(total_footer_rows * sizeof(char *)); // The account balance length is variable, so a fixed-size array cannot be declared.
            footer_rows[0] = malloc((strlen("Account Balance: ($): ") + account_balance_length + 1) * sizeof(char));
            sprintf(footer_rows[0], "Account Balance ($): %.2lf", account_balance);

            print_table(total_columns, club.transaction_count, column_widths, total_footer_rows, column_headings, contents, footer_rows);

            for (int i = 0; i < total_footer_rows; i++)
            {
                free(footer_rows[i]);
            }
            free(footer_rows);
            for (int i = 0; i < club.transaction_count; i++)
            {
                for (int j = 0; j < total_columns; j++)
                {
                    free(contents[i][j]);
                }
                free(contents[i]);
            }
            free(contents);
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

// record_transaction allows the user to record a new transaction for a club.
void record_transaction(struct Club club, int club_pos)
{
    struct Transaction transaction = {0}; // Initializes the new transaction struct
    transaction.club_id = club.id;

    while (true)
    {
        print_greeting();
        printf("RECORD FINANCIAL TRANSACTION FOR CLUB '%s'\n\n", club.name);
        printf("> Particulars (max %d characters): ", MAX_PARTICULARS_LENGTH);
        char transaction_particulars[MAX_PARTICULARS_LENGTH];
        fgets(transaction_particulars, sizeof(transaction_particulars), stdin); // Accepts a fixed-length input for the particulars of the transaction
        transaction_particulars[strcspn(transaction_particulars, "\n")] = '\0';
        strcpy(transaction.particulars, transaction_particulars);
        if (strlen(transaction.particulars) > 0 && strlen(transaction.particulars) <= MAX_PARTICULARS_LENGTH)
        {
            break; // If the user has input a non-zero-length string for the particulars, continue.
        }
    }

    struct Transaction prev_transaction = {0};
    for (int i = transaction_count - 1; i >= 0; i++)
    {
        if (transactions[i].club_id == club.id)
        {
            prev_transaction = transactions[i];
            break;
        }
    }

    while (true)
    {
        print_greeting();
        printf("RECORD FINANCIAL TRANSACTION FOR CLUB '%s'\n\n", club.name);
        printf("> Particulars (max %d characters): %s\n", MAX_PARTICULARS_LENGTH, transaction.particulars);
        printf("> Date and Time (DD/MM/YYYY HH:MM): ");

        transaction.time = parse_time_input();
        // If the time is invalid, the time is greater than the current time or the time is less than the previous transaction's time (ledgers are arranged chronologically), prompt again for a new time.
        if (transaction.time == (time_t)-1 || transaction.time > time(NULL) || (prev_transaction.id > 0 && transaction.time <= prev_transaction.time))
        {
            continue;
        }
        else
        {
            break; // Otherwise, continue.
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
            break; // If the chosen transaction type enum was among the available options, continue.
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
            break; // If the amount of the transaction is a positive real number, continue.
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
    if (confirmation != 'y') // If the user types anything but "y", discard the transaction and return to the main menu.
    {
        print_greeting();
        printf("Transaction discarded. ");
        return prompt_return("the club menu");
    }

    // Recall that transactions is a dynamically-allocated array. Accordingly, when a new transaction is created, it must be resized.
    transactions = realloc(transactions, (transaction_count + 1) * sizeof(struct Transaction));
    if (transactions == NULL)
    {
        report_alloc_error((transaction_count + 1) * sizeof(struct Transaction));
    }
    transaction.id = ++clubs[club_pos].prev_transaction_id; // Increments the previous transaction ID of the club, setting transaction.id to its new value
    clubs[club_pos].transaction_count++;                    // Increments the total transaction count of the club
    transactions[transaction_count++] = transaction;        // Stores the new transaction in the transactions array
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