import os
import re


f = open("../temp.c.003t.original", mode='r')

lines = f.readlines()

print(lines[10])

f.close()