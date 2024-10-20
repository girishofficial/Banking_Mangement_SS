#ifndef ADMINTASKS_H
#define ADMINTASKS_H

int verify_admin(const char *email, const char *password);
int add_employee(int id, const char *name, const char *email, const char* password, int is_manager);
int modify_employee(int id, const char *name, const char *email, const char* password, int is_manager);
int manage_user_roles(int id, int new_role);
int change_admin_password(const char *email, const char *new_password);
int add_customer(int id, const char *name, const char *email, const char *phone, const char *password, double balance, int account_active);
int logout_admin(const char *email);

#endif // ADMINTASKS_H