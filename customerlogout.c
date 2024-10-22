#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include "db.h"

int set_all_customers_logged_out() {
    const char *file_path = "/home/girish-pc/projecter/db/customers.txt";
    FILE *file = fopen(file_path, "r+b");
    if (file == NULL) {
        perror("Failed to open file");
        return 0;
    }

    // Apply an exclusive lock
    int fd = fileno(file);
    if (flock(fd, LOCK_EX) == -1) {
        perror("Failed to lock file");
        fclose(file);
        return 0;
    }

    Customer customer;
    while (fread(&customer, sizeof(Customer), 1, file) == 1) {
        // Print customer details before change
//        printf("Customer Details Before Change:\n");
//        printf("ID: %d\n", customer.id);
//        printf("Name: %s\n", customer.name);
//        printf("Email: %s\n", customer.email);
//        printf("Phone: %s\n", customer.phone);
//        printf("Password: %s\n", customer.password);
//        printf("Logged In: %d\n", customer.logged_in);

        // Update logged_in status
        customer.logged_in = 0;
        fseek(file, -sizeof(Customer), SEEK_CUR); // Move the file pointer back to the start of the customer record
        if (fwrite(&customer, sizeof(Customer), 1, file) != 1) {
            perror("Failed to update login status");
            flock(fd, LOCK_UN); // Unlock the file
            fclose(file);
            return 0;
        }

        // Print customer details after change
//        printf("Customer Details After Change:\n");
//        printf("ID: %d\n", customer.id);
//        printf("Name: %s\n", customer.name);
//        printf("Email: %s\n", customer.email);
//        printf("Phone: %s\n", customer.phone);
//        printf("Password: %s\n", customer.password);
//        printf("Logged In: %d\n", customer.logged_in);
    }

    // Unlock the file
    flock(fd, LOCK_UN);
    fclose(file);
    return 1; // Update successful
}

int main() {
    if (set_all_customers_logged_out()) {
        printf("All customers logged out successfully.\n");
    } else {
        printf("Failed to log out all customers.\n");
    }
    return 0;
}