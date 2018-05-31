import time
import serial
import random
import threading
import serial.tools.list_ports
# coding: utf8

print("Initializing")
# Establish serial connection
arduinoData = serial.Serial('COM3', 115200,timeout=0)
gameInProgress = False   #   Has the game started?
delayval = 1                        #   How Fast Green Dots Appear(~Difficulty)
time.sleep(2)                       #   Wait for Arduino to Catch up

#   List of active com ports, just in case
ports = list(serial.tools.list_ports.comports())
for p in ports:
    print(p)

#   Connection Attempt    #Receives 1-Start Game, 99-Quit
var = input("Python: Establish Connection ? (Y or N)")
if(var == 'Y' or var == 'y'):
    gameInProgress = True
    arduinoData.write(b'Y')
    time.sleep(delayval)
    msg = (arduinoData.readline().strip())
    if(len(msg) > 0 and msg != ' '):
        print(msg) 
        if(msg == b'\x07'):
            gameInProgress = False
            print("Python: Connection Failed...")
        if(msg == b'\x01'):            
            gameInProgress = True
            print("Python: Connection is Successful!")
            time.sleep(4)

def sendDataArduino():
    threading.Timer(delayval, sendDataArduino).start()
    x = random.randint(0,39)
    arduinoData.write(bytes([x]))
    #print("Python:", x )

sendDataArduino()
# Waiting on Serial Read (Gameplay via Bt serial blocks Pc serial)
while gameInProgress:    
    try:
        msg = (arduinoData.readline().strip())        
        if(len(msg) > 0):
            print(msg) 
            if(msg == b'\x07'): gameInProgress = False
    except serial.SerialException:
        print("Python: Uh Oh Serial Error")
        time.sleep(delayval)
    
# Game Close
else:
    arduinoData.close()    
    print("Python: Shutting Down")
exit()



