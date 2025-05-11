import cv2
import socket
import time
import threading
import http.server
import socketserver
import io
import os
import signal
import sys
from PIL import Image


VIDEO_DEVICE = 0  
MJPEG_PORT = 8080  
FRAME_RATE = 30
RESOLUTION = (640, 480)


frame_lock = threading.Lock()
current_frame = None
server_running = True

class MJPEGHandler(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        global current_frame, server_running
        

        if self.path == '/':
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            self.wfile.write(b"""
                <html>
                <head>
                    <title>Raspberry Pi Camera Stream</title>
                    <style>
                        body { margin: 0; background-color: #000; text-align: center; }
                        img { max-width: 100%; height: auto; }
                    </style>
                </head>
                <body>
                    <img src="/stream/video.mjpeg" />
                </body>
                </html>
            """)
            return
            
        elif self.path.startswith('/stream/video.mjpeg'):
            self.send_response(200)
            self.send_header('Content-type', 'multipart/x-mixed-replace; boundary=--jpgboundary')
            self.send_header('Cache-Control', 'no-cache')
            self.end_headers()
            
            try:
                while server_running:
                    if current_frame is None:
                        time.sleep(0.1)
                        continue
                    
                    with frame_lock:
                        encoded_frame = current_frame
                    
                    self.wfile.write(b"--jpgboundary\r\n")
                    self.send_header('Content-type', 'image/jpeg')
                    self.send_header('Content-length', len(encoded_frame))
                    self.end_headers()
                    self.wfile.write(encoded_frame)
                    self.wfile.write(b'\r\n')
                    
                    time.sleep(1.0 / FRAME_RATE)
            except Exception as e:
                print(f"Streaming error: {e}")
            return
        
        else:
            self.send_response(404)
            self.send_header('Content-type', 'text/plain')
            self.end_headers()
            self.wfile.write(b'Not Found')
            return

class ThreadedHTTPServer(socketserver.ThreadingMixIn, http.server.HTTPServer):
    daemon_threads = True

def capture_frames():
    global current_frame, server_running
    
    try:
        cap = cv2.VideoCapture(VIDEO_DEVICE)
        cap.set(cv2.CAP_PROP_FRAME_WIDTH, RESOLUTION[0])
        cap.set(cv2.CAP_PROP_FRAME_HEIGHT, RESOLUTION[1])
        
        if not cap.isOpened():
            print("Error: Could not open camera.")
            server_running = False
            return
        
        print(f"Camera initialized at {RESOLUTION[0]}x{RESOLUTION[1]}")
        
        while server_running:
            ret, frame = cap.read()
            
            if not ret:
                print("Error: Failed to capture frame.")
                time.sleep(0.1)
                continue
            
            with frame_lock:
                _, buffer = cv2.imencode('.jpg', frame, [cv2.IMWRITE_JPEG_QUALITY, 80])
                current_frame = buffer.tobytes()
            
            time.sleep(1.0 / FRAME_RATE)
    
    except Exception as e:
        print(f"Capture error: {e}")
    
    finally:
        if 'cap' in locals() and cap.isOpened():
            cap.release()
        server_running = False

def create_models_directory():
    models_dir = "/var/www/html/models"
    if not os.path.exists(models_dir):
        os.makedirs(models_dir)
    return models_dir

def signal_handler(sig, frame):
    global server_running
    print('Shutting down server...')
    server_running = False
    sys.exit(0)

def main():
    signal.signal(signal.SIGINT, signal_handler)
    
    models_dir = create_models_directory()
    print(f"Models directory: {models_dir}")
    print(f"Please download YOLOv8 ONNX model to: {models_dir}/yolov8n.onnx")
    
    capture_thread = threading.Thread(target=capture_frames)
    capture_thread.daemon = True
    capture_thread.start()
    

    try:
        print(f"Starting MJPEG server on port {MJPEG_PORT}...")
        server = ThreadedHTTPServer(('0.0.0.0', MJPEG_PORT), MJPEGHandler)
        print(f"Server started. Access it at http://[RaspberryPi-IP]:{MJPEG_PORT}/")
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    except Exception as e:
        print(f"Server error: {e}")
    
    global server_running
    server_running = False
    print("Server stopped.")

if __name__ == "__main__":
    main()