//
// Created by girish-pc on 10/20/24.
//
#include <stdio.h>
#include <string.h>

#include "db.h"

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
            // Print employee details before change
            printf("Employee Details Before Change:\n");
            printf("ID: %d\n", employee.id);
            printf("Name: %s\n", employee.name);
            printf("Email: %s\n", employee.email);
            printf("Password: %s\n", employee.password);
            printf("Is Manager: %d\n", employee.is_manager);
            printf("Logged In: %d\n", employee.logged_in);

            // Update logged_in status
            employee.logged_in = 0;
            fseek(file, -sizeof(Employee), SEEK_CUR); // Move the file pointer back to the start of the employee record
            if (fwrite(&employee, sizeof(Employee), 1, file) != 1) {
                perror("Failed to update login status");
                fclose(file);
                return 0;
            }

            // Print employee details after change
            printf("Employee Details After Change:\n");
            printf("ID: %d\n", employee.id);
            printf("Name: %s\n", employee.name);
            printf("Email: %s\n", employee.email);
            printf("Password: %s\n", employee.password);
            printf("Is Manager: %d\n", employee.is_manager);
            printf("Logged In: %d\n", employee.logged_in);

            fclose(file);
            return 1; // Update successful
        }
    }

    fclose(file);
    return 0; // Employee not found
}

int main() {
    int employee_id = 1; // Replace with the actual employee ID
    if (set_employee_logged_out(employee_id)) {
        printf("Employee logged out successfully.\n");
    } else {
        printf("Failed to log out employee.\n");
    }
    return 0;
}