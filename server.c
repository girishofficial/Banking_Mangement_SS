#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "db.h"
#include "admin/admintasks.h"
#include "/home/girish-pc/projecter/customer/customertask.h"

#define PORT 8087
#define BUFFER_SIZE 102400

void *handle_client(void *socket_desc) {
    int new_socket = *(int *)socket_desc;
    char buffer[BUFFER_SIZE];
    int read_size;

    const char *menu = "Select an option:\n"
                       "1. Customer Login\n"
                       "2. Employee Login\n"
                       "3. Manager Login\n"
                       "4. Admin Login\n";
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
                } else {
                    int result = verify_customer(customer_id, password);
                    if (result == 1) {
                        write(new_socket, "Login successful.\n", 18);
                        const char *customer_menu = "Customer Options:\n"
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
                                    // Transfer Funds code
                                    break;
                                case 5:
                                    // Apply for a Loan code
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
                                    if (view_transaction_history(customer_id, new_socket)) {
                                        send(new_socket, "Transaction history displayed.\n", 31, 0);
                                        bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                        usleep(100);
                                        send(new_socket, customer_menu, strlen(customer_menu), 0);
                                    } else {
                                        send(new_socket, "Failed to display transaction history.\n", 39, 0);
                                        bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                        usleep(100);
                                        send(new_socket, customer_menu, strlen(customer_menu), 0);
                                    }
                                    break;
                                case 9:
                                    write(new_socket, "Logged out successfully.\n", 26);
                                    bzero(buffer, BUFFER_SIZE); // Clear the buffer
                                    usleep(100); // Add a slight delay to ensure data is fully sent before the next action
                                    send(new_socket, menu, strlen(menu), 0);
                                    break;
                                case 10:
                                    write(new_socket, "Exiting...\n", 11);
                                    close(new_socket);
                                    free(socket_desc);
                                    return NULL;
                                default:
                                    write(new_socket, "Invalid option. Please select again\n", 37);
                            }
                        }
                    } else {
                        write(new_socket, "Login failed. Invalid credentials.\n", 35);
                        bzero(buffer, BUFFER_SIZE); // Clear the buffer
                        usleep(100); // Ensure the message is fully sent
                        send(new_socket, menu, strlen(menu), 0); // Show main menu again
                    }
                }
                break;
            }
            case 2:
                write(new_socket, "Employee Login\n", 23);
                break;
            case 3:
                write(new_socket, "Manager Login\n", 23);
                break;
            case 4: {
                char email[50], password[50];
                write(new_socket, "Enter Admin Email: ", 19);
                read(new_socket, email, 50);
                email[strcspn(email, "\n")] = 0; // Removing the newline character from input
                printf("Received email: %s\n", email); // Debugging line

                write(new_socket, "Enter Admin Password: ", 21);
                read(new_socket, password, 50);
                password[strcspn(password, "\n")] = 0; // Removing the newline character from input
                printf("Received password: %s\n", password); // Debugging line

                int result = verify_admin(email, password);
                if (result == 1) {
                    write(new_socket, "Login successful.\n", 18);
                    const char *admin_menu = "Admin Options:\n"
                                             "1. Add Employee\n"
                                             "2. Modify Employee\n"
                                             "3. Manage User Roles\n"
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

                            write(new_socket, "Enter Employee Password: ", 25);
                            read(new_socket, password, 50);
                            password[strcspn(password, "\n")] = 0;

                            write(new_socket, "Is Manager (1 for Yes, 0 for No): ", 33);
                            read(new_socket, buffer, BUFFER_SIZE);
                            is_manager = atoi(buffer);

                            add_employee(id, name, email, password, is_manager);
                        } else if (admin_option == 2) {
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

                            write(new_socket, "Enter Employee Password: ", 25);
                            read(new_socket, password, 50);
                            password[strcspn(password, "\n")] = 0;

                            write(new_socket, "Is Manager (1 for Yes, 0 for No): ", 33);
                            read(new_socket, buffer, BUFFER_SIZE);
                            is_manager = atoi(buffer);

                            modify_employee(id, name, email, password, is_manager);
                        } else if (admin_option == 3) {
                            // Manage user roles
                            int user_id, new_role;
                            write(new_socket, "Enter User ID: ", 15);
                            read(new_socket, buffer, BUFFER_SIZE);
                            user_id = atoi(buffer);

                            write(new_socket, "Enter New Role (1 for Manager, 0 for Employee): ", 48);
                            read(new_socket, buffer, BUFFER_SIZE);
                            new_role = atoi(buffer);

                            if (manage_user_roles(user_id, new_role)) {
                                write(new_socket, "User role updated successfully\n", 31);
                            } else {
                                write(new_socket, "Failed to update user role\n", 27);
                            }
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

                            write(new_socket, "Enter Customer Password: ", 25);
                            read(new_socket, password, 50);
                            password[strcspn(password, "\n")] = 0;

                            write(new_socket, "Enter Customer Balance: ", 24);
                            read(new_socket, buffer, BUFFER_SIZE);
                            balance = atof(buffer);

                            write(new_socket, "Is Account Active (1 for Yes, 0 for No): ", 41);
                            read(new_socket, buffer, BUFFER_SIZE);
                            account_active = atoi(buffer);

                            add_customer(id, name, email, phone, password, balance, account_active);
                            bzero(buffer, BUFFER_SIZE); // Clear the buffer
                            usleep(100); // Add a slight delay to ensure data is fully sent before the next action
                            send(new_socket, admin_menu, strlen(admin_menu), 0);
                        } else if (admin_option == 6) {
                            write(new_socket, "Logged out successfully.\n", 26);
                            bzero(buffer, BUFFER_SIZE); // Clear the buffer
                            usleep(100); // Add a slight delay to ensure data is fully sent before the next action
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

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Could not create socket");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
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
            return 1;
        }

        printf("Handler assigned\n");
    }

    if (new_socket < 0) {
        perror("Accept failed");
        return 1;
    }

    close(server_fd);
    return 0;
}