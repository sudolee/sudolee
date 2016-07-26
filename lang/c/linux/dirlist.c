#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <dirent.h>
#include <sys/stat.h>

#define RET_ERROR (-1)
#define RET_OKAY  (0)

#define DIRLIST_RECURSIVE

#define ISSLASH(C) ((C) == '/')

long dirent_buf_size(DIR *dirp)
{
	long name_max, len;

	name_max = fpathconf(dirfd(dirp), _PC_NAME_MAX);
	if (name_max == -1)  /* Limit not defined, or error */
		name_max = 255; /* Take a guess */
	len = offsetof(struct dirent, d_name) + name_max + 1;

	return len;
}

int is_dot_or_dotdot(char *pathname)
{
	if (pathname[0] == '.') {
		char sep = pathname[(pathname[1] == '.') + 1];
		return !sep;
	} else
		return 0;
}

int is_directory(char *pathname)
{
	struct stat stat_buf;

	if (lstat(pathname, &stat_buf) == -1)
		perror(":: lstat ");
	else {
		if (S_ISDIR(stat_buf.st_mode))
			return 1;
	}

	return 0;
}

/* scandir (optional, sort) files under directory "pathname" */
int _scandir(char *pathname)
{
	DIR *dirp;
	struct dirent **namelist;
	char *currentpath;
	long max_pathname, namelen, orig_offset;
	int count;

	dirp = opendir(pathname);
	if (dirp == NULL) {
		perror(":: opendir ");
		return RET_ERROR;
	}

	max_pathname = dirent_buf_size(dirp);

	namelen = strlen(pathname) + max_pathname + 1;

	currentpath = malloc(namelen);
	if (currentpath == NULL) {
		perror(":: malloc ");

		closedir(dirp);
		return RET_ERROR;
	}
	memset(currentpath, 0, namelen);

	orig_offset = sprintf(currentpath, "%s/", pathname);

	//	count = scandir(currentpath, &namelist, NULL, alphasort);
	count = scandir(currentpath, &namelist, NULL, NULL);
	if (count == -1)
		perror(":: scandir ");
	else {
		while (count--) {
			if (is_dot_or_dotdot(namelist[count]->d_name)) {
				continue;
				free(namelist[count]);
			}

			sprintf(currentpath + orig_offset, "%s", namelist[count]->d_name);
			printf("%s\n", currentpath);

			free(namelist[count]);

#ifdef DIRLIST_RECURSIVE
			if (is_directory(currentpath))
				_scandir(currentpath);
#endif
		}
		free(namelist);
	}

	closedir(dirp);
	free(currentpath);

	return RET_OKAY;
}

/* reentrant version */
int dirlist_r(char *pathname)
{
	DIR *dirp;
	struct dirent *dt, *entry;
	long max_pathname, namelen, orig_offset;
	char *currentpath;

	dirp = opendir(pathname);
	if (dirp == NULL) {
		perror(":: opendir ");
		return RET_ERROR;
	}

	max_pathname = dirent_buf_size(dirp);

	entry = malloc(max_pathname);
	if (entry == NULL) {
		perror(":: malloc ");
		closedir(dirp);
		return RET_ERROR;
	}

	namelen = strlen(pathname) + max_pathname + 1;

	currentpath = malloc(namelen);
	if (currentpath == NULL) {
		perror(":: malloc ");

		free(entry);
		closedir(dirp);
		return RET_ERROR;
	}
	memset(currentpath, 0, namelen);

	orig_offset = sprintf(currentpath, "%s/", pathname);

	while ((readdir_r(dirp, entry, &dt) == 0) && (dt != NULL)) {
		/* hide . & .. */
		if (is_dot_or_dotdot(dt->d_name))
			continue;

		sprintf(currentpath + orig_offset, "%s", dt->d_name);

		printf("%s\n", currentpath);

#ifdef DIRLIST_RECURSIVE
		if (is_directory(currentpath))
			dirlist_r(currentpath);
#endif
	}

	free(entry);
	free(currentpath);
	closedir(dirp);

	return RET_OKAY;
}

int dirlist(char *pathname)
{
	DIR *dirp;
	struct dirent *dt;
	char *currentpath;
	long max_pathname, namelen, orig_offset;
	int olderrno;

	dirp = opendir(pathname);
	if (dirp == NULL) {
		perror(":: opendir ");
		return RET_ERROR;
	}

	max_pathname = dirent_buf_size(dirp);

	namelen = strlen(pathname) + max_pathname + 1;

	currentpath = malloc(namelen);
	if (currentpath == NULL) {
		perror(":: malloc ");

		closedir(dirp);
		return RET_ERROR;
	}
	memset(currentpath, 0, namelen);

	orig_offset = sprintf(currentpath, "%s/", pathname);

	for (;;) {
		olderrno = errno;

		dt = readdir(dirp);
		if (dt == NULL) {
			/* error detected when errno changed with readdir return NULL. */
			if (errno != olderrno)
				perror(":: readdir ");
			/* else, end of stream reached */
			break;
		}

		/* hide . & .. */
		if (is_dot_or_dotdot(dt->d_name))
			continue;

		sprintf(currentpath + orig_offset, "%s", dt->d_name);

		printf("%s\n", currentpath);

#ifdef DIRLIST_RECURSIVE
		if (is_directory(currentpath))
			dirlist(currentpath);
#endif
	}

	free(currentpath);
	closedir(dirp);

	return RET_OKAY;
}

int main(int argc, char **argv)
{
	if (argc > 1 && argv[1]) {
		dirlist_r(argv[1]);
//		dirlist(argv[1]);
//		_scandir(argv[1]);
	} else {
		dirlist_r(".");
//		dirlist(".");
//		_scandir(".");
	}

	return 0;
}
