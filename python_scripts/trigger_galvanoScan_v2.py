from tkinter import *
from time import sleep
import serial
import pyautogui
from time import perf_counter_ns
#from pytictoc import TicToc

serialTrigger, serialGalvano = None, None

def spinwait_us(delay):
    target = perf_counter_ns()+delay*1e6
    while perf_counter_ns() < target:
        pass

def send_command():
    num_iter = num_iter_box.get(1.0, 'end-1c')
    pause_ms = delay_pause_box.get(1.0, 'end-1c')
    galvano_angle = galvano_delta_box.get(1.0, 'end-1c')
    galvano_axis = galvano_axis_box.get(1.0, 'end-1c')
    galvano_steps = galvano_steps_box.get(1.0, 'end-1c')
    galvano_pause = galvano_pause_box.get(1.0, 'end-1c')

    #"[RECORD=10 5000]"
    commandTrig = '[RECORD=' + num_iter + ' ' +\
                         pause_ms + ']'

    print(commandTrig)

    #serialTrigger = open_port()
    if not serialTrigger or not serialGalvano:
        lbl["text"] = f'Not connected'
        return 0
    serialTrigger.write(commandTrig.encode())

    #for i in range(1, int(galvano_steps)+1):
        #commandGalvano = '[mcp_'+ galvano_axis + '=' + str(i*int(galvano_angle)) + ']'
        #print(commandGalvano)
    commandGalvano = '[RECORD=' + str(int(galvano_steps)) + ' ' + \
                      str(int(galvano_pause)) + ' ' + \
                      str(int(galvano_angle))+']'

    serialGalvano.write(commandGalvano.encode())
        #sleep(int(galvano_pause) / 1000.0)
        #spinwait_us(int(galvano_pause))

    #commandGalvano = '[mcp_' + galvano_axis + '=' + '0' + ']'
    #serialGalvano.write(commandGalvano.encode())
    sleep(int(pause_ms) // 1000)
    #pyautogui.press("Enter")
    lbl["text"] = "Command was sent."



def readSerial():
    response = serialTrigger.readline()
    response = response.decode('utf-8')
    print(response)

def open_port_trigger():
    global serialTrigger
    num_port = numCOMport_trigger_box.get(1.0, 'end-1c')
    baud = 115200
    try:
        serialTrigger = serial.Serial(port=num_port,
                                      baudrate=baud,
                                      timeout=3,
                                      writeTimeout=1,
                                      parity=serial.PARITY_NONE,
                                      stopbits=serial.STOPBITS_ONE)
        lblOpenPrtTrig["text"] = f'Trigger is connected'
        print(f'{num_port} is connected')
    except serial.serialutil.SerialException:
        if not serialTrigger:
            lblOpenPrtTrig["text"] = f'Trigger is not connected'

def open_port_galvano():
    global serialGalvano
    num_port = numCOMport_galvan_box.get(1.0, 'end-1c')
    baud = 115200
    try:
        serialGalvano = serial.Serial(port=num_port,
                                      baudrate=baud,
                                      timeout=3,
                                      writeTimeout=1,
                                      parity=serial.PARITY_NONE,
                                      stopbits=serial.STOPBITS_ONE)
        lblOpenPrtGalvan["text"] = f'Galvanoscanner is connected'
        print(f'{num_port} is connected')
    except serial.serialutil.SerialException:
        if not serialGalvano:
            lblOpenPrtGalvan["text"] = f'Galvanoscanner is not connected'

win = Tk()
win.title('Command')
win.geometry("600x200")

# Creating a text box widget
numCol, numRow = 0, 1
label_num_iter = Label(win, text='Number of iterations (trigger)')
label_num_iter.grid(column=numCol, row=numRow)
num_iter_box = Text(win, height=1, width=40)
num_iter_box.insert(1.0, 5)
num_iter_box.grid(column=numCol+1, row=numRow)

numRow +=1
label_pause = Label(win, text='Pause between iterations (ms), (trigger)')
label_pause.grid(column=numCol, row=numRow)
delay_pause_box = Text(win, height=1, width=40)
delay_pause_box.insert(1.0, 1000)
delay_pause_box.grid(column=numCol+1, row=numRow)

numRow +=1
label_galvano_steps = Label(win, text='Number of steps, (galvanoscanner)')
label_galvano_steps.grid(column=numCol, row=numRow)
galvano_steps_box = Text(win, height=1, width=40)
galvano_steps_box.insert(1.0, 5)
galvano_steps_box.grid(column=numCol+1, row=numRow)

numRow +=1
label_galvano_delta = Label(win, text='The angle (0-4095), (galvanoscanner)')
label_galvano_delta.grid(column=numCol, row=numRow)
galvano_delta_box = Text(win, height=1, width=40)
galvano_delta_box.insert(1.0, 400)
galvano_delta_box.grid(column=numCol+1, row=numRow)

numRow +=1
label_galvano_pause = Label(win, text='Pause between steps (ms), (galvanoscanner)')
label_galvano_pause.grid(column=numCol, row=numRow)
galvano_pause_box = Text(win, height=1, width=40)
galvano_pause_box.insert(1.0, 1000)
galvano_pause_box.grid(column=numCol+1, row=numRow)

numRow +=1
label_galvano_axis = Label(win, text='Axis for rotation, (galvanoscanner)')
label_galvano_axis.grid(column=numCol, row=numRow)
galvano_axis_box = Text(win, height=1, width=40)
galvano_axis_box.insert(1.0, 'x')
galvano_axis_box.grid(column=numCol+1, row=numRow)

numRow +=1
label_numCOMport = Label(win, text='COMport num. for trigger')
label_numCOMport.grid(column=numCol, row=numRow)
numCOMport_trigger_box = Text(win, height=1, width=40)
numCOMport_trigger_box.insert(1.0, 'COM8')
numCOMport_trigger_box.grid(column=numCol+1, row=numRow)

numRow +=1
label_numCOMport = Label(win, text='COMport num. for galvanoscanner')
label_numCOMport.grid(column=numCol, row=numRow)
numCOMport_galvan_box = Text(win, height=1, width=40)
numCOMport_galvan_box.insert(1.0, 'COM10')
numCOMport_galvan_box.grid(column=numCol+1, row=numRow)

#open_btn = Button(win, text="Open Text File", command=open_text)

# Create a button to save the text
numRow +=1
comBut = Button(win, text="Send command", command=send_command)
comBut.grid(column=numCol, row=numRow, pady=10, padx=10)
lbl = Label(win, text="No command received yet.")
lbl.grid(column=numCol, row=numRow+1, pady=10, padx=10)

#numRow=5
numCol=1
portButton = Button(win, text='COMport trigger', command=open_port_trigger)
portButton.grid(column=numCol, row=numRow, pady=10, padx=10)
lblOpenPrtTrig = Label(win, text="No COMport")
lblOpenPrtTrig.grid(column=numCol, row=numRow+1, pady=10, padx=10)

#numRow=5
numCol=2
portButton = Button(win, text='COMport galvanoscanner', command=open_port_galvano)
portButton.grid(column=numCol, row=numRow, pady=10, padx=10)
lblOpenPrtGalvan = Label(win, text="No COMport")
lblOpenPrtGalvan.grid(column=numCol, row=numRow+1, pady=10, padx=10)

win.mainloop()
