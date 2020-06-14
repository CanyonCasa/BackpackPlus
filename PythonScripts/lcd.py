#!/usr/bin/env python3

'''
Python command script for Adafruit USB+Serial LCD backpack
with code mods to support BackpackPlus version.
---> http://www.adafruit.com/category/63_96

Written by Enchanted Engineering.
BSD license, check license.txt for more information
All text above must be included in any redistribution
'''

import serial
import shlex
import sys
from time import sleep
import re

# defines defaults...
port = 'COM1'
cmd = None
lcdData = []
file = None
interactive = False
verbose = False
quiet = False
text = ''

# lcd command definitions
commands = {
  'setbaud': {
    'code':               0x39,
    'desc':               'Set serial baud rate and writes to EEPROM.',
    'note':               '0x53:1200, 0x29:2400, 0xCf:4800, 0x67:9600, 0x33:19200, 0x22:28800, 0x19:38400, 0x10:57600, 0x08:115200',
    'format':             'SetBaud <hex_value>'
    },
  'changesplash': {
    'code':               0x40,
    'desc':               'Sets power-on splash screen, text:screen_size, i.e. 16x2=32 characters. (Adafruit Backpack only)',
    'format':             'ChangeSplash "splash screen text"'
    },
  'savesplash': {
    'code':               0x40,
    'desc':               'Write LCD display to power-onsplash screen. (For BackpackPlus only. Wait >0.5s after)',
    'note':               'Saves screen text to EEPROM as splash screen',
    'format':             'SaveSplash'
    },
  'splashdelay': {
    'code':               0x41,
    'desc':               'Sets the splash screen delay, in ~0.1 seconds units',
    'format':             'SplashDelay <quarter_seconds>'
    },
  'displayontimed': {
    'code':               0x42,
    'desc':               'Turn display backlight and auto off after timeout (ignored for Adafruit Backpack)',
    'note':               '0-127 in minutes, 128 + 0-127 seconds, i.e. MSB:0->minutes, MSB:1->seconds',
    'format':             'DisplayOnTimed <timeout>'
    },
  'displayon': {
    'code':               0x45,
    'desc':               'Turn display backlight on',
    'format':             'DisplayOn'
    },
  'displayoff': {
    'code':               0x46,
    'desc':               'Turn display backlight off',
    'format':             'DisplayOff'
    },
  'setcursor': {
    'code':               0x47,
    'desc':               'Move cursor to (COL,ROW)',
    'format':             'SetCursor <col> <row>'
    },
  'gohome': {
    'code':               0x48,
    'desc':               'Move cursor to (1,1)',
    'format':             'GoHome'
    },
  'underlinecursoron': {
    'code':               0x4A,
    'desc':               'Turn on the underline cursor',
    'format':             'UnderlineCursorOn'
    },
  'underlinecursoroff': {
    'code':               0x4B,
    'desc':               'Turn off the underline cursor',
    'format':             'UnderlineCursorOff'
    },
  'cursorback': {
    'code':               0x4C,
    'desc':               'Move cursor back one space, wrap to end',
    'format':             'CursorBack'
    },
  'cursorforward': {
    'code':               0x4D,
    'desc':               'Move cursor forward one space, wrap to beginning',
    'format':             'CursorForward'
    },
  'createcustomcharacter': {
    'code':               0x4E,
    'desc':               'Create custom character #0-7 with 8 bytes of data direct to character RAM',
    'format':             'CreateCustomCharacter <#0-7> <byte1> <byte2> <byte3> <byte4> <byte5> <byte6> <byte7> <byte8>'
    },
  'setcontrast': {
    'code':               0x50,
    'desc':               'Set backlight contrast',
    'format':             'SetContrast <contrast>'
    },
  'autoscrollon': {
    'code':               0x51,
    'desc':               'Scroll display lines, where new lines appear at bottom',
    'format':             'AutoscrollOn'
    },
  'autoscrolloff': {
    'code':               0x52,
    'desc':               'Do NOT scroll display, where new lines overwrite at top',
    'format':             'AutoscrollOff'
    },
  'blockcursoron': {
    'code':               0x53,
    'desc':               'Turn on the Block cursor',
    'format':             'BlockCursorOn'
    },
  'blockcursoroff': {
    'code':               0x54,
    'desc':               'Turn off the Block cursor',
    'format':             'BlockCursorOff'
    },
  'gpioreadSavemask': {
    'code':               0x55,
    'desc':               'Save port mask and read the 4 general purpose I/O pins [PC7 Pc4 PC2 PB0]',
    'note':               'Mask hi nibble masks bits; mask lo nibble inverts bits, (port&lo)^hi, i.e. (port&mask)^(mask>>4)',
    'format':             'GPIOReadSaveMask <mask>'
    },
  'gpiooff': {
    'code':               0x56,
    'desc':               'Set the general purpose pin 1-4 to LOW (0V)',
    'format':             'GPIOOff <pin>'
    },
  'gpioon': {
    'code':               0x57,
    'desc':               'Set the general purpose pin 1-4 to HIGH (5V)',
    'format':             'GPIOOn <pin>'
    },
  'clearscreen': {
    'code':               0x58,
    'desc':               'Clear text on display',
    'format':             'ClearScreen'
    },
  'gpioread': {
    'code':               0x59,
    'desc':               'Reads the 4 general purpose I/O pins [PC7 PC4 PC2 PB0] with preset mask',
    'note':               'See GPIOReadSaveMask for mask usage.',
    'format':             'GPIORead'
    },
  'savecontrast': {
    'code':               0x91,
    'desc':               'Set backlight contrast and save to EEPROM',
    'format':             'SaveContrast <contrast>'
    },
  'savebrightness': {
    'code':               0x98,
    'desc':               'Set backlight brightness and save to EEPROM',
    'format':             'SaveBrightness <brightness>'
    },
  'setbrightness': {
    'code':               0x99,
    'desc':               'Set backlight brightness',
    'format':             'SetBrightness <brightness>'
    },
  'loadcustomcharacters': {
    'code':               0xC0,
    'desc':               'Load characters from EEPROM bank, 0-3 into character RAM',
    'format':             'LoadCustomCharacters <bank>'
    },
  'savecustomcharacter': {
    'code':               0xC1,
    'desc':               'Create custom character #0-7 with 8 bytes of data saved to EEPROM bank 0-3',
    'format':             'SaveCustomCharacter <bank> <#0-7> <byte1> <byte2> <byte3> <byte4> <byte5> <byte6> <byte7> <byte8>'
    },
  'gpiostartstate': {
    'code':               0xC3,
    'desc':               'Sets the "initial" state of the GPIO pin, default "input pullup"',
    'note':               'mode=> 0x11:input_pullup, 0x10:input, 0:output_low, 1:output_high',
    'format':             'GPIOStartState <pin> <mode>'
    },
  'savergb': {
    'code':               0xD0,
    'desc':               'Sets the backlight red, green, and blue levels (0-255) and saves to EEPROM',
    'note':               'For BackpackPlus only',
    'format':             'SaveRGB <red> <green> <blue>',
    },
  'savesize': {
    'code':               0xD1,
    'desc':               'Set display size up to 20x4. Only needed once.',
    'format':             'SaveSize <cols> <rows>'
    },
  'testbaud': {
    'code':               0xD2,
    'desc':               'Test non standard baudrate. (Adafruit only, not supported)',
    'format':             'Testbaud',
    },
  'savescrollmode': {
    'code':               0xD3,
    'desc':               'Set scroll mode',
    'format':             'SaveScrollMode <mode>'
    },
  'scalergb': {
    'code':               0xD4,
    'desc':               'Scales the maximum LED intensities red, green, and blue levels (0-255, Adafruit: 100, 190, 255).',
    'format':             'ScaleRGB <red> <green> <blue>',
    },
  'setrgb': {
    'code':               0xD5,
    'desc':               'Sets the backlight red, green, and blue levels (0-255).',
    'format':             'SetRGB <red> <green> <blue>',
    },
  'debug': {
    'code':               0xDC,
    'desc':               'Set debug mode flags',
    'note':               'For BackpackPlus only',
    'format':             'Debug <mode_flags>'
    },
  'dumpeeprom': {
    'code':               0xDD,
    'desc':               'Dump (4 byte) pages of EEPROM.',
    'note':               'For BackpackPlus only',
    'format':             'DumpEEPROM <first_page> <last_page>'
    },
  'editeeprom': {
    'code':               0xDE,
    'desc':               'Edit (write 4 byte) pages of EEPROM',
    'note':               'For BackpackPlus only',
    'format':             'EditEEPROM <page> <byte1> <byte2> <byte3> <byte4>'
    },
  'test': {
    'code':               0xDF,
    'desc':               'Free format test command for code development',
    'note':               'For BackpackPlus only',
    'format':             'Test <byte> <optional?>'
    }
  }

