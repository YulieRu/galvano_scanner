import serial
import time
import numpy as np

class Stepper(object):
    def __init__(self, 
                dict_set={"port": "COM4", 
                           "baudrate": 115200, 
                           "timeout": 0.001, 
                           "parity": serial.PARITY_NONE,
                           "stopbits": serial.STOPBITS_ONE},
                manage_type="marlin"):
        
        if (manage_type == "marlin"):
            try:
                self.stepper = serial.Serial(port=dict_set["port"], 
                                            baudrate=dict_set["baudrate"],
                                            timeout=dict_set["timeout"],
                                            parity=dict_set["parity"],
                                            stopbits=dict_set["stopbits"])
                self.manage_type = manage_type
                print("Блок управления инициализирован успешно!")
            except:
                print("Произошла ошибка инициализации блока управления!")
    
        if (manage_type == "myduino"):
            try:
                self.stepper = serial.Serial(port=dict_set["port"], 
                                            baudrate=dict_set["baudrate"],
                                            timeout=dict_set["timeout"],
                                            parity=dict_set["parity"],
                                            stopbits=dict_set["stopbits"])
                
                self.manage_type = manage_type
                for i in range(0, 200, 1):
                    if self.stepper.inWaiting() <= 0:
                        time.sleep(0.05)
                    else:
                        status_connection = self.stepper.read_until("\r\n").decode().strip("\r\n")
                        if status_connection == "Start!":
                            print("Блок управления инициализирован успешно!")
                        else:
                            print("Произошла ошибка инициализации блока управления!")
                            self.stepper.close()
            except:
                print("Произошла ошибка инициализации блока управления!")
    
    def __del__(self):
        self.stepper.write(bytes(f"[disable]\n".encode()))
        self.stepper.close()
        print("Закончил работу!")

    def Recv(self, N_iterations = 20000, timeout=0.001):
        #Time_readable = N_iterations * timeout
        if self.manage_type == "marlin":
            for i in range(0, N_iterations, 1):
                if self.stepper.inWaiting() <= 0:
                    time.sleep(timeout)
                else:
                    receive = self.stepper.readline().decode().strip("\n")
                    break;
            return receive
        
        if self.manage_type == "myduino":
            for i in range(0, N_iterations, 1):
                if self.stepper.inWaiting() <= 0:
                    time.sleep(timeout)
                    receive = 0
                else:
                    receive = self.stepper.read_until("\r\n").decode().strip("\r\n")
                    break;
            return receive

    def home(self, homing_state = {"state": True, "axes_homing": ["X"]}):
        if (self.manage_type == "marlin") and (homing_state["state"] == True):
            try:
                str_home = "G28 "
                for axes in homing_state["axes_homing"]:
                    str_home += f"{axes} "
                self.stepper.write(bytes(f"{str_home.strip()}\n".encode()))
                while True:
                    message = self.stepper.readline()
                    if message == b'ok\n':
                        print("Парковка завершена, выхожу из цикла")
                        break
                    if message == b'echo:busy: processing\n':
                        pass
            except:
                print("Произошла ошибка отправки команды!")
        
        if ((self.manage_type == "myduino") and (homing_state["state"] == True)):
            try:
                self.stepper.write(bytes(f"[home]\n".encode()))
                if (self.Recv() == "Parking done!"):
                    return 1
                else:
                    return 0
            except:
                return 0

    def linear_move(self, X=0.0, Y=0.0, Z=0.0, feedrate=100, pause=None):
        if (self.manage_type == "marlin"):
            if pause is None:
                self.stepper.write(bytes(f"G1 X{X} Y{Y} Z{Z} F{feedrate}\n".encode()))
                while True:
                    if self.stepper.readline() == b'ok\n':
                        break
            else:
                self.stepper.write(bytes(f"G1 X{X} Y{Y} Z{Z} F{feedrate}\n".encode()))
                while True:
                    if self.stepper.readline() == b'ok\n':
                        break
                time.sleep(pause)
        
        if (self.manage_type == "myduino"):
            self.stepper.write(bytes(f"[linearmove={X}]\n".encode()))
            if self.Recv() == "Ready":
                return 1
            else:
                return 0