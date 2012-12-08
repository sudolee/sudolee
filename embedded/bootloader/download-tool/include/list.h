#ifndef _LIST_H_
#define _LIST_H_

#include "type.h"

struct list_head {
	struct list_head *next, *prev;
};

#define list_for_each(pos, head) \
	for (pos= head->next; pos != (head); pos = pos->next)

#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
* list_for_each_entry  -   iterate over list of given type
* @pos:    the type * to use as a loop cursor.
* @head:   the head for your list.
* @member: the name of the list_struct within the struct.
*/
#define list_for_each_entry(pos, head, member) \
	for (pos = list_entry((head)->next, typeof(*pos), member); \
			&pos->member != (head); \
			pos = list_entry(pos->member.next, typeof(*pos), member))

void init_list_head(struct list_head *list);
void list_add(struct list_head *new, struct list_head *head);
void list_del(struct list_head *entry);
u32 list_is_last(const struct list_head *, const struct list_head *);
u32 list_empty(struct list_head *entry);

#endif /* _LIST_H_ */
