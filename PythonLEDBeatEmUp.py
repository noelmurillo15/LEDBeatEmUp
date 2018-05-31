import time
import serial
import random


print("Initializing")
# Establish serial connection
arduinoData = serial.Serial('COM3', 115200,timeout=0)
gameInProgress = False   #   Has the game started?
delayval = 1                        #   How Fast Green Dots Appear(~Difficulty)
time.sleep(2)                       #   Wait for Arduino to Catch up

#   List of active com ports, just in case
import serial.tools.list_ports
ports = list(serial.tools.list_ports.comports())
for p in ports:
    print(p)

#   Connection Attempt    #Receives 1-Start Game, 99-Quit
var = input("\nPython: Establish Connection ? (Y or N)")
if(var == 'Y' or var == 'y'):
    gameInProgress = True
    arduinoData.write(b'Y')
    time.sleep(delayval)
    msg = (arduinoData.readline().strip())
    if(len(msg) > 0 and msg != ' '):
        print(msg) 
        if(msg == 99):
            gameInProgress = False
            print("Python: Connection Failed...")
        if(msg == 1):            
            print("Python: Connection is Successful!")
            
#Transmit Attempt up to 16 chars
print("Enter your name: ")
while True:
    i = input("")
    if not i:
        break
    print("Ready to begin, ", i )
    
#   Game Start!
print("Python: Initializing Game")
gameInProgress = True

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
    except serial.SerialException:
        print("Python: Uh Oh Serial Error\n")
    time.sleep(delayval)
    
# Game Close
else:
    arduinoData.close()
    print("Python: Shutting Down")
    time.sleep(delayval)
exit()



