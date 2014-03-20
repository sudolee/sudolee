
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

/*
 * Main pool management apis.
 */
inline void item_set_next2(struct list_info *listinfo, _u32 idx, _u32 next, _u32 next_offset)
{
	_u8 *base = (_u8 *)listinfo->item_base;

	*((_u32 *)(base + (idx * listinfo->item_size) + next_offset)) = next;
}

inline void item_set_next(struct list_info *listinfo, _u32 idx, _u32 next)
{
	item_set_next2(listinfo, idx, next, listinfo->next_offset);
}

inline _u32 item_get_next2(struct list_info *listinfo, _u32 idx, _u32 next_offset)
{
	_u8 *base = (_u8 *)listinfo->item_base;

	return *((_u32 *)(base + (idx * listinfo->item_size) + next_offset));
}

inline _u32 item_get_next(struct list_info *listinfo, _u32 idx)
{
	return item_get_next2(listinfo, idx, listinfo->next_offset);
}

inline _u32 *item_get_next_addr2(struct list_info *listinfo, _u32 idx, _u32 next_offset)
{
	_u8 *base = (_u8 *)listinfo->item_base;

	return (_u32 *)(base + (idx * listinfo->item_size) + next_offset);
}

inline _u32 *item_get_next_addr(struct list_info *listinfo, _u32 idx)
{
	return item_get_next_addr2(listinfo, idx, listinfo->next_offset);
}

void list_info_init(struct list_info *listinfo)
{
	int i;

	listinfo->free_head = 0;
	listinfo->free_tail = listinfo->item_total - 1;
//	listinfo->used_head = INVALID_LIST_HEAD;

	for(i = 0; i < listinfo->item_total; i++) {
		item_set_next(listinfo, i, i + 1);
	}
	item_set_next(listinfo, i - 1, INVALID_LIST_HEAD);
}

void list_free_item(struct list_info *listinfo, _u32 idx)
{
	/* TODO: get_lock(listinfo->lock) */

	if(listinfo->free_head != INVALID_LIST_HEAD) {
		item_set_next(listinfo, listinfo->free_tail, idx);
		listinfo->free_tail = idx;
	} else {
		listinfo->free_head = idx;
		listinfo->free_tail = idx;
	}
	item_set_next(listinfo, idx, INVALID_LIST_HEAD);

	/* TODO: release_lock(listinfo->lock) */
}

/* Note: after alloc, the item is not in any list. */
_u32 list_alloc_item(struct list_info *listinfo)
{
	/* TODO: get_lock(listinfo->lock) */

	if(listinfo->free_head == INVALID_LIST_HEAD) {
		/* TODO: release_lock(listinfo->lock) */
		return INVALID_LIST_HEAD;
	}

	_u32 new = listinfo->free_head;
	listinfo->free_head = item_get_next(listinfo, new);

	if(listinfo->free_head == INVALID_LIST_HEAD) {
		listinfo->free_tail = INVALID_LIST_HEAD;
	}
	/* TODO: release_lock(listinfo->lock) */

	item_set_next(listinfo, new, INVALID_LIST_HEAD);

	return new;
}

/*
 * Index table management apis.
 */
void index_table_init(struct index_item *itb, _u32 total)
{
	int i;

	for(i = 0; i < total; i++)
	{
		/* TODO: get_lock(itb->base[i].lock) */

		itb[i].idx = INVALID_LIST_HEAD;

		/* TODO: release_lock(itb->base[i].lock) */
	}
}

/*
 * Do really link new item into index(match) list.
 * @ dest_addr: belong to new item's parent, the address of its next feild.
 */
void index_list_do_link(_u32 *dest_addr, _u32 new)
{
	*((_u32 *)dest_addr) = new;
}

/*
 * Search whether is there any item match cmpdata,
 * if anyone matched, return NULL.
 * else, return the dest address index of new item will be write into.
 *
 * @ cmpdata: used by caller to check whether there is already a same item,
 *         and will be passed as is_same_item()'s 3rd parameter.
 *
 * p.s. this function could also be used for finding exist item in index(match) list.
 */
