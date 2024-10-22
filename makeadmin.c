#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "db.h"

int add_admin() {
    const char *file_path = "/home/girish-pc/projecter/db/admins.txt";
    int fd = open(file_path, O_RDWR | O_APPEND);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }

    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_END;
    lock.l_start = 0;
    lock.l_len = sizeof(Admin);

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Failed to lock record");
        close(fd);
        return 0;
    }

    Admin new_admin;

    printf("Enter ID: ");
    scanf("%d", &new_admin.id);

    printf("Enter Email: ");
    scanf(" %49[^\n]", new_admin.email);

    printf("Enter Password: ");
    scanf(" %49[^\n]", new_admin.password);

    new_admin.logged_in = 0;

    if (write(fd, &new_admin, sizeof(Admin)) == -1) {
        perror("Failed to write to file");
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
        close(fd);
        return 0;
    }

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
    return 1;
}

int main() {
    add_admin();
    return 0;
}