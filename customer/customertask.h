#ifndef CUSTOMERTASK_H
#define CUSTOMERTASK_H

int verify_customer(int customer_id, const char *password);
double view_account_balance(int customer_id);
int deposit_money(int customer_id, double amount);
int withdraw_money(int customer_id, double amount);
int transfer_funds(int from_customer_id, int to_customer_id, double amount);
int apply_for_loan(int customer_id, double amount);
int change_password(int customer_id, const char *new_password);
int add_feedback(int customer_id, const char *feedback);
int view_transaction_history(int customer_id, int socket); // Updated function signature
int get_customer_id_from_email(const char *email);

#endif // CUSTOMERTASK_H