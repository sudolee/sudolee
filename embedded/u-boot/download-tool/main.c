#include "cpm.h"
#include "mem.h"


typedef void (init_func_t)(void);
#if 0
static void (*init_tbl[])(void) = {
#else
init_func_t *init_tbl[] = {
#endif
	cpm_init,
	mem_init,
	NULL,
};

void c_start(void)
{
	init_func_t *init_funcp;

	for(init_funcp = init_tbl; *init_funcp; ++init_funcp)
		(*init_funcp)();

	/* hang there */
	for(;;);
}
