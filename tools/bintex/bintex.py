import sys
import os

size = os.path.getsize(sys.argv[1])
bytes_read = open(sys.argv[1], "rb").read()

print(size)
line_out = ""
for b in bytes_read:
    line_out += "{:02x}".format(b)
    if len(line_out) == 64:
        print(line_out)
        line_out = ""
print(line_out)
