#!/bin/python

'''
Script to convert hex EEPROM dump to ASCII text
'''

import sys

# defines defaults...
N = 16
addr = 0

def x2ord(x):
  return int(x,16)

def ord2xx(x):
  return "%02X" % x

def printable(x):
  if x<32 or x>127:
    return '.'
  return chr(x)

# get arguments, remove script name
args = sys.argv[1:]

# usage check...
if len(args)==0 or '-h' in args:
  print "syntax: python hex2Txt.py hex_filename"
  exit(0)
else:
  file = args.pop(0)

if file:
  print "Converting HEX file: ", file, "..."  # a little debug info...
  commandlines = open(file,"r").readlines()
  tokens = [];
  for line in commandlines:
    tokens += map(x2ord,line.strip().split(','))
  print "Found %i tokens" % len(tokens)
  while len(tokens):
    htxt = ' '.join(map(ord2xx,tokens[0:N]))
    text = ''.join(map(printable,tokens[0:N]))
    print "%04X: %s: %s" % (addr, htxt, text)
    tokens = tokens[N:]
    addr += N
