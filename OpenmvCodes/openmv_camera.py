#Color recognition ball export x and y coordinate differences and distance
#Main code uploaded to camera

import sensor, image, time , pyb, os, tf, math, uos, gc
from math import pi
from pyb import UART
from pyb import Timer
from pyb import LED
import json
import math
# import micropython
import ustruct

# micropython.alloc_emergency_exception_buf(1000)


led = pyb.LED(2) # Red LED = 1, Green LED = 2, Blue LED = 3, IR LEDs = 4.
thresholds = [#(48, 89, 54, -70, 98, 44), # Color threshold
            (55, 80, -30, -2, 15, 48),
             (26, 74, -58, -10, 61, 14),
             (97, 56, -38, 73, 23, 127)]
threshold_index = 0 # 0 for red, 1 for green, 2 for blue


sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)  # 320*240
# sensor.set_windowing((240, 240))
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False) # must be turned off for color tracking
sensor.set_auto_whitebal(False) # must be turned off for color tracking
sensor.set_auto_exposure(False)
Focus = 1.25                                 # Focal length in mm
# F=1.25
# In QQVGA:
# Actual size of each pixel
W_Pix = 2.05/160                                # in mm
H_Pix = 2.755/120                               # in mm

# Convert pixel coordinate difference to angle difference
def pix2ang(pix):
    delta_angle = 180/pi*math.atan((pix*W_Pix)/(2*Focus))         # Approximate as field of view calculation
    return delta_angle

clock = time.clock()
uart = UART(3, 115200)
uart.init(115200, bits=8, parity=None, stop=1, timeout_char=1000) # Initialize with given parameters

def tick(timer):            # we will receive the timer object when being called
    global data
    if blobs:
        #print("Find")
        #print('Distance(mm):',output_cood)
        # print('Coordinate (x,y)', output_pix)
        uart.write(data)
    else:
        #print("NO FIND")
        uart.write(data)
        # print("Data to be sent: ", data) ## Check data in else condition


tim = Timer(4, freq=10)      # create a timer object using timer 4 - trigger at 1Hz
tim.callback(tick)          # set the callback to our tick function

def find_max(blobs):
    max_size=0
    for blob in blobs:
        if blob[2]*blob[3] > max_size:
            max_blob=blob
            max_size = blob[2]*blob[3]
    return max_blob

def Uart_Receive():   #UART receive, change color threshold for framing the ball
    global threshold_index
    if uart.any():
        temp_data = uart.readchar()
        if temp_data==0:   #Red
           threshold_index=0

           # print(temp_data,threshold_index)
        elif temp_data==1:
           threshold_index=1
           # print(temp_data,threshold_index)

#Function definition for sliding slice to get average of l values
def process_l_value(l):
    global l_window, l_index, avg_l
    l_window[l_index] = l
    # print("l_window:", l_window)
    # print("l_index:", l_index)
    avg_l = sum(l_window) / len(l_window)
    # print("Average of last 10 values:", avg_l)
    l_index = (l_index + 1) % 10

l_window = [0] * 10  # Initialize a list of length 10, initial value 0
l_index = 0  # Initialize list index

while True:
    clock.tick()
    img = sensor.snapshot()
    Uart_Receive()
    blobs = img.find_blobs([thresholds[threshold_index]])


    if blobs:
        led.on()
        max_blob = find_max(blobs)
        img.draw_rectangle(max_blob[0:4])                               # Draw rectangle
        img.draw_cross(max_blob.cx(), max_blob.cy())                    # Draw cross
        b = max_blob.x() # x-coordinate for the blob
        L = (max_blob.w()+max_blob.h())/2
        # print("L*10:", L*10)
        l = int(8800/L*10)
        process_l_value(l)
# L_real = K/(Radius of Object, in pixels)
# Calibration: At a real distance of 60cm, L is approximately =(33.5+34.0)/2=33.75
# k=60*33.75=2025
# Verified that in the range [20cm~80cm], the error is within 2%

        # Pixel position difference (in pixels)
        x_err = max_blob.cx()-img.width()/2                             # Pan adjusts x direction position difference
        pan_err_real = pix2ang(x_err);
        # Online debugging print
        #print("pan error:", pan_err_real)
        y_err = max_blob.cy()-img.height()/2                            # Tilt adjusts y direction position difference
        tilt_err_real = pix2ang(y_err);
        # Online debugging print
        #print("tilt error:", tilt_err_real)

        output_cood = "%d" % (avg_l)


        #Calculate angle
        #Send data
        data = ustruct.pack(
                   "fff", # Three integers (each integer occupies four bytes)
                   float(avg_l), # distance in mm
                   float(tilt_err_real), #data 2
                   float(pan_err_real) #data 3
                   )


    else:
        # micropython.mem_info()
        l = -1
        tanangle = -181
        sin = -181
        data = ustruct.pack(
                   "fff", # Three integers (each integer occupies four bytes)
                   float(l), # up sample by 4    #data 1
                   float(tanangle), # up sample by 4    #data 2
                   float(sin) #data 3 (byte sequence of length 1)
                   )

        led.off()
    # time.sleep_ms(70)
