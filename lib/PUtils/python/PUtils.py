import enet
import random
import sys
import time
import threading
import ctypes
import json

from aiohttp import web
import socketio, random
from enum import Enum


# Define constants
NODE_NETWORK_MSG_PAYLOAD_SIZE = 1024  # Replace with the actual size

# Define the struct class
class Message(ctypes.Structure):
    _fields_ = [
        ("mid", ctypes.c_int32),  # int32_t
        ("seq", ctypes.c_uint8),  # uint8_t
        ("dstId", ctypes.c_int32),  # int32_t
        ("srcId", ctypes.c_int32),  # int32_t
        ("rspNeeded", ctypes.c_bool),  # bool
        ("isRspMsg", ctypes.c_bool),  # bool
        ("payload", ctypes.c_char * NODE_NETWORK_MSG_PAYLOAD_SIZE)  # char payload[NODE_NETWORK_MSG_PAYLOAD_SIZE]
    ]

    def __init__(self, mid=0, seq=0, dstId=0, srcId=0, rspNeeded=False, isRspMsg=False, payload=None):
        if payload is None:
            payload = b'\x00' * NODE_NETWORK_MSG_PAYLOAD_SIZE  # Default empty payload
        elif len(payload) > NODE_NETWORK_MSG_PAYLOAD_SIZE:
            raise ValueError(f"Payload cannot exceed {NODE_NETWORK_MSG_PAYLOAD_SIZE} bytes")
        
        self.mid = mid
        self.seq = seq
        self.dstId = dstId
        self.srcId = srcId
        self.rspNeeded = rspNeeded
        self.isRspMsg = isRspMsg
        self.payload = payload

    def to_bytes(self):
        """ Convert the Message structure to bytes representation """
        return bytes(self)

    @classmethod
    def from_bytes(cls, byte_data):
        """ Create a Message structure from byte data """
        if len(byte_data) != ctypes.sizeof(cls):
            raise ValueError(f"Expected byte data of size {ctypes.sizeof(cls)}")
        message = cls()
        ctypes.memmove(ctypes.addressof(message), byte_data, ctypes.sizeof(message))
        return message


class NetworkNode:
    def __init__(self):
        self.channel_ = 0
        self.msgSeq_ = 0
        self.newMsg_ = []
        self.rspMsg_ = []
        self.settings_ = dict()
        self.data_ = dict()

        self.settings_['serverIp'] = 'localhost'
        self.settings_['port'] = 9898
        self.settings_['nodeType'] = 'CLIENT_NODE'
        self.settings_['nodeId'] = 1
        self.settings_['maxNodeIds'] = 250
        self.settings_['keepAliveUntilTimeMs'] = 10
        self.settings_['maxNewMsgSize'] = 10
        self.settings_['maxRspMsgSize'] = 10
        self.settings_['logTransitMsgs'] = True

    def initialize(self):
        self.host_ = enet.Host(None, self.settings_['maxNodeIds'], 1, 1024*1024, 1024*1024)
        # self.peer_ = self.host_.connect(enet.Address(self.settings_['serverIp'], self.settings_['port']), self.settings_['nodeId'])
        self.peer_ = self.host_.connect(enet.Address(self.settings_['serverIp'].encode('utf-8'), self.settings_['port']), 1, data=self.settings_['nodeId'])

    def run(self):
        self.shutdownCalled_ = False
        while False == self.shutdownCalled_:
            event_ = self.host_.service(self.settings_['keepAliveUntilTimeMs'])
            if event_.type == enet.EVENT_TYPE_CONNECT:
                if self.settings_['logTransitMsgs']: print("CNode[%u]:Node connected[%s]:srcId:[%u]" % (self.settings_['nodeId'], event_.peer.address, event_.data))

            elif event_.type == enet.EVENT_TYPE_DISCONNECT:
                if self.settings_['logTransitMsgs']: print("CNode[%u]:Node disconnected[%s]:srcId:[%u]" % (self.settings_['nodeId'], event_.peer.address, event_.data))
                continue

            elif event_.type == enet.EVENT_TYPE_RECEIVE:
                m = Message.from_bytes(event_.packet.data)
                if self.settings_['logTransitMsgs']: print("CNode[%u]:[%u -> %u]:seq[%u]:mid[%d]" % (self.settings_['nodeId'], m.srcId, m.dstId, m.seq, m.mid))
                if 0==m.dstId:
                    pass
                elif m.dstId == self.settings_['nodeId']:
                    if m.isRspMsg:
                        self.rspMsg_.append(m)
                        if len(self.rspMsg_) > self.settings_['maxRspMsgSize']:
                            mDrop = self.rspMsg_.pop(0)
                    else:
                        self.newMsg_.append(m)
                        if len(self.newMsg_) > self.settings_['maxNewMsgSize']:
                            mDrop = self.newMsg_.pop(0)
                continue

    def shutdown(self):
        self.shutdownCalled_ = True

    def disconnect(self):
        self.peer_.disconnect()

    def send(self, msg, updateSeq=True):
        if True==updateSeq:
            self.msgSeq_ = (self.msgSeq_ + 1) % 256
            msg.seq = self.msgSeq_
        msg.srcId = self.settings_['nodeId']
        packet = enet.Packet(msg.to_bytes())
        self.peer_.send(self.channel_, packet)

    def sendQuery(self, msg, timeoutMs):
        msg.rspNeeded = True
        msg.isRspNeeded = False
        self.msgSeq_ = (self.msgSeq_ + 1) % 256
        seq = self.msgSeq_
        msg.seq = self.msgSeq_
        self.send(msg, updateSeq=False)

        # Wait for the response.
        if timeoutMs < 0: timeoutMs = 1
        currMs = 0
        while currMs < timeoutMs:
            for m in self.rspMsg_:
                if m.seq == seq:
                    return m
            time.sleep(0.01)
            currMs += 10
        return None

    def respQuery(self, msg):
        msg.rspNeeded = False
        msg.isRspMsg = True
        self.send(msg, updateSeq=False)

    def recv(self):
        if len(self.newMsg_) > 0:
            m = self.newMsg_.pop(0)
            return m
        return None

