import socket
from evdev import list_devices, InputDevice, categorize, ecodes

#CENTER_TOLERANCE = 350

TCP_IP = '192.168.4.1'
TCP_PORT = 12345


try:
  dev = InputDevice( list_devices()[0] )
except:
  print("Controller Issue!")
  exit()

axisRef = {
  0: 'ls-x',
  1: 'ls-y',
  2: 'lt',
  3: 'rs-x',
  4: 'rs-y',
  5: 'rt'
}

btnRef = {
  304: 'A',
  305: 'B',
  307: 'X',
  308: 'Y',
  310: 'LB',
  311: 'RB',
  317: 'LS',
  318: 'RS',
  316: 'xbox',
  314: 'home',
  315: 'menu'
}

controllerState = {
  'ls-x': 0,
  'ls-y': 0,
  'rs-x': 0,
  'rs-y': 0,
  'lt': 0,
  'rt': 0,
  'dpad-up': False,
  'dpad-down': False,
  'dpad-left': False,
  'dpad-right': False,
  'A': False,
  'B': False,
  'X': False,
  'Y': False,
  'LB': False,
  'RB': False,
  'LS': False,
  'RS': False,
  'xbox': False,
  'home': False,
  'menu': False
}

def leftPad(string:str, targetNum:int):
  return (' ' * (targetNum - len(string))) + string

def btnText(string:str, boolean:bool):
  if boolean:
    return string
  else:
    return '-' * len(string)
  

def printState():
  print(f"\rLS:[{leftPad(str(controllerState['ls-x']), 6)}, {leftPad(str(controllerState['ls-y']), 6)}], " + # Left stick
        f"RS:[{leftPad(str(controllerState['rs-x']), 6)}, {leftPad(str(controllerState['rs-y']), 6)}], " + # Right stick
        f"LT:{leftPad(str(controllerState['lt']), 4)}, " +
        f"RT:{leftPad(str(controllerState['rt']), 4)} [" +
        btnText("A", controllerState['A']) +
        btnText("B", controllerState['B']) +
        btnText("X", controllerState['X']) +
        btnText("Y", controllerState['Y']) +
        btnText("LB", controllerState['LB']) +
        btnText("RB", controllerState['RB']) +
        btnText("LS", controllerState['LS']) +
        btnText("RS", controllerState['RS']) +
        btnText("@", controllerState['xbox']) +
        btnText("H", controllerState['home']) +
        btnText("M", controllerState['menu']) +
        btnText("^", controllerState['dpad-up']) + 
        btnText("v", controllerState['dpad-down']) + 
        btnText("<", controllerState['dpad-left']) +
        btnText(">", controllerState['dpad-right']), end="]             ")

def doDPAD(event):
  if event.code == 16:
    if event.value == -1:
      controllerState['dpad-left'] = True
      controllerState['dpad-right'] = False
    elif event.value == 0:
      controllerState['dpad-left'] = False
      controllerState['dpad-right'] = False
    elif event.value == 1:
      controllerState['dpad-left'] = False
      controllerState['dpad-right'] = True
  elif event.code == 17:
    if event.value == -1:
      controllerState['dpad-up'] = True
      controllerState['dpad-down'] = False
    elif event.value == 0:
      controllerState['dpad-up'] = False
      controllerState['dpad-down'] = False
    elif event.value == 1:
      controllerState['dpad-up'] = False
      controllerState['dpad-down'] = True
      
  
def toShort(num):
  if num > 65535:
    raise OverflowError
  return num.to_bytes(2, 'big', signed=True)

def getButtonBytes():
  buttons = [
    controllerState['A'],
    controllerState['B'],
    controllerState['X'],
    controllerState['Y'],
    controllerState['LB'],
    controllerState['RB'],
    controllerState['LS'],
    controllerState['RS'],
    controllerState['xbox'],
    controllerState['home'],
    controllerState['menu'],
    controllerState['dpad-up'],
    controllerState['dpad-down'],
    controllerState['dpad-left'],
    controllerState['dpad-right'],
    False # Unused button
  ]
  data = 0
  for i in range(16):
    data |= buttons[i] << i
  return data.to_bytes(2, "little", signed=True)
  
      
def getBytes():
  return toShort(controllerState['ls-x']) + \
  toShort(controllerState['ls-y']) + \
  toShort(controllerState['rs-x']) + \
  toShort(controllerState['rs-y']) + \
  toShort(controllerState['lt']) + \
  toShort(controllerState['rt']) + \
    getButtonBytes()
    
    

try:
  s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  s.connect((TCP_IP, TCP_PORT))
  for event in dev.read_loop():
    if event.type == ecodes.EV_KEY:
      controllerState[btnRef[event.code]] = event.value
        
    elif event.type == ecodes.EV_ABS:

      if event.code in [16, 17]:
        doDPAD(event)
      else:
        controllerState[axisRef[event.code]] = event.value
        
    printState()
    s.send(getBytes())
except:
  print("\n\n\nClient not connected!")
