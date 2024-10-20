#include <stdio.h>
#include <string.h>

#include "db.h"

int set_logged_out(int customer_id) {
    const char *file_path = "/home/girish-pc/projecter/db/customers.txt";
    FILE *file = fopen(file_path, "r+b");
    if (file == NULL) {
        perror("Failed to open file");
        return 0;
    }

    Customer customer;
    while (fread(&customer, sizeof(Customer), 1, file) == 1) {
        if (customer.id == customer_id) {
            // Print customer details before change
            printf("Customer Details Before Change:\n");
            printf("ID: %d\n", customer.id);
            printf("Name: %s\n", customer.name);
            printf("Email: %s\n", customer.email);
            printf("Phone: %s\n", customer.phone);
            printf("Password: %s\n", customer.password);
            printf("Logged In: %d\n", customer.logged_in);

            // Update logged_in status
            customer.logged_in = 0;
            fseek(file, -sizeof(Customer), SEEK_CUR); // Move the file pointer back to the start of the customer record
            if (fwrite(&customer, sizeof(Customer), 1, file) != 1) {
                perror("Failed to update login status");
                fclose(file);
                return 0;
            }

            // Print customer details after change
            printf("Customer Details After Change:\n");
            printf("ID: %d\n", customer.id);
            printf("Name: %s\n", customer.name);
            printf("Email: %s\n", customer.email);
            printf("Phone: %s\n", customer.phone);
            printf("Password: %s\n", customer.password);
            printf("Logged In: %d\n", customer.logged_in);

            fclose(file);
            return 1; // Update successful
        }
    }

    fclose(file);
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