import paho.mqtt.client as mqtt
import time
import serial

# MQTT Stuff
mqtt_broker = "127.0.0.1"
# mqtt_user = "MQTTUSER"
# mqtt_pass = "MQTTPASS"
broker_port = 1883

# Serial stuff
PORT = "/dev/cu.usbmodem14301"
BAUDRATE = 115200

ser = serial.Serial(PORT, BAUDRATE)

def on_connect(client, userdata, flags, rc):
    print(f"Connected With Result Code: {rc}")

def on_message_buzzer(client, userdata, message):
    temp_message = message.payload.decode() + "\n"
    ser.write(temp_message.encode())

def on_log(client, obj, level, string):
    print(string)

def read_ldr():
    ldr_reading = str(ser.readline().replace("\n", ""))
    return ldr_reading

client = mqtt.Client(clean_session = True)
client.on_connect = on_connect
client.on_message = on_message_buzzer
client.on_log = on_log
# client.username_pw_set(username = mqtt_user, password = mqtt_pass)
client.connect(mqtt_broker, broker_port)

# Subscribe to your topic here
client.subscribe("uno/buzzer", qos = 1)
client.message_callback_add("uno/buzzer", on_message_buzzer)

# Start looping (non-blocking)
client.loop_start()

while True:
    # Read data here
    ldr_reading = read_ldr()
    # Publish data here
    client.publish(topic = "uno/light", payload = ldr_reading, qos = 1, retain = False)
    #if ldr_reading < 400 :
    #    client.publish(topic = "esp32/led", payload = "1", qos = 1, retain = False)
    #else:
    #    client.publish(topic = "esp32/led", payload = "0", qos = 1, retain = False)
    time.sleep(5)
