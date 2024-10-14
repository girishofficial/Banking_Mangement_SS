//
// Created by girish-pc on 10/15/24.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int verify_manager(const char *email, const char *password) {
    FILE *file = fopen("employee.txt", "r");
    if (file == NULL) {
        perror("Could not open employee.txt");
        return 0;
    }

    char file_email[50], file_password[50];
    int id, is_manager;
    char name[50];

    while (fscanf(file, "%d %49s %49s %49s %d", &id, name, file_email, file_password, &is_manager) == 5) {
        if (is_manager == 1 && strcmp(email, file_email) == 0 && strcmp(password, file_password) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}