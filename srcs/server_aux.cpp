#include "irc.hpp"

void add_to_poll_fds(struct pollfd *poll_fds[], int new_fd, int *poll_count, int *poll_size) 
{   
    if (*poll_count == *poll_size) {
        *poll_size *= 2;
        *poll_fds = (pollfd*)realloc(*poll_fds, sizeof(**poll_fds) * (*poll_size));
        if (!*poll_fds) {
            perror("realloc failed");
            exit(EXIT_FAILURE);
        }
    }

    (*poll_fds)[*poll_count].fd = new_fd;
    (*poll_fds)[*poll_count].events = POLLIN;
    (*poll_count)++;
}

void del_from_poll_fds(struct pollfd **poll_fds, int i, int *poll_count) 
{
    // Copie le fd de la fin du tableau à cet index
    (*poll_fds)[i] = (*poll_fds)[*poll_count - 1];
    (*poll_count)--;
}