class AppSettings:
    def __init__(self):
        pass

    def load(self, file_path):
        """
        Reads a JSON file and returns the data as a Python object (dict or list).
        """
        try:
            with open(file_path, 'r') as file:
                data = json.load(file)
                return data
        except FileNotFoundError:
            print(f"Error: File not found at path: {file_path}")
            return None
        except json.JSONDecodeError:
            print(f"Error: Invalid JSON format in file: {file_path}")
            return None

index_html = '''
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Real-time Chart with Socket.IO</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <script src="https://cdn.socket.io/4.0.0/socket.io.min.js"></script>
    <style>
        canvas {
            width: 100%;
            height: 400px;
        }
    </style>
</head>
<body>
    <h2>Real-time Chart</h2>
    <canvas id="myChart"></canvas>
    <button onClick="sendMsg()">Send Message</button>

    <script>
        const MessageType = {
            UpdateDataLabels: 1,
            GetUpdateData: 2,
            UpdateData: 3
        }

        const ctx = document.getElementById('myChart').getContext('2d');
        data = {
            labels: [], // Time or other data points
            datasets: []
        };

        config = {
            type: 'line',
            data: data,
            options: {
                responsive: true,
                scales: {
                    x: {
                        type: 'linear',
                        position: 'bottom'
                    }
                },
                animation: {
                    duration: 0 // Disable animation for faster updates
                }
            }
        };
        var chart;

        const socket = io.connect('http://' + document.domain + ':' + location.port);
        socket.on('disconnect', () => {
            console.log('server disconnected');
            chart = null;
        });

        socket.on('message', (newData) => {
            if (newData.MessageType === MessageType.UpdateDataLabels) {
                console.log(newData);

                for (let i = 0; i < newData.args.length; i++) {
                    data.datasets.push({ label: newData.args[i], borderColor: 'rgba(75, 192, 192, 1)', data: [], fill: false });
                }
                chart = new Chart(ctx, config);
            }
            else if (newData.MessageType === MessageType.UpdateData && data.datasets.length > 0) {
                console.log(newData);
                data.labels.push(Date.now());

                for (let i = 0; i < newData.args.length; i++) {
                    data.datasets[i].data.push(newData.args[i].toFixed(2));
                }
                // Limit the chart to the last 30 points for better performance
                if (data.labels.length > 30) {
                    data.labels.shift();
                    data.datasets[0].data.shift();
                    data.datasets[1].data.shift();
                }

                // Update the chart
                chart.update();
            }
        });


        function sendMsg() {
          socket.emit("message",
              {
                  "MessageType" : MessageType.GetUpdateData,
                  args : ["zero", "1", "two", "0x11"]
              });
        }
    </script>
</body>
</html>
'''

class MessageType(Enum):
    UpdateDataLabels = 1
    GetUpdateData = 2
    UpdateData = 3


def get_sensor_data():
    temperature = random.uniform(20.0, 30.0)  # Simulating temperature in °C
    humidity = random.uniform(40.0, 60.0)     # Simulating humidity in %
    return {'temperature': temperature, 'humidity': humidity}


class WebserverNode:
    def __init__(self):
        pass

    def initialize(self):
        self.sio = socketio.AsyncServer()
        self.app = web.Application()
        self.sio.attach(self.app)
        self.app.router.add_get('/', self.index)

        self.sio.on('connect', handler=self.connect)
        self.sio.on('message', handler=self.message)
        self.sio.on('disconnect', handler=self.disconnect)

    def run(self):
        web.run_app(self.app)

    async def index(self, request):
        return web.Response(text=index_html, content_type='text/html')

    # @sio.event
    async def connect(self, sid, environ):
        print(f"Client connected: {sid}")
        m = dict()
        m['MessageType'] = MessageType.UpdateDataLabels.value
        m['args'] = ['temperature', 'humidity']
        await self.sio.emit('message', m, room=sid)

    # @sio.event
    async def disconnect(self, sid):
        print(f"Client disconnected: {sid}")

    # @sio.on('message')
    async def message(self, sid, message):
        if (message['MessageType'] == MessageType.GetUpdateData.value):
            data = get_sensor_data()
            m = dict()
            m['MessageType'] = MessageType.UpdateData.value
            m['args'] = [data['temperature'], data['humidity']]
            await self.sio.emit('message', m, room=sid)