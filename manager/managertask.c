//
// Created by girish-pc on 10/15/24.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "../db.h"
#include <sys/socket.h>
#include "../employee/emptask.h"

#define BUFFER_SIZE 10240
#define LOGGED_IN_FILE "/home/girish-pc/projecter/db/logged_in.txt"


int verify_manager(const char *email, const char *password) {
    const char *file_path = "/home/girish-pc/projecter/db/employees.txt";
    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    Employee emp;
    while (read(fd, &emp, sizeof(Employee)) > 0) {
        if (strcmp(emp.email, email) == 0 && strcmp(emp.password, password) == 0 && emp.is_manager == 1) {
            close(fd);
            return 1; // Credentials are valid
        }
    }

    close(fd);
    return 0; // Invalid credentials
}

int update_customer_status(int customer_id, int new_status) {
    const char *file_path = "/home/girish-pc/projecter/db/customers.txt";
    int fd = open(file_path, O_RDWR);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    Customer customer;
    while (read(fd, &customer, sizeof(Customer)) > 0) {
        if (customer.id == customer_id) {
            lseek(fd, -sizeof(Customer), SEEK_CUR);
            customer.account_active = new_status;
            if (write(fd, &customer, sizeof(Customer)) == -1) {
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

int review_customer_feedback(int socket) {
    const char *file_path = "/home/girish-pc/projecter/db/feedback.txt";
    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0'; // Null-terminate the buffer
        send(socket, buffer, bytes_read, 0);
    }

    if (bytes_read == -1) {
        perror("Failed to read file");
        close(fd);
        return 0;
    }

    close(fd);
    return 1;
}


int assign_employee_to_loan(int loan_id, int employee_id) {
    const char* file_path = "/home/girish-pc/projecter/db/loans.txt";
    int fd = open(file_path, O_RDWR);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    LoanApplication loan;
    while (read(fd, &loan, sizeof(LoanApplication)) > 0) {
        if (loan.loan_id == loan_id) {
            lseek(fd, -sizeof(LoanApplication), SEEK_CUR);
            loan.assigned_employee_id = employee_id;
            if (write(fd, &loan, sizeof(LoanApplication)) == -1) {
                perror("Failed to write to file");
                close(fd);
                return 0;
            }
            close(fd);
            return 1; // Assignment successful
        }
    }

    close(fd);
    return 0; // Loan not found
}
