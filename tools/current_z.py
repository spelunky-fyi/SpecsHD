# In xdbg the following log with log the active state, entity type, and current z.
# {mem;1@esp+29c}: {mem;4@esi+0xc}: {mem;4@esi+40} 
# This script parses the data output from that log.

import re
import struct

matcher = re.compile(r"(\d\d): ([a-zA-Z0-9]+): ([a-zA-Z0-9]+)")
f = open("entities.txt")
entities = {}
for line in f:
    try:
        match = matcher.match(line.strip())
        if not match:
            continue
        (active, type_, current_z) = match.groups()
        if active == "00":
            continue
        type_ = struct.unpack("i", int(type_, 16).to_bytes(4, 'big'))[0]
        current_z = struct.unpack("f", int(current_z, 16).to_bytes(4, 'big'))[0]
        if type_ not in entities:
            entities[type_] = set()
        entities[type_].add(current_z)
    except Exception as err:
        print("failed on {:r}: {}".format(line, err))


for (type_, current_z) in entities.items():
    print(f"{type_}, {','.join(map(str, current_z))}")