/*
 * Note : this list based on index, not pointer.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INVALID_LIST_HEAD 0xffffffff

struct list_head {
	unsigned long next;
};

static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = INVALID_LIST_HEAD;
}

/*
 * Note: need check is the list head empty.
 */
static inline unsigned long __list_add_tail(struct list_head *new, unsigned long new_idx,
		struct list_head *tail)
{
	tail->next = new_idx;
	new->next  = INVALID_LIST_HEAD;

	return new_idx;
}

static inline unsigned long __list_add_head(struct list_head *new, unsigned long new_idx,
		unsigned long *head_idx)
{
	new->next = *head_idx;
	*head_idx = new_idx;

	return *head_idx;
}

/*
 * Note: need check is del the list head.
 */
static inline unsigned long __list_del(struct list_head *del, struct list_head *prev,
		struct list_head *next)
{
	prev->next = INVALID_LIST_HEAD;
	del->next  = INVALID_LIST_HEAD;
}

/*
 * Note: need check old is the list head.
 */
static inline unsigned long list_replace(struct list_head *prev, struct list_head *next,
		struct list_head *old, struct list_head *new, unsigned long new_idx)
{
	unsigned long old_idx = prev->next;

	if(prev->next == INVALID_LIST_HEAD) {
	} else {
		prev->next = new_idx;
		next->prev = new_idx;
		new->prev  = old->prev;
		new->next  = old->next;
	}

	old->prev = INVALID_LIST_HEAD;
	old->next = INVALID_LIST_HEAD;

	return old_idx;
}

#define next_offset(_typeof_st, _next) (((_typeof_st *)0)->(_next))

struct list_info {
	void  *item_base;
	size_t item_total;
	size_t item_size;
	size_t next_offset;
	unsigned long free_head, free_tail, used_head;
};

struct item {
	int val;

	unsigned long next;
};

struct table {
	struct list_info listinfo;

	struct item items[1024];
};
static void listinfo_init(struct listinfo *listinfo)
{
	unsigned long i;
	unsigned char *base = (unsigned char *)listinfo->item_base;

	listinfo->free_head = 0;
	listinfo->free_tail = listinfo->memb_total - 1;
	listinfo->used_head = INVALID_LIST_HEAD;

	for(i = 0; i < listinfo->memb_total; i++) {
		*((unsigned long *)(base[i * listinfo->item_size] + listinfo->next_offset)) = i;
	}
	*((unsigned long *)(base[i * listinfo->item_size] + listinfo->next_offset)) = INVALID_LIST_HEAD;
}
static unsigned long item_get_next(struct listinfo *listinfo, unsigned long new)
{
}
static unsigned long list_get_item(struct listinfo *listinfo)
{
	if(listinfo->free_head == INVALID_LIST_HEAD) {
		return INVALID_LIST_HEAD;
	} else {
		unsigned long new;
		/* TODO: need lock */
		new = listinfo->free_head;
	}
}


int main(int argc, char **argv)
{
	struct table *tb;

	tb = (struct table *)malloc(sizeof(struct table));
	if(tb == NULL) {
		printf("memory alloc failed, :(\n");
		exit(EXIT_FAILURE);
	}
	tb->listinfo.item_base = tb->items;
	tb->listinfo.item_size = sizeof(struct item);
	tb->listinfo.item_total = sizeof(tb->items) / sizeof(struct item);
	tb->listinfo.next_offset = next_offset(struct value, next);


	exit(EXIT_SUCCESS);
}
