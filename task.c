#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#define LOCK_SUFFIX ".lck"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    char lockfile[strlen(argv[1]) + strlen(LOCK_SUFFIX) + 1];
    sprintf(lockfile, "%s%s", argv[1], LOCK_SUFFIX);

    int lock_fd;
    pid_t pid = getpid();
    char pid_buf[16];
    sprintf(pid_buf, "%d", pid);
    struct timespec tw;
    tw.tv_sec = 0L;
    tw.tv_nsec = 100000000L;
     while (access(lockfile, F_OK) == 0) {
        nanosleep(&tw, NULL);
    }

    lock_fd = open(lockfile, O_WRONLY | O_CREAT | O_EXCL, 0777);
    if (lock_fd == -1) {
    	printf("Failed to create lock file: %s\n", strerror(errno));
    	return 1;
    }
    int bytes_written = write(lock_fd, pid_buf, strlen(pid_buf));
    if (bytes_written != strlen(pid_buf)) {
        printf("Failed to write to lock file: %s\n", strerror(errno));
        return 1;
    }
    close(lock_fd);

    int fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        printf("Failed to open file: %s\n", strerror(errno));
        return 1;
    }

    sleep(1);
    close(fd);

    if (access(lockfile, F_OK) == 0) {
        FILE *lock_fd = fopen(lockfile, "r");
        fseek(lock_fd, 0, SEEK_END);
        int size = ftell(lock_fd);
        char *pidstr = malloc(size);
        fseek(lock_fd, 0, SEEK_SET);
        fread(pidstr, 1, size, lock_fd);
        int pid = atoi(pidstr);
        if (pid != getpid()) {
            printf("Lock file was modified by another process\n");
            return 1;
        }
        fclose(lock_fd);
        if (unlink(lockfile) == -1) {
        	printf("Failed to delete lock file: %s\n", strerror(errno));
        	return 1;
        }
    } else {
        printf("Lock file not exist\n");
        return 1;
    }

return 0;

}

        
