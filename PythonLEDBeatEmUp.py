import time
import serial
import random


print("Initializing\n")
# Establish serial connection
arduinoData = serial.Serial('COM3', 115200,timeout=0)
gameInProgress = False   #   Has the game started?
delayval = 1                        #   How Fast Green Dots Appear(~Difficulty)
time.sleep(2)                       #   Wait for Arduino to Catch up

import serial.tools.list_ports
ports = list(serial.tools.list_ports.comports())
for p in ports:
    print(p)

var = input("Start the Game Y/N?")
if(var == 'Y' or var == 'y'):
    print("Toggle sheeld uart switch ON and connect your Smartphone")
    gameInProgress = True
    arduinoData.write(b'Y')
    time.sleep(delayval*5)


while gameInProgress:
    #   Serial Write
    x = random.randint(0,39)
    arduinoData.write(bytes([x]))    
    # Serial Read
    try:
        msg = (arduinoData.readline().strip())        
        if(len(msg) > 0 and msg != ' '):
            print(msg) 
        if(msg == 99): gameInProgress = False
        if(msg == 1):gameInProgress = True
    except serial.SerialException:
        print("Uh Oh Serial Error\n")
    time.sleep(delayval)
else:
    arduinoData.close()
    print("Shutting Down\n")
    time.sleep(delayval)
exit()



