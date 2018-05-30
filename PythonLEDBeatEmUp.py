import time
import serial
import random

arduinoData = serial.Serial('COM3', 9600,timeout=0)
print("Initializing\n")

delayval = 1
gameInProgress = False
time.sleep(2)

var = input("Start the Game Y/N?")
if(var == 'Y' or var == 'y'):
    print("Game Start")
    gameInProgress = True
    arduinoData.write(b'Y')
    time.sleep(delayval*5)

while gameInProgress:
    #   Serial Write
    #print("Python Random Number : ")
    x = random.randint(0,39)
    arduinoData.write(bytes([x]))    
    # Serial Read
    try:
        msg = (arduinoData.readline().strip())        
        if(len(msg) > 0 and msg != ' '):
            print(msg) 
        if(msg == 99): gameInProgress = False 
    except serial.SerialException:
        print("Uh Oh Serial Error\n")
    time.sleep(delayval)
else:
    arduinoData.close()
    print("Shutting Down\n")
    time.sleep(delayval)
exit()



