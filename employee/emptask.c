#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "../db.h"


#define BUFFER_SIZE 10240
#define LOGGED_IN_FILE "/home/girish-pc/projecter/db/logged_in.txt"
#include "../customer/customertask.h"

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
    int fd = open(file_path, O_RDWR); // Change to O_RDWR to allow writing
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    Employee emp;
    off_t pos;
    while ((pos = lseek(fd, 0, SEEK_CUR)) != -1 && read(fd, &emp, sizeof(Employee)) > 0) {
        if (emp.id == employee_id) {
            if (emp.logged_in != 1 && strcmp(emp.password, password) == 0 && emp.is_manager == 0) {
                emp.logged_in = 1;
                lseek(fd, pos, SEEK_SET); // Move the file pointer back to the start of the employee record
                if (write(fd, &emp, sizeof(Employee)) == -1) {
                    perror("Failed to update login status");
                    close(fd);
                    return 0;
                }
                close(fd);
                return 1; // Verification successful
            } else {
                printf("Something not matched");
                close(fd);
                return 0;
            }
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
    customer.id= id;
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

int is_employee_logged_in(const char *email) {
    int fd = open(LOGGED_IN_FILE, O_RDONLY | O_CREAT, 0644);
    if (fd == -1) {
        perror("Failed to open logged in file");
        return 0;
    }

    char line[256];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, line, sizeof(line) - 1)) > 0) {
        line[bytes_read] = '\0'; // Null-terminate the buffer
        line[strcspn(line, "\n")] = 0; // Remove newline character
        if (strcmp(line, email) == 0) {
            close(fd);
            return 1; // Employee is already logged in
        }
    }

    close(fd);
    return 0; // Employee is not logged in
}

int set_employee_logged_in(int employee_id) {
    const char *file_path = "/home/girish-pc/projecter/db/employees.txt";
    int fd = open(file_path, O_RDWR);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    Employee employee;
    off_t pos;
    while ((pos = lseek(fd, 0, SEEK_CUR)) != -1 && read(fd, &employee, sizeof(Employee)) > 0) {
        if (employee.id == employee_id) {
            employee.logged_in = 1;
            lseek(fd, pos, SEEK_SET); // Move the file pointer back to the start of the employee record
            if (write(fd, &employee, sizeof(Employee)) == -1) {
                perror("Failed to update login status");
                close(fd);
                return 0;
            }
            close(fd);
            return 1; // Update successful
        }
    }

    close(fd);
    return 0; // Employee not found
}

int log_out_employee(const char *email) {
    FILE *fd = fopen(LOGGED_IN_FILE, "r");
    if (fd == NULL) {
        perror("Failed to open logged in file");
        return 0;
    }

    FILE *temp_fd = fopen("/home/girish-pc/projecter/db/temp.txt", "w+");
    if (temp_fd == NULL) {
        perror("Failed to open temp file");
        fclose(fd);
        return 0;
    }

    char line[256];
    int found = 0;
    while (fgets(line, sizeof(line), fd) != NULL) {
        line[strcspn(line, "\n")] = 0; // Remove newline character
        if (strcmp(line, email) != 0) {
            fprintf(temp_fd, "%s\n", line);
        } else {
            found = 1;
        }
    }

    fclose(fd);
    fclose(temp_fd);

    if (remove(LOGGED_IN_FILE) != 0) {
        perror("Failed to remove logged in file");
        return 0;
    }

    if (rename("/home/girish-pc/projecter/db/temp.txt", LOGGED_IN_FILE) != 0) {
        perror("Failed to rename temp file");
        return 0;
    }

    return found;
}

int process_loan(int loan_id, int employee_id, const char *status) {
    const char* file_path = "/home/girish-pc/projecter/db/loans.txt";
    int fd = open(file_path, O_RDWR);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    LoanApplication loan;
    while (read(fd, &loan, sizeof(LoanApplication)) > 0) {
        if (loan.loan_id == loan_id) {
            if (loan.assigned_employee_id != employee_id) {
                close(fd);
                return 0; // Employee is not authorized to process this loan
            }
            lseek(fd, -sizeof(LoanApplication), SEEK_CUR);
            strncpy(loan.status, status, sizeof(loan.status) - 1);
            loan.status[sizeof(loan.status) - 1] = '\0';
            write(fd, &loan, sizeof(LoanApplication));
            close(fd);

            // If the loan is approved, deposit the loan amount to the customer's account
            if (strcmp(status, "approved") == 0) {
                deposit_money(loan.customer_id, loan.amount);
            }
            return 1;
        }
    }

    close(fd);
    return 0; // Loan not found
}

int show_all_loans(int new_socket) {
    const char* file_path = "/home/girish-pc/projecter/db/loans.txt";
    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    LoanApplication loan;
    char buffer[256];
    while (read(fd, &loan, sizeof(LoanApplication)) > 0) {
        snprintf(buffer, sizeof(buffer), "Loan ID: %d, Customer ID: %d, Amount: %.2f, Status: %s, Assigned Employee ID: %d\n",
                 loan.loan_id, loan.customer_id, loan.amount, loan.status, loan.assigned_employee_id);
        send(new_socket, buffer, strlen(buffer), 0);
    }

    close(fd);
    return 1;
}

int set_employee_logged_out(int employee_id) {
    const char *file_path = "/home/girish-pc/projecter/db/employees.txt";
    FILE *file = fopen(file_path, "r+b");
    if (file == NULL) {
        perror("Failed to open file");
        return 0;
    }

    Employee employee;
    while (fread(&employee, sizeof(Employee), 1, file) == 1) {
        if (employee.id == employee_id) {
            employee.logged_in = 0;
            fseek(file, -sizeof(Employee), SEEK_CUR); // Move the file pointer back to the start of the employee record
            if (fwrite(&employee, sizeof(Employee), 1, file) != 1) {
                perror("Failed to update login status");
                fclose(file);
                return 0;
            }

            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0; // Employee not found
}

