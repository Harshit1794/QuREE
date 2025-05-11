import asyncio
import websockets
import json
import datetime
import os
import threading
import time


orientation_data = {"yaw": 0, "pitch": 0, "timestamp": ""}
data_file_path = "orientation_data.json"

file_lock = threading.Lock()

def save_data_to_file():
    while True:
        try:
            with file_lock:
                with open(data_file_path, 'w') as f:
                    json.dump(orientation_data, f)
        except Exception as e:
            print(f"Error saving data to file: {e}")
        time.sleep(0.1)  

async def handle_connection(websocket):
    print(f"Client connected from {websocket.remote_address}")
    
    try:
        async for message in websocket:
            data = json.loads(message)
            
            data["timestamp"] = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
            global orientation_data
            orientation_data = data
            
            print(f"Received: Yaw={data['yaw']}°, Pitch={data['pitch']}°")
            
            await websocket.send(json.dumps({"status": "received"}))
    except websockets.exceptions.ConnectionClosed:
        print(f"Connection closed with {websocket.remote_address}")
    except Exception as e:
        print(f"Error: {e}")

async def main():
    data_thread = threading.Thread(target=save_data_to_file, daemon=True)
    data_thread.start()
    
    async with websockets.serve(handle_connection, "0.0.0.0", 8765):
        print("WebSocket server started on ws://0.0.0.0:8765")
        print(f"Data will be written to {os.path.abspath(data_file_path)}")
        await asyncio.Future()

if __name__ == "__main__":
    asyncio.run(main())