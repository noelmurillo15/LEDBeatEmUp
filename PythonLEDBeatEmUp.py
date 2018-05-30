import time
import serial
import random


print("Initializing\n")
arduinoData = serial.Serial('COM3', 9600,timeout=0) # Establish serial connection
gameInProgress = False   #   Has the game started?
delayval = 1                        #   How Fast Green Dots Appear(~Difficulty)
time.sleep(2)                       #   Wait for Arduino to Catch up


var = input("Start the Game Y/N?")
if(var == 'Y' or var == 'y'):
    print("Game Start")
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
    except serial.SerialException:
        print("Uh Oh Serial Error\n")
    time.sleep(delayval)
else:
    arduinoData.close()
    print("Shutting Down\n")
    time.sleep(delayval)
exit()



