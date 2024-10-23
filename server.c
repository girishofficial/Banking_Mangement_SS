#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include "db.h"
#include "admin/admintasks.h"
#include "/home/girish-pc/projecter/customer/customertask.h"
#include "/home/girish-pc/projecter/employee/emptask.h"
#include "/home/girish-pc/projecter/manager/managertask.h"
#include "login/login.h"
#include <termios.h>


#define PORT 8087
#define BUFFER_SIZE 102400
#define RETRY_LIMIT 5
#define RETRY_DELAY 2

pthread_mutex_t customer_edit_mutex;

int server_fd;

void signal_handler(int sig) {
    printf("Received signal %d, shutting down server...\n", sig);
    system("/home/girish-pc/projecter/logemp");
    system("/home/girish-pc/projecter/cuslog");
    close(server_fd);
    exit(0);
}

void *handle_client(void *socket_desc) {
    int new_socket = *(int *)socket_desc;
    char buffer[BUFFER_SIZE];
    int read_size;

    const char *menu = "Welcome to the Bank of Banglore(IIIT)\n"
                       "=======================================================\n"
                       "Select an option:\n"
                       "1. Customer Login\n"
                       "2. Employee Login\n"
                       "3. Manager Login\n"
                       "4. Admin Login\n"
                       "Type EXIT to close the program anytime.\n";
    system("clear");
    send(new_socket, menu, strlen(menu), 0);

    while ((read_size = read(new_socket, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[read_size] = '\0'; // Null-terminate the buffer
        int option = atoi(buffer);

        switch (option) {
            case 1: {
                char email[50], password[50];
                write(new_socket, "Enter Customer Email: ", 22);
                read(new_socket, email, 50);
                email[strcspn(email, "\n")] = 0; // Removing the newline character from input

                write(new_socket, "Enter Customer Password: ", 25);
                read(new_socket, password, 50);
                password[strcspn(password, "\n")] = 0; // Removing the newline character from input
                int customer_id = get_customer_id_from_email(email);
                if (customer_id == -1) {
                        write(new_socket, "Login failed. Invalid credentials.\n", 35);
                        bzero(buffer, BUFFER_SIZE);
                        usleep(100);
                        send(new_socket, menu, strlen(menu), 0);
                    } else {
                        // if(is_customer_logged_in(customer_id)) {
                        //     write(new_socket, "Login failed. Customer already logged in.\n", 42);
                        //     bzero(buffer, BUFFER_SIZE);
                        //     usleep(100);
                        //     send(new_socket, menu, strlen(menu), 0);
                        // }else {
                            int result = verify_customer(customer_id, password);
                            if (result == 1) {
                                write(new_socket, "Login successful.\n", 18);

                                const char *customer_menu = "Welcome Customer\n"
                                                            "=======================================================\n"
                                                            "Customer Options:\n"
                                                            "1. View Account Balance\n"
                                                            "2. Deposit Money\n"
                                                            "3. Withdraw Money\n"
                                                            "4. Transfer Funds\n"
                                                            "5. Apply for a Loan\n"
                                                            "6. Change Password\n"
                                                            "7. Add Feedback\n"
                                                            "8. View Transaction History\n"
                                                            "9. Logout\n"
                                                            "10. Exit\n";
                                send(new_socket, customer_menu, strlen(customer_menu), 0);

                                while ((read_size = read(new_socket, buffer, BUFFER_SIZE - 1)) > 0) {
                                    buffer[read_size] = '\0'; // Null-terminate the buffer
                                    int customer_option = atoi(buffer);

                                    switch (customer_option) {
                                        case 1:
                                            double balance = view_account_balance(customer_id);
                                        if(balance >= 0) {
                                            char balance_msg[100];
                                            sprintf(balance_msg, "Account Balance: %.2f\n", balance);
                                            send(new_socket, balance_msg, strlen(balance_msg), 0);
                                            bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                            usleep(100);
                                            send(new_socket, customer_menu, strlen(customer_menu), 0);
                                        }else {
                                            send(new_socket, "Failed to fetch account balance\n", 32, 0);
                                            bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                            usleep(100);
                                            send(new_socket, customer_menu, strlen(customer_menu), 0);
                                        }
                                        break;
                                        case 2:
                                            double amount;
                                        write(new_socket, "Enter amount to deposit: ", 25);
                                        read(new_socket, buffer, BUFFER_SIZE);
                                        amount = atof(buffer);

                                        if (deposit_money(customer_id, amount)) {
                                            send(new_socket, "Deposit successful.\n", 20, 0);
                                            bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                            usleep(100);
                                            send(new_socket, customer_menu, strlen(customer_menu), 0);
                                        } else {
                                            send(new_socket, "Failed to deposit money.\n", 25, 0);
                                            bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                            usleep(100);
                                            send(new_socket, customer_menu, strlen(customer_menu), 0);
                                        }
                                        break;
                                        case 3:
                                            double amt;
                                        write(new_socket, "Enter amount to withdraw: ", 26);
                                        read(new_socket, buffer, BUFFER_SIZE);
                                        amt = atof(buffer);

                                        if (withdraw_money(customer_id, amt)) {
                                            send(new_socket, "Withdrawal successful.\n", 23, 0);
                                            bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                            usleep(100);
                                            send(new_socket, customer_menu, strlen(customer_menu), 0);
                                        } else {
                                            send(new_socket, "Failed to withdraw money.\n", 26, 0);
                                            bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                            usleep(100);
                                            send(new_socket, customer_menu, strlen(customer_menu), 0);
                                        }
                                        break;
                                        case 4:
                                            int to_customer_id;
                                            write(new_socket, "Enter recipient ID: ", 20);
                                            read(new_socket, buffer, BUFFER_SIZE);
                                            to_customer_id = atoi(buffer);

                                            write(new_socket, "Enter amount to transfer: ", 26);
                                            read(new_socket, buffer, BUFFER_SIZE);
                                            amount = atof(buffer);

                                            if (transfer_funds(customer_id, to_customer_id, amount)) {
                                                send(new_socket, "Transfer successful.\n", 21, 0);
                                            } else {
                                                send(new_socket, "Transfer failed.\n", 17, 0);
                                            }

                                            bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                            usleep(100);
                                            send(new_socket, customer_menu, strlen(customer_menu), 0);
                                            break;
                                        case 5:
                                            double loan_amount;
                                        write(new_socket, "Enter loan amount: ", 19);
                                        read(new_socket, buffer, BUFFER_SIZE);
                                        loan_amount = atof(buffer);

                                        if (apply_for_loan(customer_id, loan_amount)) {
                                            send(new_socket, "Loan application submitted successfully.\n", 41, 0);
                                        } else {
                                            send(new_socket, "Failed to submit loan application.\n", 34, 0);
                                        }

                                        bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                        usleep(100);
                                        send(new_socket, customer_menu, strlen(customer_menu), 0);
                                        break;
                                        case 6:
                                            char new_password[50];
                                        write(new_socket, "Enter new password: ", 20);
                                        read(new_socket, new_password, 50);
                                        new_password[strcspn(new_password, "\n")] = 0; // Removing the newline character from input

                                        if (change_password(customer_id, new_password)) {
                                            send(new_socket, "Password changed successfully.\n", 31, 0);
                                            bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                            usleep(100);
                                            send(new_socket, customer_menu, strlen(customer_menu), 0);
                                        } else {
                                            send(new_socket, "Failed to change password.\n", 27, 0);
                                            bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                            usleep(100);
                                            send(new_socket, customer_menu, strlen(customer_menu), 0);
                                        }
                                        bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                        usleep(100);
                                        send(new_socket, customer_menu, strlen(customer_menu), 0);
                                        break;
                                        case 7:
                                            char feedback[BUFFER_SIZE];
                                        write(new_socket, "Enter your feedback: ", 21);
                                        read(new_socket, feedback, BUFFER_SIZE);
                                        feedback[strcspn(feedback, "\n")] = 0; // Removing the newline character from input

                                        if (add_feedback(customer_id, feedback)) {
                                            send(new_socket, "Feedback added successfully.\n", 29, 0);
                                            bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                            usleep(100);
                                            send(new_socket, customer_menu, strlen(customer_menu), 0);
                                        } else {
                                            send(new_socket, "Failed to add feedback.\n", 24, 0);
                                            bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                            usleep(100);
                                            send(new_socket, customer_menu, strlen(customer_menu), 0);
                                        }
                                        break;
                                        case 8:
                                            if (view_transaction_history(email, new_socket)) {
                                                send(new_socket, "Transaction history displayed.\n", 31, 0);
                                            } else {
                                                send(new_socket, "Failed to display transaction history.\n", 39, 0);
                                            }
                                        bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                        usleep(100);
                                        send(new_socket, customer_menu, strlen(customer_menu), 0);
                                        break;

                                        case 9:
                                            set_logged_out(customer_id);
                                            write(new_socket, "Logged out successfully.\n", 26);
                                            bzero(buffer, BUFFER_SIZE);
                                            usleep(100);
                                            send(new_socket, menu, strlen(menu), 0);
                                            break;
                                        case 10:
                                            set_logged_out(customer_id);
                                            send(buffer,"exit customer", 11, 0);

                                        break;

                                        default:
                                            write(new_socket, "Invalid option. Please select again\n", 37);
                                            bzero(buffer, BUFFER_SIZE);
                                            usleep(100);
                                            send(new_socket, customer_menu, strlen(customer_menu), 0);
                                            break;
                                    }
                                }
                            } else {
                                write(new_socket, "Login failed. Invalid credentials.\n", 35);
                                bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                usleep(100); // Ensure the message is fully sent
                                send(new_socket, menu, strlen(menu), 0); // Show main menu again
                                break;
                            }
                            break;

                }
                break;
            }
            case 2:
                char email[50], password[50];
                write(new_socket, "Enter Employee Email: ", 22);
                read(new_socket, email, 50);
                email[strcspn(email, "\n")] = 0; // Removing the newline character from input

                write(new_socket, "Enter Employee Password: ", 25);
                read(new_socket, password, 50);
                password[strcspn(password, "\n")] = 0;


                    int employee_id = get_employee_id_from_email(email);
                    if (employee_id == -1) {
                        write(new_socket, "Login failed. Invalid credentials.\n", 35);
                        bzero(buffer, BUFFER_SIZE);
                        usleep(100);
                        send(new_socket, menu, strlen(menu), 0);
                    } else {
                        int result = verify_employee(employee_id, password);
                        if (result == 1) {
                            write(new_socket, "Login successful.\n", 18);
                            const char *employee_menu = "Welcome Employee\n"
                                                        "=======================================================\n"
                                                        "Employee Options:\n"
                                                        "1. Add New Customer\n"
                                                        "2. Modify Customer Details\n"
                                                        "3. Process Loan Applications\n"
                                                        "4. View Customer Transactions\n"
                                                        "5. Change Password\n"
                                                        "6. Logout\n"
                                                        "7. Exit\n";
                            send(new_socket, employee_menu, strlen(employee_menu), 0);

                            while ((read_size = read(new_socket, buffer, BUFFER_SIZE - 1)) > 0) {
                                buffer[read_size] = '\0'; // Null-terminate the buffer
                                int employee_option = atoi(buffer);

                                switch (employee_option) {
                                    case 1:
                                        int id;
                                    char name[50], email[50], phone[20], password[50];
                                    double balance;
                                    int account_active;

                                    write(new_socket, "Enter Customer ID: ", 19);
                                    read(new_socket, buffer, BUFFER_SIZE);
                                    id = atoi(buffer);

                                    write(new_socket, "Enter Customer Name: ", 21);
                                    read(new_socket, name, 50);
                                    name[strcspn(name, "\n")] = 0;

                                    write(new_socket, "Enter Customer Email: ", 22);
                                    read(new_socket, email, 50);
                                    email[strcspn(email, "\n")] = 0;

                                    write(new_socket, "Enter Customer Phone: ", 22);
                                    read(new_socket, phone, 20);
                                    phone[strcspn(phone, "\n")] = 0;

                                    write(new_socket, "Customer Password: ", 25);
                                    read(new_socket, password, 50);
                                    password[strcspn(password, "\n")] = 0;

                                    write(new_socket, "Enter Customer Balance: ", 24);
                                    read(new_socket, buffer, BUFFER_SIZE);
                                    balance = atof(buffer);

                                    write(new_socket, "Is Account Active (1 for Yes, 0 for No): ", 41);
                                    read(new_socket, buffer, BUFFER_SIZE);
                                    account_active = atoi(buffer);

                                    if (add_new_customer(id, name, email, phone, password, balance, account_active)) {
                                        send(new_socket, "Customer added successfully.\n", 29, 0);
                                    } else {
                                        send(new_socket, "Failed to add customer.\n", 24, 0);
                                    }
                                    bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                    usleep(100); // Add a slight delay to ensure data is fully sent before the next action
                                    send(new_socket, employee_menu, strlen(employee_menu), 0);
                                    break;
                                    case 2: {
                                        int num_customers = list_customers();
                                        if (num_customers == 0) {
                                            send(new_socket, "No customers found.\n", 20, 0);
                                            break;
                                        }

                                        char buffer[BUFFER_SIZE];
                                        write(new_socket, "Select a customer to modify (1-N): ", 35);
                                        read(new_socket, buffer, BUFFER_SIZE);
                                        int customer_index = atoi(buffer);

                                        if (customer_index < 1 || customer_index > num_customers) {
                                            send(new_socket, "Invalid selection.\n", 19, 0);
                                            break;
                                        }

                                        // Retrieve and display customer details
                                        Customer customer;
                                        if (get_customer_details(customer_index, &customer)) {
                                            char customer_details[BUFFER_SIZE];
                                            snprintf(customer_details, BUFFER_SIZE,
                                                     "Customer Details:\nName: %s\nEmail: %s\nPhone: %s\nBalance: %.2f\nAccount Active: %d\n",
                                                     customer.name, customer.email, customer.phone, customer.balance, customer.account_active);
                                                     send(new_socket, customer_details, strlen(customer_details), 0);
                                        } else {
                                            send(new_socket, "Failed to retrieve customer details.\n", 36, 0);
                                            break;
                                        }

                                        // Provide options to edit customer details
                                        const char *edit_menu = "Edit Options:\n1. Name\n2. Email\n3. Phone\n4. Balance\n5. Account Active\n6. Cancel\nSelect an option (1-6): ";
                                        send(new_socket, edit_menu, strlen(edit_menu), 0);
                                        read(new_socket, buffer, BUFFER_SIZE);
                                        int edit_option = atoi(buffer);

                                        switch (edit_option) {
                                            case 1:
                                                bzero(buffer, BUFFER_SIZE);
                                                write(new_socket, "Enter new name: ", 16);
                                            read(new_socket, customer.name, sizeof(customer.name));
                                            customer.name[strcspn(customer.name, "\n")] = 0;
                                            break;
                                            case 2:
                                                bzero(buffer, BUFFER_SIZE);
                                                write(new_socket, "Enter new email: ", 17);
                                            read(new_socket, customer.email, sizeof(customer.email));
                                            customer.email[strcspn(customer.email, "\n")] = 0;
                                            break;
                                            case 3:
                                                bzero(buffer, BUFFER_SIZE);
                                                write(new_socket, "Enter new phone: ", 17);
                                            read(new_socket, customer.phone, sizeof(customer.phone));
                                            customer.phone[strcspn(customer.phone, "\n")] = 0;
                                            break;
                                            case 4:
                                                bzero(buffer, BUFFER_SIZE);
                                                write(new_socket, "Enter new balance: ", 19);
                                            read(new_socket, buffer, BUFFER_SIZE);
                                            customer.balance = atof(buffer);
                                            break;
                                            case 5:
                                                bzero(buffer, BUFFER_SIZE);
                                                write(new_socket, "Is account active (1 for Yes, 0 for No): ", 41);
                                            read(new_socket, buffer, BUFFER_SIZE);
                                            customer.account_active = atoi(buffer);
                                            break;
                                            case 6:
                                                bzero(buffer, BUFFER_SIZE);
                                                send(new_socket, "Edit cancelled.\n", 16, 0);
                                            break;
                                            default:
                                                send(new_socket, "Invalid option.\n", 16, 0);
                                            break;
                                        }

                                        if (edit_option >= 1 && edit_option <= 5) {
                                            if (modify_customer_details(customer_index, &customer)) {
                                                send(new_socket, "Customer details updated successfully.\n", 39, 0);
                                            } else {
                                                send(new_socket, "Failed to update customer details.\n", 34, 0);
                                            }
                                        }

                                        bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                        usleep(100); // Add a slight delay to ensure data is fully sent before the next action
                                        send(new_socket, employee_menu, strlen(employee_menu), 0);
                                        break;
                                    }
                                    case 3:
                                        // Show all loan applications
                                            if (show_all_loans(new_socket)) {
                                                send(new_socket, "All loan applications displayed.\n", 33, 0);
                                            } else {
                                                send(new_socket, "Failed to display loan applications.\n", 36, 0);
                                                break;
                                            }

                                    // Process a specific loan application
                                    int loan_id;
                                    char loan_status[20];
                                    write(new_socket, "Enter loan ID to process: ", 26);
                                    read(new_socket, buffer, BUFFER_SIZE);
                                    loan_id = atoi(buffer);

                                    write(new_socket, "Enter loan status (approved/rejected): ", 39);
                                    read(new_socket, loan_status, sizeof(loan_status));
                                    loan_status[strcspn(loan_status, "\n")] = '\0'; // Remove newline character

                                    if (process_loan(loan_id, employee_id, loan_status)) {
                                        send(new_socket, "Loan processed successfully.\n", 29, 0);
                                    } else {
                                        send(new_socket, "Failed to process loan.\n", 24, 0);
                                    }

                                    bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                    usleep(100);
                                    send(new_socket, employee_menu, strlen(employee_menu), 0);
                                    break;
                                    case 4:
                                        write(new_socket, "Enter Customer ID: ", 19);
                                        read(new_socket, buffer, BUFFER_SIZE);
                                        int customer_id = atoi(buffer);

                                        if (view_customer_transactions(customer_id, new_socket)) {
                                            send(new_socket, "Transaction history displayed.\n", 31, 0);
                                        } else {
                                            send(new_socket, "Failed to display transaction history.\n", 39, 0);
                                        }
                                        bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                        usleep(100);
                                        send(new_socket, employee_menu, strlen(employee_menu), 0);
                                        break;
                                    case 5:
                                        char new_password[50];
                                    write(new_socket, "Enter new password: ", 20);
                                    read(new_socket, new_password, 50);
                                    new_password[strcspn(new_password, "\n")] = 0;

                                    if (change_employee_password(employee_id, new_password)) {
                                        send(new_socket, "Password changed successfully.\n", 31, 0);
                                    } else {
                                        send(new_socket, "Failed to change password.\n", 27, 0);
                                    }
                                    bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                    usleep(100); // Add a slight delay to ensure data is fully sent before the next action
                                    send(new_socket, employee_menu, strlen(employee_menu), 0);
                                    break;
                                    case 6:
                                        set_employee_logged_out(employee_id);
                                        write(new_socket, "Logged out successfully.\n", 26);
                                        bzero(buffer, BUFFER_SIZE);
                                        usleep(100);
                                        send(new_socket, menu, strlen(menu), 0);
                                        break;
                                    case 7:
                                        set_employee_logged_out(employee_id);
                                        write(new_socket, "exit\n", 11);
                                        send(new_socket,"exit", 11, 0);
                                        break;
                                        default:
                                            write(new_socket, "Invalid option. Please select again\n", 37);
                                            bzero(buffer, BUFFER_SIZE);
                                            usleep(100);
                                            send(new_socket, menu, strlen(menu), 0);
                                        break;
                                }
                            }
                        } else {
                            write(new_socket, "Login failed. Invalid credentials.\n", 35);
                            bzero(buffer, BUFFER_SIZE); // Clear the buffer
                            usleep(100); // Ensure the message is fully sent
                            send(new_socket, menu, strlen(menu), 0);
                        }
                    }
                break;
            case 3: {
                write(new_socket, "Enter Manager Email: ", 21);
                read(new_socket, email, 50);
                email[strcspn(email, "\n")] = 0; // Removing the newline character from input

                write(new_socket, "Enter Manager Password: ", 24);
                read(new_socket, password, 50);
                password[strcspn(password, "\n")] = 0; // Removing the newline character from input
                int manager_id = get_employee_id_from_email(email);
                int result = verify_manager(manager_id, password);
                if (result == 1) {
                    write(new_socket, "Login successful.\n", 18);
                    const char *manager_menu = "Welcome Manager\n"
                                               "=======================================================\n"
                                               "Manager Options:\n"
                                               "1. (activate/deactivate)Modify Customer Accounts\n"
                                               "2. Assign Loan Application Processes to Employees\n"
                                               "3. Review Customer Feedback\n"
                                               "4. Change Password\n"
                                               "5. Logout\n"
                                               "6. Exit\n";
                    send(new_socket, manager_menu, strlen(manager_menu), 0);

                    while ((read_size = read(new_socket, buffer, BUFFER_SIZE - 1)) > 0) {
                        buffer[read_size] = '\0'; // Null-terminate the buffer
                        int manager_option = atoi(buffer);

                        if (manager_option == 1) {
                            int num_customers = list_customers();
                            if (num_customers == 0) {
                                send(new_socket, "No customers found.\n", 20, 0);
                            } else {
                                char buffer[BUFFER_SIZE];
                                write(new_socket, "Select a customer to modify (1-N): ", 35);
                                read(new_socket, buffer, BUFFER_SIZE);
                                int customer_index = atoi(buffer);

                                if (customer_index < 1 || customer_index > num_customers) {
                                    send(new_socket, "Invalid selection.\n", 19, 0);
                                } else {
                                    // Retrieve and display customer details
                                    Customer customer;
                                    if (get_customer_details(customer_index, &customer)) {
                                        char customer_details[BUFFER_SIZE];
                                        snprintf(customer_details, BUFFER_SIZE,
                                                 "Customer Details:\nName: %s\nEmail: %s\nPhone: %s\nBalance: %.2f\nAccount Active: %d\n",
                                                 customer.name, customer.email, customer.phone, customer.balance, customer.account_active);
                                        send(new_socket, customer_details, strlen(customer_details), 0);
                                    } else {
                                        send(new_socket, "Failed to retrieve customer details.\n", 36, 0);
                                    }
                                    int retry_count = 0;
                                    // Provide options to edit customer details
                                    const char *edit_menu = "Edit Options:\n1. Name\n2. Email\n3. Phone\n4. Balance\n5. Account Active\n6. Cancel\nSelect an option (1-6): ";
                                    send(new_socket, edit_menu, strlen(edit_menu), 0);
                                    read(new_socket, buffer, BUFFER_SIZE);
                                    int edit_option = atoi(buffer);

                                    if (edit_option == 1) {
                                        write(new_socket, "Enter new name: ", 16);
                                        read(new_socket, customer.name, sizeof(customer.name));
                                        customer.name[strcspn(customer.name, "\n")] = 0;
                                    } else if (edit_option == 2) {
                                        write(new_socket, "Enter new email: ", 17);
                                        read(new_socket, customer.email, sizeof(customer.email));
                                        customer.email[strcspn(customer.email, "\n")] = 0;
                                    } else if (edit_option == 3) {
                                        write(new_socket, "Enter new phone: ", 17);
                                        read(new_socket, customer.phone, sizeof(customer.phone));
                                        customer.phone[strcspn(customer.phone, "\n")] = 0;
                                    } else if (edit_option == 4) {
                                        write(new_socket, "Enter new balance: ", 19);
                                        read(new_socket, buffer, BUFFER_SIZE);
                                        customer.balance = atof(buffer);
                                    } else if (edit_option == 5) {
                                        write(new_socket, "Is account active (1 for Yes, 0 for No): ", 41);
                                        read(new_socket, buffer, BUFFER_SIZE);
                                        customer.account_active = atoi(buffer);
                                    } else if (edit_option == 6) {
                                        send(new_socket, "Edit cancelled.\n", 16, 0);
                                    } else {
                                        send(new_socket, "Invalid option.\n", 16, 0);
                                    }

                                    if (edit_option >= 1 && edit_option <= 5) {
                                        if (modify_customer_details(customer_index, &customer)) {
                                            send(new_socket, "Customer details updated successfully.\n", 39, 0);
                                        } else {
                                            send(new_socket, "Failed to update customer details due to already being existed or wrong format.\n", 34, 0);
                                        }
                                    }

                                    bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                    usleep(100);
                                    send(new_socket, manager_menu, strlen(manager_menu), 0);
                                }
                            }
                        } else if (manager_option == 2) {
                            // Show all loan applications
                            if (show_all_loans(new_socket)) {
                                send(new_socket, "All loan applications displayed.\n", 33, 0);
                            } else {
                                send(new_socket, "Failed to display loan applications.\n", 38, 0);
                            }

                            // Prompt for loan ID to process
                            write(new_socket, "Enter loan ID to process: ", 26);
                            read(new_socket, buffer, BUFFER_SIZE);
                            int loan_id = atoi(buffer);

                            // Clear the buffer before reading the next input
                            bzero(buffer, BUFFER_SIZE);

                            // Prompt for employee ID to assign
                            write(new_socket, "Enter employee ID to assign: ", 29);
                            read(new_socket, buffer, BUFFER_SIZE);
                            int employee_id = atoi(buffer);

                            if (assign_employee_to_loan(loan_id, employee_id)) {
                                send(new_socket, "Employee assigned to loan successfully.\n", 40, 0);
                            } else {
                                send(new_socket, "Failed to assign employee to loan.\n", 35, 0);
                            }

                            bzero(buffer, BUFFER_SIZE); // Clear the buffer
                            usleep(100);
                            send(new_socket, manager_menu, strlen(manager_menu), 0);
                        } else if (manager_option == 3) {
                            if (review_customer_feedback(new_socket)) {
                                send(new_socket, "Feedback displayed successfully.\n", 33, 0);
                            } else {
                                send(new_socket, "Failed to display feedback.\n", 28, 0);
                            }
                            bzero(buffer, BUFFER_SIZE);
                            usleep(100);
                            send(new_socket, manager_menu, strlen(manager_menu), 0);
                        } else if (manager_option == 4) {
                            char new_password[50];
                            write(new_socket, "Enter new password: ", 20);
                            read(new_socket, new_password, 50);
                            new_password[strcspn(new_password, "\n")] = 0; // Removing the newline character from input

                            if (change_manager_password(employee_id, new_password)) {
                                send(new_socket, "Password changed successfully.\n", 31, 0);
                            } else {
                                send(new_socket, "Failed to change password.\n", 27, 0);
                            }
                            bzero(buffer, BUFFER_SIZE);
                            usleep(100);
                            send(new_socket, manager_menu, strlen(manager_menu), 0);
                            break;
                        } else if (manager_option == 5) {
                            set_employee_logged_out(manager_id);
                            write(new_socket, "Logged out successfully.\n", 26);
                            bzero(buffer, BUFFER_SIZE); // Clear the buffer
                            usleep(100); // Add a slight delay to ensure data is fully sent before the next action
                            send(new_socket, menu, strlen(menu), 0);
                            break;
                        } else if (manager_option == 6) {
                            // Code to exit
                            write(new_socket, "Exiting...\n", 11);
                            close(new_socket);
                            exit(0);
                            break;
                        } else {
                            write(new_socket, "Invalid option. Please select again\n", 37);
                        }
                    }
                } else {
                    write(new_socket, "Login failed. Invalid credentials.\n", 35);
                    bzero(buffer, BUFFER_SIZE); // Clear the buffer
                    usleep(100); // Ensure the message is fully sent
                    send(new_socket, menu, strlen(menu), 0); // Show main menu again
                }
                break;
            }
            case 4: {
                char email[50], password[50];
                write(new_socket, "Enter Admin Email: ", 19);
                read(new_socket, email, 50);
                email[strcspn(email, "\n")] = 0; // Removing the newline character from input
                printf("Received email: %s\n", email); // Debugging line

                write(new_socket, "Enter admin password: ", 21);
                read(new_socket, password, 50);
                password[strcspn(password, "\n")] = 0; // Removing the newline character from input
                printf("Received password: %s\n", password); // Debugging line

                int result = verify_admin(email, password);
                if (result == 1) {
                    write(new_socket, "Login successful.\n", 18);
                    const char *admin_menu = "Welcome Admin\n"
                                             "=======================================================\n"
                                             "Admin Options:\n"
                                             "1. Add Employee\n"
                                             "2. Modify Customer\n"
                                             "3. Manage Employee\n"
                                             "4. Change Password\n"
                                             "5. Add Customer\n"
                                             "6. Logout\n";
                    send(new_socket, admin_menu, strlen(admin_menu), 0);

                    while ((read_size = read(new_socket, buffer, BUFFER_SIZE - 1)) > 0) {
                        buffer[read_size] = '\0'; // Null-terminate the buffer
                        int admin_option = atoi(buffer);
                        if (admin_option == 1) {
                            // Collect employee details
                            int id;
                            char name[50], email[50], password[50];
                            int is_manager;

                            write(new_socket, "Enter Employee ID: ", 19);
                            read(new_socket, buffer, BUFFER_SIZE);
                            id = atoi(buffer);

                            write(new_socket, "Enter Employee Name: ", 21);
                            read(new_socket, name, 50);
                            name[strcspn(name, "\n")] = 0;

                            write(new_socket, "Enter Employee Email: ", 22);
                            read(new_socket, email, 50);
                            email[strcspn(email, "\n")] = 0;

                            write(new_socket, "Employee Password: ", 25);
                            read(new_socket, password, 50);
                            password[strcspn(password, "\n")] = 0;

                            write(new_socket, "Is Manager (1 for Yes, 0 for No): ", 33);
                            read(new_socket, buffer, BUFFER_SIZE);
                            is_manager = atoi(buffer);

                            add_employee(id, name, email, password, is_manager);
                            bzero(buffer, BUFFER_SIZE); // Clear the buffer
                            usleep(100);
                            send(new_socket, admin_menu, strlen(admin_menu), 0);
                        } else if (admin_option == 2) {
                            int num_customers = list_customers();
            if (num_customers == 0) {
                send(new_socket, "No customers found.\n", 20, 0);
            } else {
                char buffer[BUFFER_SIZE];
                write(new_socket, "Select a customer to modify (1-N): ", 35);
                read(new_socket, buffer, BUFFER_SIZE);
                int customer_index = atoi(buffer);

                if (customer_index < 1 || customer_index > num_customers) {
                    send(new_socket, "Invalid selection.\n", 19, 0);
                } else {
                    // Retrieve and display customer details
                    Customer customer;
                    if (get_customer_details(customer_index, &customer)) {
                        char customer_details[BUFFER_SIZE];
                        snprintf(customer_details, BUFFER_SIZE,
                                 "Customer Details:\nName: %s\nEmail: %s\nPhone: %s\nBalance: %.2f\nAccount Active: %d\n",
                                 customer.name, customer.email, customer.phone, customer.balance, customer.account_active);
                        send(new_socket, customer_details, strlen(customer_details), 0);
                    } else {
                        send(new_socket, "Failed to retrieve customer details.\n", 36, 0);
                    }

                    // Provide options to edit customer details
                    const char *edit_menu = "Edit Options:\n1. Name\n2. Email\n3. Phone\n4. Balance\n5. Account Active\n6. Cancel\nSelect an option (1-6): ";
                    send(new_socket, edit_menu, strlen(edit_menu), 0);
                    read(new_socket, buffer, BUFFER_SIZE);
                    int edit_option = atoi(buffer);

                    if (edit_option == 1) {
                        write(new_socket, "Enter new name: ", 16);
                        read(new_socket, customer.name, sizeof(customer.name));
                        customer.name[strcspn(customer.name, "\n")] = 0;
                    } else if (edit_option == 2) {
                        write(new_socket, "Enter new email: ", 17);
                        read(new_socket, customer.email, sizeof(customer.email));
                        customer.email[strcspn(customer.email, "\n")] = 0;
                    } else if (edit_option == 3) {
                        write(new_socket, "Enter new phone: ", 17);
                        read(new_socket, customer.phone, sizeof(customer.phone));
                        customer.phone[strcspn(customer.phone, "\n")] = 0;
                    } else if (edit_option == 4) {
                        write(new_socket, "Enter new balance: ", 19);
                        read(new_socket, buffer, BUFFER_SIZE);
                        customer.balance = atof(buffer);
                    } else if (edit_option == 5) {
                        write(new_socket, "Is account active (1 for Yes, 0 for No): ", 41);
                        read(new_socket, buffer, BUFFER_SIZE);
                        customer.account_active = atoi(buffer);
                    } else if (edit_option == 6) {
                        send(new_socket, "Edit cancelled.\n", 16, 0);
                    } else {
                        send(new_socket, "Invalid option.\n", 16, 0);
                    }

                    if (edit_option >= 1 && edit_option <= 5) {
                        if (modify_customer_details(customer_index, &customer)) {
                            send(new_socket, "Customer details updated successfully.\n", 39, 0);
                        } else {
                            send(new_socket, "Failed to update customer details.\n", 34, 0);
                        }
                    }

                    bzero(buffer, BUFFER_SIZE); // Clear the buffer
                    usleep(100);
                    send(new_socket, admin_menu, strlen(admin_menu), 0);
                }
            }
                        } else if (admin_option == 3) {
                            list_all_employees(new_socket);

    // Select an employee to edit
                            write(new_socket, "Select an employee to modify (ID): ", 35);
                            read(new_socket, buffer, BUFFER_SIZE);
                            int employee_id = atoi(buffer);

                            // Retrieve and display selected employee details
                            Employee emp;
                            if (get_employee_details(employee_id, &emp)) {
                                char employee_details[BUFFER_SIZE];
                                snprintf(employee_details, BUFFER_SIZE,
                                         "Employee Details:\nID: %d\nName: %s\nEmail: %s\nIs Manager: %d\n",
                                         emp.id, emp.name, emp.email, emp.is_manager);
                                send(new_socket, employee_details, strlen(employee_details), 0);
                            } else {
                                send(new_socket, "Failed to retrieve employee details.\n", 36, 0);
                                return;
                            }

                            // Provide options to edit employee details
                            const char *edit_menu = "Edit Options:\n1. Name\n2. Email\n3. Password\n4. Is Manager\n5. Cancel\nSelect an option (1-5): ";
                            send(new_socket, edit_menu, strlen(edit_menu), 0);
                            read(new_socket, buffer, BUFFER_SIZE);
                            int edit_option = atoi(buffer);

                            switch (edit_option) {
                                case 1:
                                    write(new_socket, "Enter new name: ", 16);
                                    read(new_socket, emp.name, sizeof(emp.name));
                                    emp.name[strcspn(emp.name, "\n")] = 0;
                                    break;
                                case 2:
                                    write(new_socket, "Enter new email: ", 17);
                                    bzero(emp.email, sizeof(emp.email)); // Clear the email buffer
                                    read(new_socket, emp.email, sizeof(emp.email));
                                    emp.email[strcspn(emp.email, "\n")] = 0;
                                    break;
                                case 3:
                                    write(new_socket, "Enter new password: ", 20);
                                    read(new_socket, emp.password, sizeof(emp.password));
                                    emp.password[strcspn(emp.password, "\n")] = 0;
                                    break;
                                case 4:
                                    write(new_socket, "Is Manager (1 for Yes, 0 for No): ", 33);
                                    read(new_socket, buffer, BUFFER_SIZE);
                                    emp.is_manager = atoi(buffer);
                                    break;
                                case 5:
                                    send(new_socket, "Edit cancelled.\n", 16, 0);
                                    return;
                                default:
                                    send(new_socket, "Invalid option.\n", 16, 0);
                                    return;
                            }

                            if (edit_option >= 1 && edit_option <= 4) {
                                if (modify_employee(employee_id, emp.name, emp.email, emp.password, emp.is_manager)) {
                                    send(new_socket, "Employee details updated successfully.\n", 39, 0);
                                } else {
                                    send(new_socket, "Failed to update employee details.\n", 34, 0);
                                }
                            }

                            bzero(buffer, BUFFER_SIZE); // Clear the buffer
                            usleep(100); // Add a slight delay to ensure data is fully sent before the next action
                            send(new_socket, admin_menu, strlen(admin_menu), 0);
                        } else if (admin_option == 4) {
                            char new_password[50];
                            write(new_socket, "Enter New Admin Password: ", 26);
                            read(new_socket, new_password, sizeof(new_password));
                            new_password[strcspn(new_password, "\n")] = 0; // Removing the newline character from input
                            if (change_admin_password(email, new_password)) {
                                write(new_socket, "Password changed successfully.\n", 31);
                            } else {
                                write(new_socket, "Failed to change password.\n", 27);
                            }
                            send(new_socket, admin_menu, strlen(admin_menu), 0);
                        } else if (admin_option == 4) {
                            char new_password[50];
                            write(new_socket, "Enter New Admin Password: ", 26);
                            read(new_socket, new_password, 50);
                            new_password[strcspn(new_password, "\n")] = 0; // Removing the newline character from input
                            if (change_admin_password(email, new_password)) {
                                write(new_socket, "Password changed successfully.\n", 31);
                            } else {
                                write(new_socket, "Failed to change password.\n", 27);
                            }
                            send(new_socket, admin_menu, strlen(admin_menu), 0); // Show menu again
                        } else if (admin_option == 5) {
                            // Add customer code
                            int id;
                            char name[50], email[50], phone[20], password[50];
                            double balance;
                            int account_active;

                            write(new_socket, "Enter Customer ID: ", 19);
                            bzero(buffer, BUFFER_SIZE);
                            read(new_socket, buffer, BUFFER_SIZE);
                            id = atoi(buffer);

                            write(new_socket, "Enter Customer Name: ", 21);
                            bzero(buffer, BUFFER_SIZE);
                            read(new_socket, name, 50);
                            name[strcspn(name, "\n")] = 0;

                            write(new_socket, "Enter Customer Email: ", 22);
                            bzero(buffer, BUFFER_SIZE);
                            read(new_socket, email, 50);
                            email[strcspn(email, "\n")] = 0;

                            write(new_socket, "Enter Customer Phone: ", 22);
                            bzero(buffer, BUFFER_SIZE);
                            read(new_socket, phone, 20);
                            phone[strcspn(phone, "\n")] = 0;

                            write(new_socket, "Enter Customer Password: ", 25);
                            bzero(buffer, BUFFER_SIZE);
                            read(new_socket, password, 50);
                            password[strcspn(password, "\n")] = 0;

                            write(new_socket, "Enter Customer Balance: ", 24);
                            bzero(buffer, BUFFER_SIZE);
                            read(new_socket, buffer, BUFFER_SIZE);
                            balance = atof(buffer);

                            write(new_socket, "Is Account Active (1 for Yes, 0 for No): ", 41);
                            bzero(buffer, BUFFER_SIZE);
                            read(new_socket, buffer, BUFFER_SIZE);
                            account_active = atoi(buffer);

                            add_customer(id, name, email, phone, password, balance, account_active);
                            bzero(buffer, BUFFER_SIZE); // Clear the buffer
                            usleep(100); // Add a slight delay to ensure data is fully sent before the next action
                            send(new_socket, admin_menu, strlen(admin_menu), 0);
                        } else if (admin_option == 6) {
                            logout_admin(email);
                            write(new_socket,"Logout Successfull", 19);
                            bzero(buffer, BUFFER_SIZE);
                            usleep(100);
                            send(new_socket, menu, strlen(menu), 0);
                            break;
                        } else {
                            write(new_socket, "Invalid option. Please select again\n", 37);
                        }
                    }
                } else {
                    write(new_socket, "Login failed. Invalid credentials.\n", 35);
                    bzero(buffer, BUFFER_SIZE); // Clear the buffer
                    usleep(100); // Ensure the message is fully sent
                    send(new_socket, menu, strlen(menu), 0); // Show main menu again
                }
                break;
            }
            default:
                write(new_socket, "Invalid option. Please select again\n", 37);
                bzero(buffer, BUFFER_SIZE); // Clear the buffer
                usleep(100); // Add delay to ensure data is sent properly
                send(new_socket, menu, strlen(menu), 0);
                break;
        }
    }

    if (read_size == 0) {
        printf("Client disconnected\n");
    } else if (read_size == -1) {
        perror("recv failed");
    }

    close(new_socket);
    free(socket_desc);
    return NULL;
}

int main() {
    int server_fd, new_socket, *new_sock;
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);
    int opt = 1;

    if (pthread_mutex_init(&customer_edit_mutex, NULL) != 0) {
        perror("Mutex init failed");
        return 1;
    }

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGPIPE, SIG_IGN);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Could not create socket");
        return 1;
    }

    // Set the SO_REUSEADDR option
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return 1;
    }

    listen(server_fd, 3);

    printf("Waiting for incoming connections...\n");
    while ((new_socket = accept(server_fd, (struct sockaddr *)&client, &client_len))) {
        printf("Connection accepted\n");

        pthread_t client_thread;
        new_sock = malloc(1);
        *new_sock = new_socket;

        if (pthread_create(&client_thread, NULL, handle_client, (void *)new_sock) < 0) {
            perror("Could not create thread");
            close(new_socket);
            free(new_sock);
            continue;
        }

        printf("Handler assigned\n");
    }

    if (new_socket < 0) {
        perror("Accept failed");
        close(server_fd);
        return 1;
    }
    pthread_mutex_destroy(&customer_edit_mutex);

    close(server_fd);
    return 0;
}