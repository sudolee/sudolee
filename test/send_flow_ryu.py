#!/usr/bin/python

import os
import json
from random import getrandbits, randint
from ipaddress import IPv4Address, IPv6Address

def random_mac():
    return "02:00:00:%02x:%02x:%02x" % (randint(0, 255), randint(0, 255), randint(0, 255))

def random_ip(v):
    if v == 4:
        bits = getrandbits(32) # generates an integer with 32 random bits
        addr = IPv4Address(bits) # instances an IPv4Address object from those bits
        addr_str = str(addr) # get the IPv4Address object's string representation
    elif v == 6:
        bits = getrandbits(128) # generates an integer with 128 random bits
        addr = IPv6Address(bits) # instances an IPv6Address object from those bits
        # .compressed contains the short version of the IPv6 address
        # str(addr) always returns the short address
        # .exploded is the opposite of this, always returning the full address with all-zero groups and so on
        addr_str = addr.compressed

    return addr_str

#flow = 'curl http://192.168.0.79:8080/stats/flowentry/add -X POST -H "Content-Type:application/json" -d '
flow = 'curl http://192.168.0.79:8080/stats/flowentry/add -X POST -d '

for table in range(0, 2):
    for i in range(1, 65535):
        dmac = random_mac()
        smac = random_mac()
        dip = random_ip(4)
        sip = random_ip(4)
        dport = randint(1, 65535)

        data = json.dumps({
            "dpid":479759545090258,
            "cookie": i + (table * 65536),
            "table_id": table,
            "match": {
                "in_port": 1,
                "dl_dst": dmac,
                "dl_src": smac,
                "dl_type": 0x800,
                "nw_proto": 6,
                "nw_dst": dip,
                "nw_src": sip,
                "tp_dst": dport
                },
            "actions":[
                {
                    "type":"OUTPUT",
                    "port": "2"
                    }
                ]
            })

        cmd = flow + '%r' % data
        print(cmd)
        os.system(cmd)

'''
v = random_ip(4)
print(v)
v = random_mac()
print(v)
'''
