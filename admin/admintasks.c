#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include "../db.h"
#include "admintasks.h"

int verify_admin(const char *email, const char *password) {
    const char* file_path = "/home/girish-pc/projecter/db/admins.txt";
    char stored_email[50], stored_password[50];
    int logged_in;
    FILE *file = fopen(file_path, "r+"); // Open the file in read-write mode
    if (file == NULL) {
        perror("Failed to open file db/admins.txt");
        return -1;
    }

    long pos;
    while ((pos = ftell(file)) != -1 && fscanf(file, "%49[^,],%49[^,],%d\n", stored_email, stored_password, &logged_in) != EOF) {
        if (strcmp(stored_email, email) == 0 && strcmp(stored_password, password) == 0) {
            if (logged_in == 1) {
                fclose(file);
                return 0; // Already logged in
            }
            logged_in = 1; // Set logged_in to 1
            fseek(file, pos, SEEK_SET); // Move the file pointer back to the start of the current record
            fprintf(file, "%s,%s,%d\n", stored_email, stored_password, logged_in); // Update the record
            fclose(file);
            return 1; // Record found and updated
        }
    }

    fclose(file);
    return 0; // Record not found
}

int lookup(const char *id) {
    const char* file_path = "/home/girish-pc/projecter/db/employees.txt";
    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }
    Employee emp;
    while (read(fd, &emp, sizeof(emp)) > 0) {
        if (strcmp(id, emp.id) == 0) {
            close(fd);
            return 1; // ID already exists
        }
    }
    close(fd);
    return 0;
}

int add_employee(int id, const char *name, const char *email, const char *password, int is_manager) {
    const char* file_path = "/home/girish-pc/projecter/db/employees.txt";
    FILE *file = fopen(file_path, "a");
    if (file == NULL) {
        perror("Failed to open file");
        return 0;
    }

    Employee emp;
    emp.id = id;
    strncpy(emp.name, name, sizeof(emp.name) - 1);
    strncpy(emp.email, email, sizeof(emp.email) - 1);
    strncpy(emp.password, password, sizeof(emp.password) - 1);
    emp.is_manager = is_manager;

    fwrite(&emp, sizeof(Employee), 1, file);
    fclose(file);
    return 1;
}

int modify_employee(int id, const char *name, const char *email, const char *password, int is_manager) {
    const char *file_path = "/home/girish-pc/projecter/db/employees.txt";
    int fd = open(file_path, O_WRONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }
    Employee emp;
    while (read(fd, &emp, sizeof(emp)) > 0) {
        if (emp.id == id) {
            lseek(fd, -sizeof(emp), SEEK_CUR);
            strncpy(emp.name, name, sizeof(emp.name) - 1);
            strncpy(emp.email, email, sizeof(emp.email) - 1);
            strncpy(emp.password, password, sizeof(emp.password) - 1);
            emp.is_manager = is_manager;
            write(fd, &emp, sizeof(emp));
            close(fd);
            return 1;
        }
    }
    close(fd);
    return 0;
}

int manage_user_roles(int id, int new_role) {
    const char *file_path = "/home/girish-pc/projecter/db/employees.txt";
    int fd = open(file_path, O_RDWR);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    Employee emp;
    while (read(fd, &emp, sizeof(emp)) > 0) {
        if (emp.id == id) {
            lseek(fd, -sizeof(emp), SEEK_CUR);
            emp.is_manager = new_role;
            write(fd, &emp, sizeof(emp));
            close(fd);
            return 1;
        }
    }

    close(fd);
    return 0;
}


int change_admin_password(const char *email, const char *new_password) {
    const char *file_path = "/home/girish-pc/projecter/db/admins.txt";
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return 0;
    }

    char line[256];
    char temp_path[] = "/home/girish-pc/projecter/db/temp.txt";
    FILE *temp_file = fopen(temp_path, "w");
    if (temp_file == NULL) {
        perror("Failed to open temp file");
        fclose(file);
        return 0;
    }

    int found = 0;

    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");
        if (token && strcmp(token, email) == 0) {
            found = 1;
            fprintf(temp_file, "%s,%s\n", email, new_password);
        } else {
            fprintf(temp_file, "%s", line);
        }
    }

    fclose(file);
    fclose(temp_file);

    if (found) {
        remove(file_path);
        rename(temp_path, file_path);
    } else {
        remove(temp_path);
    }

    return found;
}

int add_customer(int id, const char *name, const char *email, const char *phone, const char *password, double balance, int account_active) {
    const char* file_path = "/home/girish-pc/projecter/db/customers.txt";
    FILE *file = fopen(file_path, "a");
    if (file == NULL) {
        perror("Failed to open file");
        return 0;
    }

    Customer customer;
    customer.id = id;
    strncpy(customer.name, name, sizeof(customer.name) - 1);
    strncpy(customer.email, email, sizeof(customer.email) - 1);
    strncpy(customer.phone, phone, sizeof(customer.phone) - 1);
    strncpy(customer.password, password, sizeof(customer.password) - 1);
    customer.balance = balance;
    customer.account_active = account_active;

    fwrite(&customer, sizeof(Customer), 1, file);
    fclose(file);
    return 1;
}

