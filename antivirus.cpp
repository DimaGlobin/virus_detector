#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/fanotify.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define EVENT_SIZE  (sizeof(struct fanotify_event_metadata))
#define EVENT_BUF_LEN   (1024 * (EVENT_SIZE + 16))

void kill_process(pid_t pid) {

    /*
    
    This function kill the process by its id.

    input:
    pid_t pid - id of the process that you want to kill
    
    */

    if (kill(pid, SIGKILL) == -1) {

        perror("kill");

    }
}

int main(int argc, char *argv[]) {

    int fd = 0, poll_num = 0;
    nfds_t nfds = 1;
    char buffer[EVENT_BUF_LEN];
    struct fanotify_event_metadata* metadata = NULL;

    ssize_t len = 0;
    struct pollfd fds[1]{};

    fd = fanotify_init(FAN_CLOEXEC | FAN_CLASS_CONTENT | FAN_NONBLOCK, O_RDONLY | O_LARGEFILE); //Call fanotify_init to initialize a new fanotify group

    if (fd == -1) {

        perror("fanotify_init");
        exit(EXIT_FAILURE);

    }

    if (fanotify_mark(fd, FAN_MARK_ADD | FAN_MARK_MOUNT, FAN_OPEN_PERM | FAN_CLOSE_WRITE | FAN_EVENT_ON_CHILD, AT_FDCWD, "simple_test") == -1) { //Mark filesystem object by fd
        
        perror("fanotify_mark");
        exit(EXIT_FAILURE);

    }   

    fds[0].fd = fd;
    fds[0].events = POLLIN;

    while (1) {

        poll_num = poll(fds, nfds, -1); // poll() waits for events

        if (poll_num == -1) {

            if (errno == EINTR) {
            
                continue;
            }

            perror("poll");
            exit(EXIT_FAILURE);

        }

        if (poll_num > 0) {

            if (fds[0].revents & POLLIN) { // Chek if there is data to read

                len = read(fd, buffer, EVENT_BUF_LEN);

                if (len == -1) {

                    perror("read");
                    exit(EXIT_FAILURE);

                }

                metadata = (struct fanotify_event_metadata*  )buffer;

                while (FAN_EVENT_OK(metadata, len)) {
                
                
                    if (metadata -> mask & FAN_OPEN_PERM || metadata -> mask & FAN_CLOSE_WRITE) { // Look if file was opened
                        
                        printf("Virus detected in process %d\n", metadata -> pid);
                        kill_process(metadata -> pid); // Kill this process
                    }
                
                    metadata = FAN_EVENT_NEXT(metadata, len);

                }

            }

        }

    }

    return 0;
}