#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "customertask.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>

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
    if (email == NULL) {
        printf("Email is NULL\n");
        return -1;
    }

    int fd = open("/home/girish-pc/projecter/db/customers.txt", O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return -1;
    }

    Customer customer;
    while (read(fd, &customer, sizeof(Customer)) > 0) {
        customer.email[strcspn(customer.email, "\n")] = '\0'; // Strip newline if exists

        if (strcmp(customer.email, email) == 0) {
            close(fd);
            return customer.id;
        }
    }

    close(fd);
    return -1; // Customer not found
}
int verify_customer(int customer_id, const char *password) {
    int fd = open("/home/girish-pc/projecter/db/customers.txt", O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    Customer customer;
    while (read(fd, &customer, sizeof(Customer)) > 0) {
        if (customer.id == customer_id) {
            if (strcmp(customer.password, password) == 0) {
                close(fd);
                return 1; // Verification successful
            } else {
                printf("Password mismatch for Customer ID %d.\n", customer_id);
                close(fd);
                return 0; // Password mismatch
            }
        }
    }

    printf("Customer ID %d not found.\n", customer_id);
    close(fd);
    return 0; // Customer not found
}


double view_account_balance(int customer_id) {
    int fd = open("/home/girish-pc/projecter/db/customers.txt", O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return -1.0;
    }

    Customer customer;
    while (read(fd, &customer, sizeof(Customer)) > 0) {
        printf("Checking customer ID: %d\n", customer.id); // Debugging output
        if (customer.id == customer_id) {
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
        if (customer.id == customer_id) {
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
        if (customer.id == customer_id) {
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

int apply_for_loan(int customer_id, double amount) {
    const char* file_path = "/home/girish-pc/projecter/db/loans.txt";
    FILE *file = fopen(file_path, "a");
    if (file == NULL) {
        perror("Failed to open file");
        return 0;
    }

    LoanApplication loan;
    loan.loan_id = customer_id * 100000 + (int)time(NULL) % 100000; // Generate a unique loan ID based on customer ID and timestamp
    loan.customer_id = customer_id;
    loan.amount = amount;
    strncpy(loan.status, "pending", sizeof(loan.status) - 1);
    loan.status[sizeof(loan.status) - 1] = '\0';
    loan.assigned_employee_id = -1; // Not assigned yet

    fwrite(&loan, sizeof(LoanApplication), 1, file);
    fclose(file);
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
        if (customer.id == customer_id) {
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

int view_transaction_history(const char *email, int socket) {
    int customer_id = get_customer_id_from_email(email);
    if (customer_id == -1) {
        send(socket, "Customer not found.\n", 20, 0);
        return 0;
    }

    int fd = open("/home/girish-pc/projecter/db/transactions.txt", O_RDONLY);
    if (fd == -1) {
        perror("Failed to open transactions file");
        return 0;
    }

    char buffer[256];
    char message[1024] = "Transaction History:\n";
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0'; // Null-terminate the buffer
        char *line = strtok(buffer, "\n");
        while (line != NULL) {
            int id;
            sscanf(line, "Customer ID %d:", &id);
            if (id == customer_id) {
                strcat(message, line);
                strcat(message, "\n");
            }
            line = strtok(NULL, "\n");
        }
    }

    if (bytes_read == -1) {
        perror("Failed to read transactions file");
        close(fd);
        return 0;
    }

    close(fd);
    send(socket, message, strlen(message), 0);
    return 1;
}

void log_transaction(int customer_id, const char *type, double amount) {
    FILE *file = fopen("/home/girish-pc/projecter/db/transactions.txt", "a");
    if (file == NULL) {
        perror("Failed to open transactions file");
        return;
    }

    fprintf(file, "Customer ID %d: %s %.2f\n", customer_id, type, amount);
    fclose(file);
}

int customer_exists(int customer_id) {
    const char *file_path = "/home/girish-pc/projecter/db/customers.txt";
    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    Customer customer;
    while (read(fd, &customer, sizeof(Customer)) > 0) {
        if (customer.id == customer_id) {
            close(fd);
            return 1; // Customer found
        }
    }

    close(fd);
    return 0; // Customer not found
}

int transfer_funds(const char *from_email, const char *to_email, double amount) {
    if (amount <= 0) {
        printf("Transfer failed: Invalid amount\n");
        return 0; // Invalid amount
    }

    int from_customer_id = get_customer_id_from_email(from_email);
    if (from_customer_id == -1) {
        printf("Transfer failed: Sender email %s not found\n", from_email);
        return 0; // Sender not found
    }

    int to_customer_id = get_customer_id_from_email(to_email);
    if (to_customer_id == -1) {
        printf("Transfer failed: Recipient email %s not found\n", to_email);
        return 0; // Recipient not found
    }

    double from_balance = view_account_balance(from_customer_id);
    if (from_balance < amount) {
        printf("Transfer failed: Insufficient funds\n");
        return 0; // Insufficient funds
    }

    if (!withdraw_money(from_customer_id, amount)) {
        printf("Transfer failed: Withdrawal failed\n");
        return 0; // Withdrawal failed
    }

    if (!deposit_money(to_customer_id, amount)) {
        // If deposit fails, rollback the withdrawal
        deposit_money(from_customer_id, amount);
        printf("Transfer failed: Deposit failed\n");
        return 0; // Deposit failed
    }

    // Log the transaction for both sender and receiver
    log_transaction(from_customer_id, "Transfer Out", amount);
    log_transaction(to_customer_id, "Transfer In", amount);

    printf("Transfer successful\n");
    return 1; // Transfer successful
}
