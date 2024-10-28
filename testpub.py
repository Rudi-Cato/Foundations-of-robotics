from paho.mqtt import client as mqtt_client
import random
import time
import json  # Using JSON to serialize the Twist message


broker = '' # broker endpoint
port = 8883
topic = "python/mqtt"
client_id = f'python-mqtt-{random.randint(0, 1000)}'
username = ''     #username for broker goes here
password = ''     #Password for broker goes here


def connect_mqtt():
    
    def on_connect(client, userdata, flags, rc, properties):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)
    
    # For paho-mqtt 2.0.0, you need to set callback_api_version.
    client = mqtt_client.Client(client_id=client_id, callback_api_version=mqtt_client.CallbackAPIVersion.VERSION2)
    client.username_pw_set(username, password)

    client.tls_set(ca_certs='') #CA for auth goes here


    # client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client


def publish(client):
    msg_count = 1
    while True:
        time.sleep(1)

        # Create a Twist message
        twist_msg = {
            "linear": {
                "x": 1.0,  # Forward velocity
                "y": 0.0,
                "z": 0.0
            },
            "angular": {
                "x": 0.0,
                "y": 0.0,
                "z": 0.5 * msg_count  # Increment angular velocity
            }
        }

        # Serialize the message to JSON
        msg = json.dumps(twist_msg)
        result = client.publish(topic, msg)

        # Check publish status
        status = result.rc
        if status == mqtt_client.MQTT_ERR_SUCCESS:
            print(f"Send `{msg}` to topic `{topic}`")
        else:
            print(f"Failed to send message to topic {topic}")

        msg_count += 1
        if msg_count > 16:
            break




def run():
    client = connect_mqtt()
    client.loop_start()
    publish(client)
    client.loop_stop()


if __name__ == '__main__':
    run()