# script actions
scriptActions = {
  'sleep': {
    'desc':               'Temporarily pause command file processing',
    'format':             'Sleep <seconds>'
    },
  'text': {
    'desc':               'Send a encoded text string: \0-7=>char 0-7, \8=> backspace \\r=> char(14), \\n=> char(10)',
    'format':             'Text "string"'
    },
  'raw': {
    'desc':               'Adafruit format command (for cut and paste from Adafruit scripts)',
    'format':             'matrixwritecommand([<data byte array>])'
    },
  'transcripting': {
    'desc':               'Turn verbose scripting on or off',
    'format':             'Transcripting Quiet|Normal|Verbose'
    },
  'quit': {
    'desc':               'End command file or interactive processing',
    'format':             'Quit'
    }
  }

# help info...
def help():
  print("syntax: python3 lcd.py [-p <port>] [-v] -h | -i | -f <command file> | -c <command> <parameters> | text")
  print("  where ...")
  print("    -p <port>                  specifies the serial port for communications, default COM1")
  print("    -v                         specifies verbose scripting")
  print("    -h                         specifies this help")
  print("    -i                         specifies interactive mode")
  print("    -f <command file>          specifies a file containing commands")
  print("    -c <command> <parameters>  specifies a human readable command interpreted and sent to LCD with any parameters")
  print("    text                       specifies encoded text sent to the LCD")
  print("  where commands include...")
  for i in sorted(commands.keys()):
    print("    %s" % commands[i]['format'])
    print("      %s" % commands[i]['desc'])
    if 'note' in commands[i]:
      print("      "+commands[i]['note'])
  print()
  print("  where special script actions include...")
  for i in sorted(scriptActions.keys()):
    print("    %s" % scriptActions[i]['format'])
    print("      %s" % scriptActions[i]['desc'])

