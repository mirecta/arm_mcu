#!/usr/bin/python

import time
import BaseHTTPServer
import urlparse
import socket

DOC_ROOT = 'www/'
LED_PORT = 1200
LED_IP = '192.168.1.252'


HOST_NAME = '' # !!!REMEMBER TO CHANGE THIS!!!
PORT_NUMBER = 8080 # Maybe set this to 9000.


mimeTypes = {'.png':"image/png", 
            '.jpg':"image/jpeg", 
            '.jpeg':"image/jpeg", 
            '.htm':"text/html",
            '.html':"text/html",
            '.js':"application/javascript",
            '.css':"text/css",
            '.txt':"text/plain",
            '.xml':"text/xml"}



class MyHandler(BaseHTTPServer.BaseHTTPRequestHandler):
    def do_HEAD(self):
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()

    def do_GET(self):
        """Respond to a GET request."""
        
        url = urlparse.urlparse(self.path)
        params = urlparse.parse_qs(url.query)

        if url.path.lower().endswith(tuple(mimeTypes.keys())) or url.path == '/':

            self.handleStatic(url,params)
        elif url.path == "/cmd":
            self.handleCmd(url,params)
        else:

            self.send_response(404)
            self.send_header("Content-type", "text/html")
            self.end_headers()
            self.wfile.write("<html><head><title>Title goes here.</title></head>")
            self.wfile.write("<body><p>%s Not found</p></body></html>" % url.path)

        #endif
    #enddef
  
    def handleCmd(self,url,params):
        """
        // 0x00 0x00 0x00 - reset
        // 0x01 0xMM 0x00 - set mode MM (1 - 4)
        // 0x02 0xSS 0x00 - set speed  (0 - 8)
        // 0x03 0xHH 0xSS - set color hue HH sat SS
        // 0x04 0xLL 0x00 - set max lightness

        """

        if "mode" in params:
            data = chr(0x01) + chr(int(params['mode'][0])) + chr(0)
        elif "speed" in params:
            data = chr(0x02) + chr(int(params['speed'][0])) + chr(0)
        elif "reset" in params:
            data = chr(0x00) + chr(0) + chr(0)
        elif "light" in params:
            data = chr(0x04) + chr(int(params['light'][0])) + chr(0)
        elif "hue" in params:
            data = chr(0x03) + chr(int(params['hue'][0])) + chr(int(params['saturation'][0]))
        else:
            return




        sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
        sock.sendto(data,(LED_IP,LED_PORT));


        self.send_response(200)
        self.end_headers()
    #enddef



    def handleStatic(self,url,params):
        """
        """
        try:
            filename = url.path
            if filename == '/':
                filename = "/index.html"
            #endif
            fl = open(DOC_ROOT + filename, 'r')
            
            mime = mimeTypes.get('.' + filename.split('.')[-1],"text/html")

            self.send_response(200)
            self.send_header("Content-type", mime)
            self.end_headers()
            self.wfile.write(fl.read())

        except:
            self.send_response(404)
            self.send_header("Content-type", "text/html")
            self.end_headers()
            self.wfile.write("<html><head><title>Title goes here.</title></head>")
            self.wfile.write("<body><p>%s Not found</p></body></html>" % filename)
            
        #endtry
    #enddef




if __name__ == '__main__':
    server_class = BaseHTTPServer.HTTPServer
    httpd = server_class((HOST_NAME, PORT_NUMBER), MyHandler)
    print time.asctime(), "Server Starts - %s:%s" % (HOST_NAME, PORT_NUMBER)
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()
    print time.asctime(), "Server Stops - %s:%s" % (HOST_NAME, PORT_NUMBER)
