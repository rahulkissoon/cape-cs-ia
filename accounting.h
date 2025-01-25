#include <time.h>
#include "clubs.h"

#ifndef ACCOUNTING_H
#define ACCOUNTING_H
#define MAX_PARTICULARS_LENGTH 40

enum TransactionNature
{
    ASSETS,
    LIABILITIES,
    INCOME,
    CAPITAL,
    EXPENSES
};

enum TransactionType
{
    DEBIT,
    CREDIT
};

struct Transaction
{
    int id;
    int club_id;
    char particulars[MAX_PARTICULARS_LENGTH];
    double amount;
    enum TransactionType type;
    enum TransactionNature nature;
    time_t time;
};

extern struct Transaction *transactions;

extern int transaction_count;

void view_club_ledger(int club_pos);
void record_transaction(struct Club club, int club_pos);
#endif