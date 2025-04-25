# This code is based on OpenMV demo code for color tracking
# We used this code to find the color of the target and fill those values into the main code

import sensor
import time

print("Letting auto algorithms run. Don't put anything in front of the camera!")

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)  # must be turned off for color tracking
sensor.set_auto_whitebal(False)  # must be turned off for color tracking
clock = time.clock()

# Define a 50x50 pixel region at the center
r = [(320 // 2) - (50 // 2), (240 // 2) - (50 // 2), 50, 50]

print("Auto algorithms done. Hold the object you want to track in front of the camera in the box.")
print("MAKE SURE THE COLOR OF THE OBJECT YOU WANT TO TRACK IS FULLY ENCLOSED BY THE BOX!")
for i in range(60):
    img = sensor.snapshot()
    img.draw_rectangle(r)

print("Learning thresholds...")
threshold = [50, 50, 0, 0, 0, 0]  # Initial LAB values
for i in range(60):
    img = sensor.snapshot()
    hist = img.get_histogram(roi=r)
    lo = hist.get_percentile(0.01)
    hi = hist.get_percentile(0.99)

    threshold[0] = (threshold[0] + lo.l_value()) // 2
    threshold[1] = (threshold[1] + hi.l_value()) // 2
    threshold[2] = (threshold[2] + lo.a_value()) // 2
    threshold[3] = (threshold[3] + hi.a_value()) // 2
    threshold[4] = (threshold[4] + lo.b_value()) // 2
    threshold[5] = (threshold[5] + hi.b_value()) // 2

    for blob in img.find_blobs([threshold], pixels_threshold=100, area_threshold=100, merge=True, margin=10):
        img.draw_rectangle(blob.rect())
        img.draw_cross(blob.cx(), blob.cy())
        img.draw_rectangle(r)

# Widen the threshold range
L_expand = 10
A_expand = 10
B_expand = 10

threshold[0] = max(0, threshold[0] - L_expand)
threshold[1] = min(100, threshold[1] + L_expand)
threshold[2] = max(-128, threshold[2] - A_expand)
threshold[3] = min(127, threshold[3] + A_expand)
threshold[4] = max(-128, threshold[4] - B_expand)
threshold[5] = min(127, threshold[5] + B_expand)

print("Thresholds learned...")
print("Tracking colors...")

while True:
    clock.tick()
    img = sensor.snapshot()
    found = False
    for blob in img.find_blobs([threshold], pixels_threshold=100, area_threshold=100, merge=True, margin=10):
        img.draw_rectangle(blob.rect())
        img.draw_cross(blob.cx(), blob.cy())
        found = True
    fps = clock.fps()
    print("FPS:", fps, "| LAB Threshold:", threshold if found else "No object detected")
