#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

void *xmalloc(size_t size)
{
    void *p;

    p = malloc(size);
    if (p == NULL) {
        printf(":: malloc size %d failed, %s\n", (int)size, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return p;
}

int show_file_attr(const struct stat *st)
{
    if (!st)
        return -1;

    printf(":: ID of device containing file: (%d, %d)\n", major(st->st_dev), minor(st->st_dev));
    printf(":: number of hard links: %d\n", st->st_nlink);
    printf(":: user ID of owner: %d\n", st->st_uid);
    printf(":: group ID of owner: %d\n", st->st_gid);
    printf(":: total size, in bytes: %d\n", st->st_size);
    printf(":: blocksize for filesystem I/O: %d\n", st->st_blksize);

    if (S_ISREG(st->st_mode))
        printf(":: It's a regular file\n");
    if (S_ISDIR(st->st_mode))
        printf(":: It's a directory\n");
    if (S_ISCHR(st->st_mode)) {
        printf(":: It's character device\n");
        printf(":: device ID: (%d, %d)\n", major(st->st_rdev), minor(st->st_rdev));
    }
    if (S_ISBLK(st->st_mode)) {
        printf(":: It's block device\n");
        printf(":: device ID: (%d, %d)\n", major(st->st_rdev), minor(st->st_rdev));
    }
    if (S_ISFIFO(st->st_mode))
        printf(":: It's FIFO (named pipe)\n");
    if (S_ISLNK(st->st_mode))
        printf(":: It's symbolic link\n");
    if (S_ISSOCK(st->st_mode))
        printf(":: It's socket\n");

    /* permissions */
    printf(":: owner permissions - %03o\n", st->st_mode & S_IRWXU);
    printf(":: group permissions - %03o\n", st->st_mode & S_IRWXG);
    printf(":: other permissions - %03o\n", st->st_mode & S_IRWXO);

    if (st->st_mode & S_ISVTX)
        printf(":: sticky bit set\n");
    if (st->st_mode & S_ISGID)
        printf(":: set-group-ID bit set\n");
    if (st->st_mode & S_ISUID)
        printf(":: set-user-ID bit bit set\n");

    return 0;
}

int file_attr(const char *filename)
{
    struct stat st;

    if (lstat(filename, &st) == -1) {
        printf(":: lstat with %s failed, %s\n", filename, strerror(errno));
        return -1;
    }

    show_file_attr(&st);

    return 0;
}

int main(int argc, char **argv)
{
    char *filename;

    if (argc > 1)
        filename = argv[1];
    else
        filename = __FILE__;

    file_attr(filename);

    exit(EXIT_SUCCESS);
}
