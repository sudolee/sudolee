#!/usr/bin/env python

import random
random.seed()

groups = 3

values = [random.randint(0, 1000) for i in range(100)]
values.sort(reverse=True)

# Because sum([]) == sum([0,]), value[0] maybe not zero,
#    so make index start from 1.
load_balance_groups  = [[] for grp in range(1, groups + 1)]

for v in values:
    load_balance_groups.sort(key=lambda x: sum(x))
    load_balance_groups[0].append(v)

for per in load_balance_groups:
    print(sum(per))

