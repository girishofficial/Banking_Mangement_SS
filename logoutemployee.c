#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include "db.h"

int set_all_employees_logged_out() {
    const char *file_path = "/home/girish-pc/projecter/db/employees.txt";
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

    Employee employee;
    while (fread(&employee, sizeof(Employee), 1, file) == 1) {
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
            flock(fd, LOCK_UN); // Unlock the file
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
    }

    // Unlock the file
    flock(fd, LOCK_UN);
    fclose(file);
    return 1; // Update successful
}

int main() {
    if (set_all_employees_logged_out()) {
        printf("All employees logged out successfully.\n");
    } else {
        printf("Failed to log out all employees.\n");
    }
    return 0;
}