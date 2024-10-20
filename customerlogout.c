#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    int id;
    char name[50];
    char email[50];
    char phone[15];
    char password[20];
    int logged_in;
} Customer;

int set_logged_out(int customer_id) {
    const char *file_path = "/home/girish-pc/projecter/db/customers.txt";
    int fd = open(file_path, O_RDWR);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    Customer customer;
    off_t pos;
    while ((pos = lseek(fd, 0, SEEK_CUR)) != -1 && read(fd, &customer, sizeof(Customer)) > 0) {
        if (customer.id == customer_id) {
            customer.logged_in = 0;
            lseek(fd, pos, SEEK_SET); // Move the file pointer back to the start of the customer record
            if (write(fd, &customer, sizeof(Customer)) == -1) {
                perror("Failed to update login status");
                close(fd);
                return 0;
            }
            printf("Customer Details:\n");
            printf("ID: %d\n", customer.id);
            printf("Name: %s\n", customer.name);
            printf("Email: %s\n", customer.email);
            printf("Phone: %s\n", customer.phone);
            printf("Password: %s\n", customer.password);
            printf("Logged In: %d\n", customer.logged_in);
            close(fd);
            return 1; // Update successful
        }
    }

    close(fd);
    return 0; // Customer not found
}

int main() {
    int customer_id = 1; // Replace with the actual customer ID
    if (set_logged_out(customer_id)) {
        printf("Customer logged out successfully.\n");
    } else {
        printf("Failed to log out customer.\n");
    }
    return 0;
}