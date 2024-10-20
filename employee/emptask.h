//
// Created by girish-pc on 10/14/24.
//

#ifndef EMPTASK_H
#define EMPTASK_H

int get_employee_id_from_email(const char *email);
int verify_employee(int employee_id, const char *password);
int add_new_customer(int id, const char *name, const char *email, const char *phone, const char *password, double balance, int account_active);
int list_customers();
int modify_customer_details(int customer_index, Customer *updated_customer);
int get_customer_details(int customer_index, Customer *customer);
int view_customer_transactions(int customer_id, int socket);
int change_employee_password(int employee_id, const char *new_password);
int is_employee_logged_in(const char *email);
int set_employee_logged_in(int employee_id);
int set_employee_logged_out(int employee_id);
int process_loan(int loan_id, int employee_id, const char *status);

#endif //EMPTASK_H