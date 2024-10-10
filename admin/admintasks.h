#ifndef ADMINTASKS_H
#define ADMINTASKS_H

int verify_admin(const char *email, const char *password);
int add_employee(int id, const char *name, const char *email, const char* password, int is_manager);
int modify_employee(int id, const char *name, const char *email, const char* password, int is_manager);
int manage_user_roles(int id, int new_role);
int change_admin_password(const char *email, const char *new_password);

#endif // ADMINTASKS_H