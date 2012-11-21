#include "list.h"

void init_list_head(struct list_head *list)
{
	list->prev = list;
	list->next = list;
}

/* add tail */
void list_add(struct list_head *new, struct list_head *head)
{
	head->prev->next = new;
	new->prev = head->prev;
	new->next = head;
	head->prev = new;
}

void list_del(struct list_head *entry)
{
	entry->prev->next = entry->next;
	entry->next->prev = entry->prev;
}

u32 list_is_last(const struct list_head *list, const struct list_head *head)
{
	return (head->prev == list);
}

u32 list_empty(struct list_head *entry)
{
	return (entry->prev == entry);
}
