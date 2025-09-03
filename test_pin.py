import RPi.GPIO as GPIO
import time

PIN = 27

GPIO.setmode(GPIO.BCM)

GPIO.setup(PIN, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

try:
    while True:
        if GPIO.input(PIN) == GPIO.HIGH:
            print("Connection complete (3.3V detected)")
        else:
            print("Connection open (0V / no 3.3V)")
        time.sleep(0.5)

except KeyboardInterrupt:
    GPIO.cleanup()
