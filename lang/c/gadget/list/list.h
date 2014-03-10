/*
 * Note : this list based on index, not pointer.
 */

#ifndef _LIST_H_
#define _LIST_H_

typedef unsigned int  _u32;
typedef unsigned char _u8;

#define INVALID_LIST_HEAD 0xffffffff

/*
 * Main pool management APIs.
 */
#define calc_next_offset(_typeof_item, _next) (size_t)(&(((_typeof_item *)0)->_next))

struct list_info
{
	void  *item_base;    /* base address of array */
	size_t item_total;
	size_t item_size;    /* sizeof on item within array */
	size_t next_offset;  /* offset of next feild in item */

	_u32 free_head;
	_u32 free_tail;
//	_u32 used_head;

	/* TODO: update the type of lock */
	_u32 lock;
};

void list_info_init(struct list_info *listinfo);
void list_free_item(struct list_info *listinfo, _u32 idx);
_u32 list_alloc_item(struct list_info *listinfo);

/*
 * Index table management apis.
 */
struct index_item
{
	_u32 idx;

	/* TODO: update the type of lock */
	_u32 lock;
};

struct index_table
{
	struct index_item *base;
	_u32               total;
};

/*
 * Callback for checking whether pos(index of) item same(match) to *data.
 * Return 1 when same, else return 0.
 */
typedef int (*func_is_same_item)(struct list_info *listinfo, _u32 pos, void *data);

void index_table_init(struct index_table *itb);

void index_list_do_link(_u32 *dest_addr, _u32 new);

_u32 *index_list_prelink(struct list_info *listinfo,
	struct index_item *tb_entry,
	func_is_same_item is_same_item, void *cmpdata);

_u32 index_list_unlink(struct list_info *listinfo,
		struct index_item *tb_entry,
		func_is_same_item is_same_item, void *cmpdata);

#endif /* _LIST_H_ */
