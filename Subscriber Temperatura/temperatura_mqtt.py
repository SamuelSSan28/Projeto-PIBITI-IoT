from datetime import datetime
import sys
import paho.mqtt.client as mqtt

broker = "ip"
port = "porta"
keppAlive = 60
topic = 'temperatura/sensor_10' 


def on_connect(client, userdata, flags, rc):
    client.subscribe(topic)



def on_message(client, userdata, msg):
    message = str(msg.payload) # converte a mensagem recebida
    try :
        param = message[2:6].split(" ")
        print(param)
    except Exception as e:
        print(e)
        exit(0)

try:
    print("MQTT...")
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(broker)
    client.username_pw_set("mqtt_user", "mqtt_ufpi")
    client.loop_forever()

except Exception as e:
    print ("\nScript finalizado.",e)
    sys.exit(0)
