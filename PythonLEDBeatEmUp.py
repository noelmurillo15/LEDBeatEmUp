import time
import serial
import msvcrt
import random
import threading
import serial.tools.list_ports


print("\nInitializing")
#   Establish serial connection
arduinoData = serial.Serial('COM3', 115200,timeout=0)   #port, baud rate, no timeout

#   Global Variables 
delayval = 1                         #   Data transfer delay (~Difficulty - 1 = Easy | < 1 = Hard)
inputDelay = 0.1                 #   Delay for input   
userInput = b' 0'                  #   Input buffer
gameInProgress = False    #   Has the game started?
time.sleep(2)                        #   Wait for serial connection

#   List of active com ports, just in case
ports = list(serial.tools.list_ports.comports())
for p in ports:
    print(p)

#   Connection Attempt (Receives 1-Start Game, 7-Quit)
var = input("Python: Establish Connection ? (Y or N)")
if(var == 'Y' or var == 'y'):
    arduinoData.write(b'Y')     #   Data to Arduino has to be a byte
    time.sleep(delayval)
    msg = (arduinoData.readline().strip())
    if(len(msg) > 0):        
        if(msg == b'\x07'):     #   bytes go from 0-7
            print("Python: Connection Failed...")
        if(msg == b'\x01'):     
            gameInProgress = True
            print("Python: Connection is Successful!")
            time.sleep(2)
        print("Arduino : ", msg)

#  Send data to Arduino every second on seperate thread
def arduinoDataTransfer():  
    threading.Timer(delayval, arduinoDataTransfer).start()
    x = random.randint(0,39)
    arduinoData.write(bytes([x]))

#   Input Thread - catches keyboard input while still reading data
def inputThread(inputEvent):
    while not inputEvent.isSet():
        event_is_set = inputEvent.wait()     # returns true only if internal flag is true
        if event_is_set:                                    # if input was detected
            arduinoData.write(userInput)        # send code to arduino
            inputEvent.clear()                           # sets internal flag to False            
        else:
            try:    # else try to catch any msgs from arduino
                msg = (arduinoData.readline().strip())        
                if(len(msg) > 0):
                    print("Arduino : ", msg) 
                    if(msg == b'\x07'): gameInProgress = False
            except serial.SerialException:
                print("Python: Uh Oh Serial Error")

#Initialize Threads and Start Game
if(gameInProgress): 
    #   Start Input Event
    inputEvent = threading.Event()
    #   Start Input Thread
    t1 = threading.Thread(name='Input Thread', target=inputThread, args=(inputEvent,))
    t1.start()
    #   Start sending arduino random data
    arduinoDataTransfer()

# Gameplay Loop
while gameInProgress:
    #   If keyboard press detected & input thread flag == False
    if msvcrt.kbhit() and not inputEvent.is_set():  
        key = msvcrt.getch()    #   Fetch char 
        if (len(key)>0 ):                    
            if key == b'H':      #   Up
                userInput = key    
                inputEvent.set() #   Sets internal flag to True
            elif key == b'P':   #   Down
                userInput = key    
                inputEvent.set()
            elif key == b'K':   #   Left
                userInput = key    
                inputEvent.set()
            elif key == b'M':   #   Right
                userInput = key    
                inputEvent.set()
    time.sleep(inputDelay)

print("Exiting...")
quit()
