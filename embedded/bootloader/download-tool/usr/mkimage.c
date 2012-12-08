#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define MAGIC_SIZE 8
#define MAGIC_STR "LEE.dt\n"

/*
** +-----------------+ 
** | img header      | 1 page
** +-----------------+
** | image           | n pages  
** +-----------------+
*/

struct header {
	char magic[MAGIC_SIZE];
	int headsize;

	int img_offset;
	int img_size;
};

static struct header *create_head(void)
{
	struct header *head;

	head = malloc(sizeof(struct header));
	if(!head)
		return NULL;

	strcpy(head->magic, MAGIC_STR);
	head->headsize = sizeof(struct header);
	head->img_offset = sizeof(struct header);

	return head;
}

static void make_image(const char *in, const char *out)
{
	struct header *head;
	int fin, fout;
	int ret;
	char readbuf[512];


	head = create_head();
	if(!head){
		printf("%s: create head error.\n", __func__);
		exit(EXIT_FAILURE);
	}

	fin = open(in, O_RDONLY);
	if(fin == -1) {
		printf("%s: can't open %s.\n", __func__, in);
		exit(EXIT_FAILURE);
	}

	/* fill image size area within header. */
	head->img_size = lseek(fin, 0, SEEK_END);
	printf("head->img_size = %d\n", head->img_size);
	lseek(fin, 0, SEEK_SET);

	fout = open(out, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if(fout == -1) {
		printf("%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	ret = write(fout, (void*)head, head->headsize);
	if(ret == -1) {
		printf("%s: write %s error.\n", __func__, out);
		exit(EXIT_FAILURE);
	}

	ret = 0;
	while(1) {
		ret = read(fin, readbuf, sizeof(readbuf));
		if(ret == -1) {
			printf("%s: read %s error.\n", __func__, in);
			exit(EXIT_FAILURE);
		} else if(ret == 0) {
			break;
		} else {
			ret = write(fout, readbuf, ret);
			if(ret == -1) {
				printf("%s: read %s error.\n", __func__, in);
				exit(EXIT_FAILURE);
			}
		}
	}

	close(fin);
	close(fout);
	printf("%s done.\n", __func__);
}

static void usage(void)
{
	puts("Usage: mkimage -i input file -o output file");
}

int main(int argc, char **argv)
{
	int opt;
	char *inf, *outf;
	unsigned int flags = 0;

	while ((opt = getopt(argc, argv, "o:i:")) != -1) {
		switch (opt) {
			case 'o':
				outf = optarg;
				flags |= 1 << 0;
				break;

			case 'i':
				inf = optarg;
				flags |= 1 << 1;
				break;

			default:
				usage();
				exit(EXIT_FAILURE);
		}
	}

	if(!flags) {
		usage();
		exit(EXIT_FAILURE);
	}

	make_image(inf, outf);

	return 0;
}
