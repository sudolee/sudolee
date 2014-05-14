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

inline long dirent_buf_size(DIR *dirp)
{
	long name_max, len;

	name_max = fpathconf(dirfd(dirp), _PC_NAME_MAX);
	if(name_max == -1)  /* Limit not defined, or error */
	{
		name_max = 255; /* Take a guess */
	}
	len = offsetof(struct dirent, d_name) + name_max + 1;

	return len;
}

inline int is_hide_file(char *pathname)
{
	if(pathname[0] == '.' && (pathname[1] == '\0' ||
				(pathname[1] == '.' && pathname[2] == '\0')))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/* reentrant version */
int dirlist_r(char *pathname)
{
	DIR *dirp;
	struct dirent *dt, *entry;
	long max_pathname, namelen, orig_offset;
	char *currentpath;

	dirp = opendir(pathname);
	if(dirp == NULL)
	{
		perror(":: opendir ");
		return RET_ERROR;
	}

	max_pathname = dirent_buf_size(dirp);

	entry = malloc(max_pathname);
	if(entry == NULL)
	{
		perror(":: malloc ");
		closedir(dirp);
		return RET_ERROR;
	}

	namelen = strlen(pathname) + max_pathname + 1;

	currentpath = malloc(namelen);
	if(currentpath == NULL)
	{
		perror(":: malloc ");

		free(entry);
		closedir(dirp);
		return RET_ERROR;
	}
	memset(currentpath, 0, namelen);

	orig_offset = sprintf(currentpath, "%s/", pathname);

	while((readdir_r(dirp, entry, &dt) == 0) && (dt != NULL))
	{
		/* hide . & .. */
		if(is_hide_file(dt->d_name))
		{
			continue;
		}

		sprintf(currentpath + orig_offset, "%s", dt->d_name);

		printf("%s\n", currentpath);

#ifdef DIRLIST_RECURSIVE
		struct stat stat_buf;

		if(lstat(currentpath, &stat_buf) == -1)
		{
			perror(":: lstat ");
		}
		else
		{
			if(S_ISDIR(stat_buf.st_mode))
			{
				dirlist_r(currentpath);
			}
		}
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
	if(dirp == NULL)
	{
		perror(":: opendir ");
		return RET_ERROR;
	}

	max_pathname = dirent_buf_size(dirp);

	namelen = strlen(pathname) + max_pathname + 1;

	currentpath = malloc(namelen);
	if(currentpath == NULL)
	{
		perror(":: malloc ");

		closedir(dirp);
		return RET_ERROR;
	}
	memset(currentpath, 0, namelen);

	orig_offset = sprintf(currentpath, "%s/", pathname);

	for(;;)
	{
		olderrno = errno;

		dt = readdir(dirp);
		if(dt == NULL)
		{
			/* error detected when errno changed with readdir return NULL. */
			if(errno != olderrno)
			{
				perror(":: readdir ");
			}
			/* else, end of stream reached */
			break;
		}

		/* hide . & .. */
		if(is_hide_file(dt->d_name))
		{
			continue;
		}

		sprintf(currentpath + orig_offset, "%s", dt->d_name);

		printf("%s\n", currentpath);

#ifdef DIRLIST_RECURSIVE
		struct stat stat_buf;

		if(lstat(currentpath, &stat_buf) == -1)
		{
			perror(":: lstat ");
		}
		else
		{
			if(S_ISDIR(stat_buf.st_mode))
			{
				dirlist(currentpath);
			}
		}
#endif
	}

	free(currentpath);
	closedir(dirp);

	return RET_OKAY;
}

int main(int argc, char **argv)
{
	if(argc > 1 && argv[1])
	{
		dirlist_r(argv[1]);
	}
	else
	{
		dirlist_r(".");
	}

	return 0;
}
