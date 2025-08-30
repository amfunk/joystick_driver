import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)

# Set pin 17 as input with a pull-down resistor
GPIO.setup(17, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

try:
    while True:
        if GPIO.input(17) == GPIO.HIGH:
            print("Connection complete (3.3V detected)")
        else:
            print("Connection open (0V / no 3.3V)")
        time.sleep(0.5)

except KeyboardInterrupt:
    GPIO.cleanup()
