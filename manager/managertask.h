//
// Created by girish-pc on 10/15/24.
//

#ifndef MANAGERTASK_H
#define MANAGERTASK_H
int verify_manager(const char *email, const char *password);
int update_customer_status(int customer_id, int new_status);
int review_customer_feedback(int socket);
#endif //MANAGERTASK_H