_u32 *index_list_prelink2(struct list_info *listinfo,
	struct index_item *itb, _u32 itb_idx,
	void *data, func_is_same_item is_same_item, int next_offset)
{
//	_u32 head = itb->table[itb_idx].idx;
	_u32 head = itb[itb_idx].idx;

	/* get lock */
	if(head != INVALID_LIST_HEAD)
	{
		if(is_same_item(listinfo, head, data))
		{
			return NULL;
		}
		else
		{
			while(item_get_next2(listinfo, head, next_offset) != INVALID_LIST_HEAD)
			{
				head = item_get_next2(listinfo, head, next_offset);
				if(is_same_item(listinfo, head, data))
				{
					return NULL;
				}
			}
			return item_get_next_addr2(listinfo, head, next_offset);
		}
	}
	else
	{
		return &itb[itb_idx].idx;
	}
}

_u32 *index_list_prelink(struct list_info *listinfo,
	struct index_item *itb, _u32 itb_idx,
	void *data, func_is_same_item is_same_item)
{
	return index_list_prelink2(listinfo, itb, itb_idx, data,
			is_same_item, listinfo->next_offset);
}

/*
 * unlink item from the list of index(match) table,
 * if exist, return its index,
 * else, return invalid.
 * Note: after unlink the item is not in any list.
 */
_u32 index_list_unlink2(struct list_info *listinfo,
		struct index_item *itb, _u32 itb_idx, void *data,
		func_is_same_item is_same_item, _u32 next_offset)
{
	_u32 head, current;

	head = itb[itb_idx].idx;

	if(head != INVALID_LIST_HEAD)
	{
		if(is_same_item(listinfo, head, data))
		{
			itb[itb_idx].idx = item_get_next2(listinfo, head, next_offset);

			item_set_next2(listinfo, head, INVALID_LIST_HEAD, next_offset);
			return head;
		}
		else
		{
			while(item_get_next2(listinfo, head, next_offset) != INVALID_LIST_HEAD)
			{
				current = item_get_next2(listinfo, head, next_offset);
				if(is_same_item(listinfo, current, data))
				{
					item_set_next2(listinfo, head, item_get_next2(listinfo, current, next_offset), next_offset);

					item_set_next2(listinfo, current, INVALID_LIST_HEAD, next_offset);
					return current;
				}
				head = current;
			}
		}
	}

	return INVALID_LIST_HEAD;
}
_u32 index_list_unlink(struct list_info *listinfo,
		struct index_item *itb, _u32 itb_idx, void *data,
		func_is_same_item is_same_item)
{
	return index_list_unlink2(listinfo, itb, itb_idx, data,
		is_same_item, listinfo->next_offset);
}

/*
 * e.g.
 */
void listinfo_dump(struct list_info *list)
{
	_u32 i;
	printf("--> free head:%2u, free tail:%u\n", list->free_head, list->free_tail);
	for(i = 0; i< list->item_total; i++) {
		printf("item:%2u, next:%u\n", i, item_get_next(list, i));
	}
}

#define MAX_CELLS 32

struct my_item_type {
	int  value;
	/* much more other data. */

	_u32 pnext;
};

struct instance {
	struct list_info    list;
	struct index_table  table;

	struct my_item_type mydata[MAX_CELLS];
	struct index_item   match[MAX_CELLS];
};

int main()
{
	struct instance *inst;

	inst = (struct instance *)malloc(sizeof(struct instance));
	if(!inst) {
		printf("Alloc memory for instance failed, :(\n");
		exit(EXIT_FAILURE);
	}

	/* main pool list init */
	inst->list.item_base   = inst->mydata;
	inst->list.item_size   = sizeof(struct my_item_type);
	inst->list.item_total  = sizeof(inst->mydata) / sizeof(struct my_item_type);
	inst->list.next_offset = calc_next_offset(struct my_item_type, pnext);

	list_info_init(&inst->list);

	/* index(match) table init */
	inst->table.total = sizeof(inst->match) / sizeof(struct index_item);
	inst->table.base  = inst->match;

	index_table_init(inst->match, sizeof(inst->match) / sizeof(struct index_item));

	/* then use the common APIs */
	_u32 i, new;
	listinfo_dump(&inst->list);
	printf("item alloc...\n");
	for(i = 0; i < 33; i++) {
		new = list_alloc_item(&inst->list);
	}
	listinfo_dump(&inst->list);

	printf("then, free items...\n");
	for(i = 0; i < 32; i++) {
		list_free_item(&inst->list, i);
	}
	listinfo_dump(&inst->list);

	exit(EXIT_SUCCESS);
}
