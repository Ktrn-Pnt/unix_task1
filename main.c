#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>

#define ERROR_COUNT_ARGUMENTS "incorrect number of arguments\n"
#define ERROR_INCORRECT_BLOCK_SIZE "incorrect block size\n"
#define ERROR_INCORRECT_ORIGIN "not open origin file\n"
#define ERROR_INCORRECT_PURPOSE "not open purpose file\n"
#define ERROR_READ_ORIGIN "not read origin file\n"
#define ERROR_WRITE_PURPOSE "not write purpose file\n"
#define ERROR_LSEEK "seek error"
#define BLOCK_SIZE 4096

void close_fd(int origin_fd, int purpose_fd) {
    close(origin_fd);
    close(purpose_fd);
}


void print_error_with_close(char* error, int origin_fd, int purpose_fd) {
    close_fd(origin_fd, purpose_fd);
    printf("%s", error);
}

int open_purpose(char* file) {
    return open(file, O_WRONLY | O_CREAT | O_TRUNC, 
    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
}

int open_origin(char* file) {
    return open(file, O_RDONLY);
}

int get_block_size(int argc, char* argv[]) {
    int block_size = BLOCK_SIZE;
    int param;

    while ((param = getopt(argc, argv, "b:")) != -1) {
        if (param == 'b') {
            block_size = atoi(optarg);
        }
    }

    return block_size;
}

int main(int argc, char* argv[]) {
   
    int purpose_fd, origin_fd;

    int block_size = get_block_size(argc, argv);
    if (block_size <= 0) {
        printf(ERROR_INCORRECT_BLOCK_SIZE);
        return 1;
    }

    if (argc - optind > 2) {
        printf(ERROR_COUNT_ARGUMENTS);
        return 1;
    }

    if (argc - optind == 1) { 
        origin_fd = STDIN_FILENO;
        purpose_fd = open_purpose(argv[optind]);
    } else {
        origin_fd = open_origin(argv[optind]);
        purpose_fd = open_purpose(argv[++optind]);
    }

    if (purpose_fd == -1) {
        print_error_with_close(ERROR_INCORRECT_PURPOSE, origin_fd, purpose_fd);
        return -1;
    }

    if (origin_fd == -1) {
        print_error_with_close(ERROR_INCORRECT_ORIGIN, origin_fd, purpose_fd);
        return -1;
    }

    char buffer[block_size];
    int is_empty;
    int buffer_size, write_result, lseek_result;

    while (buffer_size = read(origin_fd, buffer, block_size)) {

        if (buffer_size == -1) {
            print_error_with_close(ERROR_READ_ORIGIN, origin_fd, purpose_fd);
            return -1;
        }

        is_empty = 1;
        for (int i = 0; i < buffer_size; i++) {
            if (buffer[i] != 0) {
                is_empty = 0;
                break;
            }
        }

        if (is_empty) {
            lseek_result = lseek(purpose_fd, buffer_size, SEEK_CUR);
            if (lseek_result == -1) {
                print_error_with_close(ERROR_LSEEK, origin_fd, purpose_fd);
                return -1;
            }
        } else {
            write_result = write(purpose_fd, buffer, buffer_size);
            if (write_result == -1) {
                print_error_with_close(ERROR_WRITE_PURPOSE, origin_fd, purpose_fd);
                return -1;
            }
        }
    }

    close_fd(origin_fd, purpose_fd);

    return 0;
}

