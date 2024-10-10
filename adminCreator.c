#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "db.h"

void add_admin() {
    Admin admin;
    const int fd = open("db/admins.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);

    if (fd == -1) {
        perror("Failed to open file db/admins.txt");
        return;
    }

    printf("Enter Email: ");
    scanf(" %49[^\n]", admin.email);  // Read a line of text with spaces

    printf("Enter Password: ");
    scanf(" %49[^\n]", admin.password);  // Read a line of text with spaces

    // Write the admin email and password to the file
    dprintf(fd, "%s,%s\n", admin.email, admin.password);
    printf("Admin added successfully!\n");

    close(fd);
}

int main() {
    add_admin();
    return 0;
}