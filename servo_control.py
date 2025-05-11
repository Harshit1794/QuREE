import json
from time import sleep
import os
from gpiozero import Servo

def read_orientation_data():
    try:
        if os.path.exists("orientation_data.json"):
            with open("orientation_data.json", "r") as f:
                data = json.load(f)
                return data
        else:
            return {"yaw": 0, "pitch": 0, "timestamp": "No data available"}
    except Exception as e:
        print(f"Error reading orientation data: {e}")
        return {"yaw": 0, "pitch": 0, "timestamp": f"Error: {str(e)}"}
        
panServo = Servo(24, min_pulse_width = 0.0005, max_pulse_width = 0.0025)
tiltServo = Servo(17, min_pulse_width = 0.0005, max_pulse_width = 0.0025)


while True:
	data = read_orientation_data()
	if int(data["pitch"]) in range(0,90):
		pitch = 90 - int(data[pitch])
		yaw = int(data["yaw"])
	elif int(data["pitch"]) in range(-90, 0):
		pitch = -1*int(data["pitch"]) - 90
		yaw = int(data["yaw"]) - 180

    if yaw in range(0,180):
        panServo.value = round(((yaw-90)/90), 4)
	    tiltServo.value = round((pitch/90), 4)
	sleep(0.1)