# format bytes in 2 character hex strings...
def chHex(c):
  return "0x%02X" % c

# return ordinal value for integer or hex strings
def x2ord(x):
  if isinstance(x, str):
    if x.startswith('0x'):
      x = int(x, 16)
    else:
      x = int(x)
  return x

# decode a text string for writing to LCD
def decodeText(text):
  tmp = text.replace('\\b', chr(8))
  tmp = text.replace('\\n', chr(10))
  tmp = tmp.replace('\\r', chr(13))
  for c in '01234567':
    tmp=tmp.replace('\\'+c, chr(int(c)))
  return tmp

# convert a command list to bytes as needed and send to LCD
def commandLCD(commandList):
  data = list(map(x2ord, commandList))
  if not quiet:
    print(str(list(map(chHex, data))))
  for c in data:
    lcd.write(c.to_bytes(1, byteorder='little'))

# send serial data to
def sendData(action, data):
  global verbose, quiet
  if action in commands:
      # process command
    if verbose: # a little debug info...
      print("command...")
      print("  %s" % commands[action]['format'])
  elif action in scriptActions:
    # script actions
    if verbose: # a little debug info...
      print("script action...")
      print("  format: %s" % scriptActions[action]['format'])
    if action=='sleep':
      if verbose:
        print("    SLEEPING %s[seconds]" % data)
      sleep(data)
      data = []
    elif action=='text':
      if verbose:
        print("    TEXT:", data)
      data = list(map(ord, decodeText(data)))
    elif action=='transcripting':
      verbose = (data=='VERBOSE')
      quiet = (data=='QUIET')
      #print "TRANSCRIPTING :", 'VERBOSE' if verbose else "QUIET" if quiet else "NORMAL"
      data = []
    elif action=='raw':
      if verbose:
        print("    Raw command:", data[1:])
  elif action == '-???-':
    print("UNKNOWN COMMAND: try \"help\" for more info")
    return
  else:
    # assume text
    if verbose: # a little debug info...
      print("text...")
      print("  \"%s\"" % action)
  if len(data):
    commandLCD(data)

