#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOGIN_STATUS_FILE "/home/girish-pc/projecter/db/login_status.txt"

// Function to check if a specific customer is logged in
int is_customer_logged_in(int customer_id) {
    FILE *file = fopen(LOGIN_STATUS_FILE, "r");
    if (file == NULL) {
        // File does not exist, create it and set initial status to 0
        file = fopen(LOGIN_STATUS_FILE, "w");
        if (file == NULL) {
            perror("Failed to create login status file");
            exit(1);
        }
        fprintf(file, "%d", 0);
        fclose(file);
        printf("Login status file created with initial status 0.\n");
        return 0; // No user is logged in
    }

    int logged_in_id;
    if (fscanf(file, "%d", &logged_in_id) != 1) {
        fprintf(stderr, "Failed to read login status\n");
        fclose(file);
        return 0; // Default to no user logged in if read fails
    }
    fclose(file);

    printf("Login status read from file: %d\n", logged_in_id);
    return logged_in_id == customer_id;
}

// Function to set the logged-in status with customer ID
void set_logged_in_status(int customer_id) {
    FILE *file = fopen(LOGIN_STATUS_FILE, "w");
    if (file == NULL) {
        perror("Failed to open login status file");
        exit(1);
    }

    if (fprintf(file, "%d", customer_id) < 0) {
        perror("Failed to write login status");
    } else {
        printf("Login status set to: %d\n", customer_id);
    }
    fclose(file);
}