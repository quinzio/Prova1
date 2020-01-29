import re
import os

f =  open("ast.txt", encoding='utf-8', errors='ignore')
lines = f.readlines()
asttypes = set()
reline = re.compile(r'\W+(\w+)')
for line in lines:
    linegroups = reline.match(line)
    if (linegroups):
        astNodeType = linegroups.groups()[0]
        print(astNodeType)
        asttypes.add(astNodeType)

for i in range(0,10):
    print('******************************************')
ix = 1
for item in sorted(asttypes):
    print(str(ix) + ' ' + item)
    ix += 1
         