#include "common.h"
#include "io.h"
#include "string.h"
#include "nand.h"

#define DEFAULT_LOAD_ADDRESS (0x30000000 + 0x100000)  /* skip 1MB space */
#define MAGIC_SIZE 8
#define MAGIC_STR "LEE.dt\0"

struct img_header {
	char magic[MAGIC_SIZE];
	int headsize;

	int img_offset;
	int img_size;
	/* maybe some pad space */
};

static inline void wait_image(struct img_header *head)
{
	printf("Waiting image at 0x%x\n", DEFAULT_LOAD_ADDRESS);

	for(;;) {
		if(!strcmp(head->magic, MAGIC_STR)) {
			printf("Image found\n");
			break;
		}
		mb();
	}
}

void dt_main(void)
{
	struct img_header *header = (struct img_header *)DEFAULT_LOAD_ADDRESS;
	struct nand_info *nf = get_nandinfo();
	size_t retlen = 0;

	wait_image(header);

	/* Cope to 0 block 0 page */
	printf("Download beginning\n");

	nf->write(nf, 0, header->img_size, &retlen, (char *)(DEFAULT_LOAD_ADDRESS + header->img_offset));
	if(retlen < header->img_size)
		printf("Warning: copy less data\n");

	printf("Download done\n");
	/* Never return */
}
