#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "../db.h"


#define BUFFER_SIZE 10240

int get_customer_details(int customer_index, Customer *customer) {
    const char *file_path = "/home/girish-pc/projecter/db/customers.txt";
    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    int index = 1;
    while (read(fd, customer, sizeof(Customer)) > 0) {
        if (index++ == customer_index) {
            close(fd);
            return 1;
        }
    }

    close(fd);
    return 0; // Customer not found
}

int get_employee_id_from_email(const char *email) {
    const char *file_path = "/home/girish-pc/projecter/db/employees.txt";
    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return -1;
    }

    Employee emp;
    while (read(fd, &emp, sizeof(Employee)) > 0) {
        if (strcmp(emp.email, email) == 0) {
            close(fd);
            return emp.id;
        }
    }

    close(fd);
    return -1; // Employee not found
}

int verify_employee(int employee_id, const char *password) {
    const char *file_path = "/home/girish-pc/projecter/db/employees.txt";
    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    Employee emp;
    while (read(fd, &emp, sizeof(Employee)) > 0) {
        if (emp.id == employee_id && strcmp(emp.password, password) == 0) {
            close(fd);
            return 1; // Verification successful
        }
    }

    close(fd);
    return 0; // Verification failed
}

int add_new_customer(int id, const char *name, const char *email, const char *phone, const char *password, double balance, int account_active) {
    const char *file_path = "/home/girish-pc/projecter/db/customers.txt";
    int fd = open(file_path, O_WRONLY | O_APPEND);
    if (fd == -1) {
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

    if (write(fd, &customer, sizeof(Customer)) == -1) {
        perror("Failed to write to file");
        close(fd);
        return 0;
    }

    close(fd);
    return 1;
}

int list_customers() {
    const char *file_path = "/home/girish-pc/projecter/db/customers.txt";
    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    Customer customer;
    int index = 1;
    while (read(fd, &customer, sizeof(Customer)) > 0) {
        printf("%d. %s\n", index++, customer.name);
    }

    close(fd);
    return index - 1; // Return the number of customers
}

int modify_customer_details(int customer_index, Customer *updated_customer) {
    const char *file_path = "/home/girish-pc/projecter/db/customers.txt";
    int fd = open(file_path, O_RDWR);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    Customer customer;
    int index = 1;
    off_t pos;
    while ((pos = lseek(fd, 0, SEEK_CUR)) != -1 && read(fd, &customer, sizeof(Customer)) > 0) {
        if (index++ == customer_index) {
            lseek(fd, pos, SEEK_SET);
            if (write(fd, updated_customer, sizeof(Customer)) == -1) {
                perror("Failed to write to file");
                close(fd);
                return 0;
            }
            close(fd);
            return 1;
        }
    }

    close(fd);
    return 0; // Customer not found
}

int view_customer_transactions(int customer_id, int socket) {
    const char *file_path = "/home/girish-pc/projecter/db/transactions.txt";
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return 0;
    }

    char buffer[BUFFER_SIZE];
    char line[BUFFER_SIZE];
    int found = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        int file_customer_id;
        char type[20];
        double amount;

        if (sscanf(line, "Customer ID %d: %s %lf", &file_customer_id, type, &amount) == 3) {
            if (file_customer_id == customer_id) {
                snprintf(buffer, BUFFER_SIZE, "Customer ID: %d\nType: %s\nAmount: %.2f\n\n",
                         file_customer_id, type, amount);
                send(socket, buffer, strlen(buffer), 0);
                found = 1;
            }
        }
    }

    if (!found) {
        send(socket, "No transactions found for this customer.\n", 41, 0);
    }

    fclose(file);
    return 1;
}

int change_employee_password(int employee_id, const char *new_password) {
    const char *file_path = "/home/girish-pc/projecter/db/employees.txt";
    int fd = open(file_path, O_RDWR);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    Employee emp;
    off_t pos;
    while ((pos = lseek(fd, 0, SEEK_CUR)) != -1 && read(fd, &emp, sizeof(Employee)) > 0) {
        if (emp.id == employee_id) {
            strncpy(emp.password, new_password, sizeof(emp.password) - 1);
            emp.password[sizeof(emp.password) - 1] = '\0'; // Ensure null-termination

            lseek(fd, pos, SEEK_SET);
            if (write(fd, &emp, sizeof(Employee)) == -1) {
                perror("Failed to write to file");
                close(fd);
                return 0;
            }
            close(fd);
            return 1; // Password change successful
        }
    }

    close(fd);
    return 0; // Employee not found
}