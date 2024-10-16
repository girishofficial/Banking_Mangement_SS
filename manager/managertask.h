//
// Created by girish-pc on 10/15/24.
//

#ifndef MANAGERTASK_H
#define MANAGERTASK_H
int verify_manager(const char *email, const char *password);
int update_customer_status(int customer_id, int new_status);
int review_customer_feedback(int socket);
int assign_employee_to_loan(int loan_id, int employee_id);
int show_all_loans(int new_socket);
#endif //MANAGERTASK_H
