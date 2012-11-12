#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>

#define USAGE() do{\
	printf("Usage: del_M [OPTION]...\n");\
	printf("Remove all the \'^M\' in \'*.c\' and \'*.h\'.\n\n");\
	printf("\t-h, display this help infomation and exit\n");\
	printf("\t-p, the path to interate over\n");\
	printf("\t\te.g: del_M -p .\n");\
	printf("\t-i, use indent after del M, if not specify -i never indent\n");\
	printf("\t\te.g: del_M -p . -i [-npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1] (default)\n");\
	printf("\t-r, directories and their contents recursively\n");\
	printf("\nAuthor: desoxydate@gmail.com\n");\
}while(0)

#define HELP() do{\
	printf("Try \'del_M -h\' for more information.\n");\
}while(0)

struct arg_tab {
	char *path;
	char i_flag;
	char *i_arg;		/* indent arguments */
	char r_flag;
};

static int get_option(int argc, char **argv, struct arg_tab *argtab)
{
	int opt;
	argtab->r_flag = 0;
	argtab->i_flag = 0;

	while ((opt = getopt(argc, argv, "hp:ir")) != -1) {
		switch (opt) {
		case 'h':
			USAGE();
			exit(EXIT_SUCCESS);
		case 'p':
			argtab->path = optarg;
			break;
		case 'i':
			argtab->i_flag = 1;
			break;
		case 'r':
			argtab->r_flag = 1;
			break;
		default:
			HELP();
			return -EINVAL;
		}
	}
	return 0;
}

static int check_path(struct arg_tab *argtab)
{
	struct stat file_stat;

	if (argtab->path) {
		if (!stat(argtab->path, &file_stat)) {
			if (S_ISDIR(file_stat.st_mode))
				return 0;
		}
	}
	return -ENOTDIR;
}

static int del_M(const char *path)
{
	char ch, flag = 0;
	int res;
	FILE *fb;
	long pos;

	fb = fopen(path, "r+");
	if (!fb) {
		fprintf(stderr, "open() %s ERROR\n", path);
		return 0;
	}

	do {
		pos = ftell(fb);
		ch = fgetc(fb);
		if (ch == 13) {
			if (!flag)
				flag = 1;
			fseek(fb, pos, SEEK_SET);
			fputc(' ', fb);
		}
	} while (ch != EOF);

	if (flag)
		printf("Delete \'^M\' in file(%s)\n", path);
	fclose(fb);
	return flag;
}

static int iterate_no_r(DIR * dir)
{
	int res, flag = 0;
	struct dirent *entry;

	while ((entry = readdir(dir)) != NULL) {
		if ((entry->d_type != DT_REG) && (entry->d_type != DT_LNK))
			continue;
		short len = strlen(entry->d_name);
		if ((entry->d_name[len - 2] == '.')
		    && ((entry->d_name[len - 1] == 'c')
			|| (entry->d_name[len - 1] == 'h'))) {
			res = del_M(entry->d_name);
			if(res > 0)
				flag = res;
		}
	}
	return flag;
}

static int iterate_r(DIR * dir)
{
	int res = 0;
	struct dirent *entry;
	return 0;
}

static int iterate(const char *path, int r_flag)
{
	int res = 0;
	DIR *dir;

	dir = opendir(path);
	if (!dir) {
		fprintf(stderr, "open dir ERROR\n");
		exit(EXIT_FAILURE);
	}

	if (r_flag)
		res = iterate_r(dir);
	else
		res = iterate_no_r(dir);

	return res;
}

static void indent(char *path)
{
#define COMMAND_1 "find %s -name \'%s\' | xargs indent %s"
#define COMMAND_2 "find %s -name \'%s\' | xargs rm"
#define INDENT_PARM "-npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1"
	char *command;
	int len;

	len = strlen(COMMAND_1) + 256;
	command = malloc(len);

	sprintf(command, COMMAND_1, path, "*.c", INDENT_PARM);
	system(command);
	sprintf(command, COMMAND_2, path, "*.c~");
//	system(command);
	sprintf(command, COMMAND_1, path, "*.h", INDENT_PARM);
	system(command);
	sprintf(command, COMMAND_2, path, "*.h~");
	system(command);
}

int main(int argc, char **argv)
{
	int res;
	char *path, default_path[] = "./";
	struct arg_tab argtab = { NULL, 0, NULL, 0 };

	res = get_option(argc, argv, &argtab);
	if (res) {
		fprintf(stderr, "get_option() ERROR.\n");
		exit(EXIT_FAILURE);
	}

	if (check_path(&argtab))
		path = default_path;
	else
		path = argtab.path;
	printf("Enter path(%s).\n", path);

	res = iterate(path, argtab.r_flag);
	if (!res)
		printf("No file(s) need to del \'^M\'\n");

	if (res > 0 && argtab.i_flag)
		indent(path);

	exit(EXIT_SUCCESS);;
}
