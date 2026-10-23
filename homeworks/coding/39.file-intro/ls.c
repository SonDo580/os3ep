/*
List Files:

Write a program that lists files in the given directory.

When called without any arguments, the program should just print the file names.

When invoked with the '-l' flag, the program should print out information about each file,
such as the owner, group, permissions, and other information obtained from the stat() system call.

The program should take one additional argument, which is the directory to read.
If no directory is given, the program should just use the current working directory.

Useful interfaces: stat(), opendir(), readdir(), getcwd().
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

void get_perm_str(char *buf, mode_t mode)
{
    // Type
    if (S_ISDIR(mode))
        buf[0] = 'd'; // directory
    else if (S_ISLNK(mode))
        buf[0] = 'l'; // symbolic link
    else
        buf[0] = '-'; // regular file
    // (there are more types)

    // User permissions
    buf[1] = (mode & S_IRUSR) ? 'r' : '-';
    buf[2] = (mode & S_IWUSR) ? 'w' : '-';
    buf[3] = (mode & S_IXUSR) ? 'x' : '-';

    // Group permissions
    buf[4] = (mode & S_IRGRP) ? 'r' : '-';
    buf[5] = (mode & S_IWGRP) ? 'w' : '-';
    buf[6] = (mode & S_IXGRP) ? 'x' : '-';

    // Other permissions
    buf[7] = (mode & S_IROTH) ? 'r' : '-';
    buf[8] = (mode & S_IWOTH) ? 'w' : '-';
    buf[9] = (mode & S_IXOTH) ? 'x' : '-';

    buf[10] = '\0';
}

void print_stat(struct stat sbuf)
{
    printf("\tInode number: %lu\n", (unsigned long)sbuf.st_ino);
    printf("\tFile size (bytes): %lld\n", (long long)sbuf.st_size);
    printf("\tNumber of (512-byte) blocks: %lld\n", (long long)sbuf.st_blocks);
    printf("\tReference count: %lu\n", (unsigned long)sbuf.st_nlink);
    printf("\tLast modified: %s", ctime(&sbuf.st_mtim.tv_sec));

    struct passwd *pwd = getpwuid(sbuf.st_uid);
    if (pwd != NULL)
        printf("\tOwner: %s\n", pwd->pw_name);
    else
        printf("\tOwner UID: %u\n", sbuf.st_uid);

    struct group *grp = getgrgid(sbuf.st_gid);
    if (grp != NULL)
        printf("\tGroup: %s\n", grp->gr_name);
    else
        printf("\tGroup GID: %u\n", sbuf.st_gid);

    char perm_str[11];
    get_perm_str(perm_str, sbuf.st_mode);
    printf("\tTypes & Permissions: %s\n", perm_str);
}

int main(int argc, char *argv[])
{
    int opt;
    int need_stat = 0;
    char *curr_dir = getcwd(NULL, 0);
    char *dir;

    while ((opt = getopt(argc, argv, "l")) != -1)
    {
        switch (opt)
        {
        case 'l':
            need_stat = 1;
            break;
        default:
            fprintf(stderr, "usage: %s [-l] [<dir>]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (argc - optind > 1)
    {
        fprintf(stderr, "usage: %s [-l] [<dir>]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    dir = (argc - optind == 1) ? argv[optind] : curr_dir;

    DIR *dp = opendir(dir);
    if (dp == NULL)
    {
        perror("opendir()");
        exit(EXIT_FAILURE);
    }

    errno = 0;

    struct dirent *d;
    while ((d = readdir(dp)) != NULL)
    {
        printf("%s\n", d->d_name);
        if (need_stat)
        {
            struct stat sbuf;
            char full_path[PATH_MAX];
            snprintf(full_path, sizeof(full_path), "%s/%s", dir, d->d_name);

            int rc = stat(full_path, &sbuf);
            if (rc < 0)
            {
                perror("stat()");
                break;
            }

            print_stat(sbuf);
        }
    }

    if (errno != 0)
        perror("readdir()");

    if (closedir(dp) < 0)
        perror("closedir()");

    free(curr_dir);

    return 0;
}