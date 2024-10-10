#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include "../db.h"
#include "admintasks.h"

int verify_admin(const char *email, const char *password) {
    const char* file_path = "/home/girish-pc/projecter/db/admins.txt";
    char stored_email[50], stored_password[50];
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("Failed to open file db/admins.txt");
        return -1;
    }

    while (fscanf(file, "%49[^,],%49s\n", stored_email, stored_password) != EOF) {
        if (strcmp(stored_email, email) == 0 && strcmp(stored_password, password) == 0) {
            fclose(file);
            return 1; // Record found
        }
    }

    fclose(file);
    return 0; // Record not found
}

int lookup(const char *id) {
    const char* file_path = "/home/girish-pc/projecter/db/employees.txt";
    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }
    Employee emp;
    while (read(fd, &emp, sizeof(emp)) > 0) {
        if (strcmp(id, emp.id) == 0) {
            close(fd);
            return 1; // ID already exists
        }
    }
    close(fd);
    return 0;
}

int add_employee(int id, const char *name, const char *email, const char *password, int is_manager) {
    const char* file_path = "/home/girish-pc/projecter/db/employees.txt";
    FILE *file = fopen(file_path, "a");
    if (file == NULL) {
        perror("Failed to open file");
        return 0;
    }

    Employee emp;
    emp.id = id;
    strncpy(emp.name, name, sizeof(emp.name) - 1);
    strncpy(emp.email, email, sizeof(emp.email) - 1);
    strncpy(emp.password, password, sizeof(emp.password) - 1);
    emp.is_manager = is_manager;

    fwrite(&emp, sizeof(Employee), 1, file);
    fclose(file);
    return 1;
}

int modify_employee(int id, const char *name, const char *email, const char *password, int is_manager) {
    const char *file_path = "/home/girish-pc/projecter/db/employees.txt";
    int fd = open(file_path, O_WRONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }
    Employee emp;
    while (read(fd, &emp, sizeof(emp)) > 0) {
        if (emp.id == id) {
            lseek(fd, -sizeof(emp), SEEK_CUR);
            strncpy(emp.name, name, sizeof(emp.name) - 1);
            strncpy(emp.email, email, sizeof(emp.email) - 1);
            strncpy(emp.password, password, sizeof(emp.password) - 1);
            emp.is_manager = is_manager;
            write(fd, &emp, sizeof(emp));
            close(fd);
            return 1;
        }
    }
    close(fd);
    return 0;
}

int manage_user_roles(int id, int new_role) {
    const char *file_path = "/home/girish-pc/projecter/db/employees.txt";
    int fd = open(file_path, O_RDWR);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    Employee emp;
    while (read(fd, &emp, sizeof(emp)) > 0) {
        if (emp.id == id) {
            lseek(fd, -sizeof(emp), SEEK_CUR);
            emp.is_manager = new_role;
            write(fd, &emp, sizeof(emp));
            close(fd);
            return 1;
        }
    }

    close(fd);
    return 0;
}


int change_admin_password(const char *email, const char *new_password) {
    const char *file_path = "/home/girish-pc/projecter/db/admins.txt";
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return 0;
    }

    char line[256];
    char temp_path[] = "/home/girish-pc/projecter/db/temp.txt";
    FILE *temp_file = fopen(temp_path, "w");
    if (temp_file == NULL) {
        perror("Failed to open temp file");
        fclose(file);
        return 0;
    }

    int found = 0;

    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");
        if (token && strcmp(token, email) == 0) {
            found = 1;
            fprintf(temp_file, "%s,%s\n", email, new_password);
        } else {
            fprintf(temp_file, "%s", line);
        }
    }

    fclose(file);
    fclose(temp_file);

    if (found) {
        remove(file_path);
        rename(temp_path, file_path);
    } else {
        remove(temp_path);
    }

    return found;
}