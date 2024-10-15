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

#define BUFFER_SIZE 10240


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