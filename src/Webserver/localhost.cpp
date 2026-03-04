#include "localhost.h"
#include "../Hid/hid.h"
#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

const char* ssid = "ESP32_S3_HID";

const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1, user-scalable=0'>
<style>
  body { font-family: Arial; text-align: center; margin: 0; padding: 10px; background-color: #f4f4f9; }
  button { padding: 15px; font-size: 16px; cursor: pointer; border: 1px solid #ccc; border-radius: 8px; background: #fff; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
  button:active { background: #e0e0e0; }
  .grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px; max-width: 350px; margin: 20px auto; }
  #tp { width: 100%; max-width: 350px; height: 300px; background: #e8e8e8; border: 2px solid #bbb; border-radius: 12px; margin: 20px auto; touch-action: none; box-shadow: inset 0 2px 5px rgba(0,0,0,0.1); transition: background 0.2s; }
  .tp-active { background: #d0d0d0 !important; }
  h2 { color: #333; }
</style>
</head>
<body>
<h2>ESP32-S3 HID Interface</h2>
<div class='grid'>
  <button onclick='fetch("/swipe?dir=up")'>Up</button>
  <button onclick='fetch("/swipe?dir=left")'>Left</button>
  <div></div>
  <button onclick='fetch("/swipe?dir=right")'>Right</button>
  <button onclick='fetch("/swipe?dir=down")'>Down</button>
</div>
<div id="tp"></div>
<script>
  let lx=0, ly=0, isD=false, tmr=null, ts=0, hm=false, lpT=null;
  const tp = document.getElementById('tp');
  
  function sendMove(dx, dy) {
    if(!tmr) {
      tmr = setTimeout(() => {
        fetch(`/mousemove?x=${dx}&y=${dy}`);
        tmr = null;
      }, 40);
    }
  }
  
  tp.addEventListener('touchstart', e => {
    e.preventDefault();
    isD = true; hm = false;
    lx = e.touches[0].clientX; ly = e.touches[0].clientY;
    ts = Date.now();
    
    lpT = setTimeout(() => {
      if(!hm) {
        fetch('/longpress');
        tp.classList.add('tp-active');
        setTimeout(() => tp.classList.remove('tp-active'), 300);
      }
    }, 600);
  }, {passive: false});
  
  tp.addEventListener('touchmove', e => {
    e.preventDefault();
    if(!isD) return;
    let cx = e.touches[0].clientX; let cy = e.touches[0].clientY;
    
    if(Math.abs(cx - lx) > 3 || Math.abs(cy - ly) > 3) {
      hm = true;
      clearTimeout(lpT);
    }
    
    if(hm) {
      sendMove(Math.round((cx - lx) * 2), Math.round((cy - ly) * 2));
      lx = cx; ly = cy;
    }
  }, {passive: false});
  
  tp.addEventListener('touchend', e => {
    isD = false;
    clearTimeout(lpT);
    if(!hm && (Date.now() - ts < 250)) fetch('/click');
  });
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
    server.send(200, "text/html", htmlPage);
}

void handleSwipe() {
    if (server.hasArg("dir")) {
        HIDCommand cmd;
        cmd.type = CMD_SWIPE;
        String dir = server.arg("dir");
        
        if (dir == "up") { cmd.x = 0; cmd.y = -120; }
        else if (dir == "down") { cmd.x = 0; cmd.y = 120; }
        else if (dir == "left") { cmd.x = -120; cmd.y = 0; }
        else if (dir == "right") { cmd.x = 120; cmd.y = 0; }
        // else if (dir == "upleft") { cmd.x = -100; cmd.y = -100; }
        // else if (dir == "upright") { cmd.x = 100; cmd.y = -100; }
        // else if (dir == "downleft") { cmd.x = -100; cmd.y = 100; }
        // else if (dir == "downright") { cmd.x = 100; cmd.y = 100; }
        else { cmd.x = 0; cmd.y = 0; }
        
        cmd.duration = 20; 
        xQueueSend(cmdQueue, &cmd, portMAX_DELAY);
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "ERR");
    }
}

void handleLongPress() {
    HIDCommand cmd;
    cmd.type = CMD_LONG_PRESS;
    cmd.button = MOUSE_LEFT;
    cmd.duration = 800;
    xQueueSend(cmdQueue, &cmd, portMAX_DELAY);
    server.send(200, "text/plain", "OK");
}

void handleMouseMove() {
    if (server.hasArg("x") && server.hasArg("y")) {
        HIDCommand cmd;
        cmd.type = CMD_MOUSE_MOVE;
        cmd.x = server.arg("x").toInt();
        cmd.y = server.arg("y").toInt();
        xQueueSend(cmdQueue, &cmd, portMAX_DELAY);
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "ERR");
    }
}

void handleClick() {
    HIDCommand cmd;
    cmd.type = CMD_MOUSE_CLICK;
    cmd.button = MOUSE_LEFT;
    xQueueSend(cmdQueue, &cmd, portMAX_DELAY);
    server.send(200, "text/plain", "OK");
}

void webTask(void *pvParameters) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid);
    server.on("/", handleRoot);
    server.on("/swipe", handleSwipe);
    server.on("/longpress", handleLongPress);
    server.on("/mousemove", handleMouseMove);
    server.on("/click", handleClick);
    server.begin();

    for (;;) {
        server.handleClient();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}