# interpret a command string into a commandList
def interpretCmd(tokens):
  lcdData = []
  rawtext = tokens[0]
  cmd = tokens.pop(0).lower()
  if cmd in commands:
    lcdData.append(0xFE)
    lcdData.append(commands[cmd]['code'])
    size = len(shlex.split(commands[cmd]['format']))-1
    if len(tokens)!=size:
      if commands[cmd]['code']==0xDF:
        size = len(tokens)
      else:
        print('WARNING: Incorrect number of parameters given [%i] for command [%i]: %s' % (len(tokens), size, commands[cmd]['format']))
    for i in range(0, size):
      tmp = tokens.pop(0)
      if commands[cmd]['code'] in [0x34, 0x40]:
        if tmp.length:
          lcdData += list(map(ord, decodeText(tmp))) # special case: break string into ordinal list add to array
      else:
        if tmp.startswith("0x"):
          tmp = int(tmp[2:], 16)
        elif tmp==str(int(tmp)):
          tmp = int(tmp)
        lcdData.append(tmp)
  elif cmd in scriptActions:
    if cmd == 'sleep':
      lcdData = (float(tokens.pop()));
    if cmd == 'transcripting':
      lcdData = tokens.pop().upper()
    if cmd == 'text':  # treat as a encoded line of text
      lcdData = tokens.pop()
    if cmd == 'raw': # Adafruit command format
      lcdData = [0xFE] + tokens
  else:
    if len(tokens)==0:
      return interpretCmd(['text', rawtext]) # assume bare text
    else:
      cmd = '-???-' # UNKNOWN COMMAND
  return [cmd, lcdData]

# split lines by whitespace and commas, but preserve quoted strings
def linesplit(line):
  ss = shlex.shlex(line, posix=True)
  ss.whitespace += ','
  ss.whitespace_split = True
  sx = list(ss)
  return sx

def sendLine(line, wait):
  if line:
    if line.startswith('matrixwritecommand'):
      [cmd, data] = interpretCmd(['raw'] + line[line.index('[')+1:line.index(']')].replace(',', ' ').split())
    else:
      [cmd, data] = interpretCmd(linesplit(line))
    sendData(cmd, data)
  for w in range(0, (1000*wait)+1):
    sleep(0.001)
    if lcd.in_waiting:
      print("  > "+lcd.readline().strip())

# get arguments, remove script name
args = sys.argv[1:]
# usage check...
if len(args)==0 or 'h' in args:
  help()
  exit(0)

# parse commandline
while (args):
  x = args.pop(0)
  if x=='-p' and len(args):
    port = args.pop(0)
  elif x=='-i':
    interactive = True
  elif x=='-v':
    verbose = True
  elif x=='-f' and len(args):
    file = args.pop(0)
  elif x=='-c' and len(args):
    [cmd, lcdData] = interpretCmd(args)
  else:
    text = x
    lcdData = list(map(ord, x))

# define serial connection to LCD...
if verbose:
  print("port: " + port)
  print("  flushing serial data...")
lcd = serial.Serial(port, 9600, timeout=1)
# clear queue
sendLine('', 1)


# commandline actions...
# write command request to LCD...
if cmd:
  sendData(cmd, lcdData);
# just write text to LCD...
if text:
  sendData(text, lcdData);
# read commands from a file
if file:
  if verbose:
    print("command file: ", file, "...")  # a little debug info...
  commandlines = open(file, "r").readlines()
  n = 0
  for line in commandlines:
    n += 1
    line = re.sub(r'#.*', '', line).strip() # exclude comments
    if not line: continue                 # skip empty lines
    if line.lower()=='quit':
      print("Quitting command file at line %i of %i lines..." % (n, len(commandlines)))
      break
    if sendLine(line, 0):
      break

# interactive mode
while (interactive):
  # readline from prompted stdin...
  line = input("CMD>").strip()
  if line.lower()=='help':
    help()
  elif line.lower()=='quit':
    interactive = False
    print("Quitting interactive mode...")
    break
  else:
    sendLine(line, 1)

# final check for data...
sendLine("", 1)
lcd.close()
