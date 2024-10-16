#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "db.h"

int main() {
    const char* file_path = "/home/girish-pc/projecter/db/employees.txt";
    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return 1;
    }

    Employee emp;
    while (read(fd, &emp, sizeof(Employee)) > 0) {
        printf("Employee ID: %d\n", emp.id);
        printf("Name: %s\n", emp.name);
        printf("Email: %s\n", emp.email);
        printf("Is Manager: %d\n", emp.is_manager);
        printf("Password: %s\n", emp.password);
        printf("\n");
    }

    close(fd);
    return 0;
}