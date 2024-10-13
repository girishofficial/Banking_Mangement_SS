#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "customertask.h"

#include <sys/socket.h>

#include "../db.h"

int add_transaction(int customer_id, double amount, const char *type) {
    int fd = open("/home/girish-pc/projecter/db/transactions.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd == -1) {
        perror("Failed to open transactions file");
        return 0;
    }

    char transaction_entry[1024];
    snprintf(transaction_entry, sizeof(transaction_entry), "Customer ID %d: %s %.2f\n", customer_id, type, amount);

    if (write(fd, transaction_entry, strlen(transaction_entry)) == -1) {
        perror("Failed to write transaction");
        close(fd);
        return 0;
    }

    printf("Transaction added for Customer ID %d.\n", customer_id);
    close(fd);
    return 1;
}

int get_customer_id_from_email(const char *email) {
    int fd = open("/home/girish-pc/projecter/db/customers.txt", O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return -1;
    }

    Customer customer;
    while (read(fd, &customer, sizeof(Customer)) > 0) {
        customer.email[strcspn(customer.email, "\n")] = '\0'; // Strip newline if exists

        printf("Comparing %s with %s\n", customer.email, email);  // Debugging output

        if (strcmp(customer.email, email) == 0) {
            close(fd);
            return customer.customer_id;
        }
    }

    close(fd);
    return -1; // Customer not found
}


int verify_customer(int customer_id, const char *password) {
    int fd = open("/home/girish-pc/projecter/db/customers.txt", O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return -1;
    }

    Customer customer;
    while (read(fd, &customer, sizeof(Customer)) > 0) {
        customer.password[strcspn(customer.password, "\n")] = '\0'; // Strip newline if exists

        printf("Checking customer ID: %d, password: %s\n", customer.customer_id, customer.password);  // Debugging output

        if (customer.customer_id == customer_id && strcmp(customer.password, password) == 0) {
            close(fd);
            return 1; // Customer verified
        }
    }

    close(fd);
    return 0; // Customer not found or password incorrect
}


double view_account_balance(int customer_id) {
    int fd = open("/home/girish-pc/projecter/db/customers.txt", O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return -1.0;
    }

    Customer customer;
    while (read(fd, &customer, sizeof(Customer)) > 0) {
        printf("Checking customer ID: %d\n", customer.customer_id); // Debugging output
        if (customer.customer_id == customer_id) {
            printf("Account Balance for Customer ID %d: %.2f\n", customer_id, customer.balance);
            close(fd);
            return customer.balance;
        }
    }

    printf("Customer ID %d not found.\n", customer_id);
    close(fd);
    return -1.0;
}

int deposit_money(int customer_id, double amount) {
    int fd = open("/home/girish-pc/projecter/db/customers.txt", O_RDWR);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    Customer customer;
    off_t pos;
    while ((pos = lseek(fd, 0, SEEK_CUR)) != -1 && read(fd, &customer, sizeof(Customer)) > 0) {
        if (customer.customer_id == customer_id) {
            customer.balance += amount;
            lseek(fd, pos, SEEK_SET); // Move the file pointer back to the start of the customer record
            if (write(fd, &customer, sizeof(Customer)) == -1) {
                perror("Failed to write updated balance");
                close(fd);
                return 0;
            }
            printf("Deposited %.2f to Customer ID %d. New Balance: %.2f\n", amount, customer_id, customer.balance);
            close(fd);
            add_transaction(customer_id, amount, "deposit"); // Add transaction
            return 1;
        }
    }

    printf("Customer ID %d not found.\n", customer_id);
    close(fd);
    return 0;
}

int withdraw_money(int customer_id, double amount) {
    int fd = open("/home/girish-pc/projecter/db/customers.txt", O_RDWR);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    Customer customer;
    off_t pos;
    while ((pos = lseek(fd, 0, SEEK_CUR)) != -1 && read(fd, &customer, sizeof(Customer)) > 0) {
        if (customer.customer_id == customer_id) {
            if (customer.balance >= amount) {
                customer.balance -= amount;
                lseek(fd, pos, SEEK_SET); // Move the file pointer back to the start of the customer record
                if (write(fd, &customer, sizeof(Customer)) == -1) {
                    perror("Failed to write updated balance");
                    close(fd);
                    return 0;
                }
                printf("Withdrew %.2f from Customer ID %d. New Balance: %.2f\n", amount, customer_id, customer.balance);
                close(fd);
                add_transaction(customer_id, amount, "withdrawal"); // Add transaction
                return 1;
            } else {
                printf("Insufficient balance for Customer ID %d.\n", customer_id);
                close(fd);
                return 0;
            }
        }
    }

    printf("Customer ID %d not found.\n", customer_id);
    close(fd);
    return 0;
}

int transfer_funds(int from_customer_id, int to_customer_id, double amount) {
    // Implementation to transfer funds
    return 1;
}

int apply_for_loan(int customer_id, double amount) {
    // Implementation to apply for a loan
    return 1;
}

int change_password(int customer_id, const char *new_password) {
    int fd = open("/home/girish-pc/projecter/db/customers.txt", O_RDWR);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    Customer customer;
    off_t pos;
    while ((pos = lseek(fd, 0, SEEK_CUR)) != -1 && read(fd, &customer, sizeof(Customer)) > 0) {
        if (customer.customer_id == customer_id) {
            strncpy(customer.password, new_password, sizeof(customer.password) - 1);
            customer.password[sizeof(customer.password) - 1] = '\0'; // Ensure null-termination
            lseek(fd, pos, SEEK_SET); // Move the file pointer back to the start of the customer record
            if (write(fd, &customer, sizeof(Customer)) == -1) {
                perror("Failed to write updated password");
                close(fd);
                return 0;
            }
            printf("Password changed for Customer ID %d.\n", customer_id);
            close(fd);
            return 1;
        }
    }

    printf("Customer ID %d not found.\n", customer_id);
    close(fd);
    return 0;
}

int add_feedback(int customer_id, const char *feedback) {
    int fd = open("/home/girish-pc/projecter/db/feedback.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd == -1) {
        perror("Failed to open feedback file");
        return 0;
    }

    char feedback_entry[1024];
    snprintf(feedback_entry, sizeof(feedback_entry), "Customer ID %d: %s\n", customer_id, feedback);

    if (write(fd, feedback_entry, strlen(feedback_entry)) == -1) {
        perror("Failed to write feedback");
        close(fd);
        return 0;
    }

    printf("Feedback added for Customer ID %d.\n", customer_id);
    close(fd);
    return 1;
}

int view_transaction_history(int customer_id, int socket) {
    int fd = open("/home/girish-pc/projecter/db/transactions.txt", O_RDONLY);
    if (fd == -1) {
        perror("Failed to open transactions file");
        return 0;
    }

    char buffer[1024];
    ssize_t bytes_read;
    char transaction_msg[1024] = "Transaction History:\n";

    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0'; // Null-terminate the buffer
        char *line = strtok(buffer, "\n");
        while (line != NULL) {
            if (strstr(line, "Customer ID") && strstr(line, "Customer ID") == line) {
                int id;
                sscanf(line, "Customer ID %d:", &id);
                if (id == customer_id) {
                    strcat(transaction_msg, line);
                    strcat(transaction_msg, "\n");
                }
            }
            line = strtok(NULL, "\n");
        }
    }

    send(socket, transaction_msg, strlen(transaction_msg), 0);
    close(fd);
    return 1;
}