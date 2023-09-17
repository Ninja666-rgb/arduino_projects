#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>

#define rxPin D1 // connect to txpin/2
#define txPin D2 // connect to rxpin/8

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);

const char *ssid = "CodeDrag";
const char chan = 3;

int run = 0;
int ultra;

const char *html = R"===(
<!DOCTYPE html>
<html>
<head>
<style>
.block {
  position: absolute;
  z-index: 1;
  text-align: center;
}

.blockcontent {
  border-radius: 7px;
  padding: 6px;
  cursor: move;
  z-index: 2;
  color: #fff;
  border: solid 1px #000000;
}

.start {
  border-radius: 7px;
  padding: 6px;
  z-index: 2;
  background-color: #ffbf00;
  color: #fff;
  border: solid 1px #000000;
}

.run {
  border-radius: 7px;
  padding: 4px;
  cursor: pointer;
  z-index: 2;
  background-color: #ff2f00;
  color: #fff;
  border: solid 1px #000000;
}

.creating{
  height: 13em;
  background-color: #f1f1f1;
  border: 1px solid #d3d3d3;
}

.input{
  width: 7em;
  border-radius: 7px;
  cursor: pointer;
  border: 3px solid #d3d3d3;
  font-size: 13px;
  padding: 2px;
}
</style>
</head>
<body style="min-height: 100vh;">

<h1>CodeDrag</h1>

<p>Drag the blocks under the start block and create your code and then press run. <br>Ctrl+click or drag blocks into the grey area to <span style="color: #f00">delete them.</span> <br><span style="color: #f00">Note:</span> You can only move <b>one</b> block at a time and you can't join them together.<br><a href="/info">Sensor information</a></p>

<div id="bin" class="creating">
  <div id="for" class="block" style="transform: translate(6px, 6px);">
    <div class="blockcontent" style="background-color: #FF0000;">Go forwards at <input class="input" placeholder="1-10(Integers)" onkeydown="return verify(event);"> speed for <input class="input" placeholder="0-10(Decimals)" onkeydown="return verify2(event);"> seconds.</div>
  </div>
  <div id="bac" class="block" style="transform: translate(6px, 46px);">
    <div class="blockcontent" style="background-color: #FF9C00;">Go backwards at <input class="input" placeholder="1-10(Integers)" onkeydown="return verify(event);"> speed for <input class="input" placeholder="0-10(Decimals)" onkeydown="return verify2(event);"> seconds.</div>
  </div>
  <div id="rig" class="block" style="transform: translate(6px, 86px);">
    <div class="blockcontent" style="background-color: #4DE49C;">Turn right at <input class="input" placeholder="1-10(Integers)" onkeydown="return verify(event);"> speed for <input class="input" placeholder="0-10(Decimals)" onkeydown="return verify2(event);"> seconds.</div>
  </div>
  <div id="lef" class="block" style="transform: translate(6px, 126px);">
    <div class="blockcontent" style="background-color: #3783FF;">Turn left at <input class="input" placeholder="1-10(Integers)" onkeydown="return verify(event);"> speed for <input class="input" placeholder="0-10(Decimals)" onkeydown="return verify2(event);"> seconds.</div>
  </div>
  <div id="forun" class="block" style="transform: translate(6px, 166px);">
    <div class="blockcontent" style="background-color: #B026FF;">Go forwards at <input class="input" placeholder="1-10(Integers)" onkeydown="return verify(event);"> speed until distance to wall is less than <input class="input" placeholder="0-100(Decimals)" onkeydown="return verify3(event);"> cm away.</div>
  </div>
  <div id="start" class="block" style="transform: translate(6px, 228px);">
    <div class="start">Start</div>
  </div>
  <div id="run" class="block" style="transform: translate(106px, 228px);" onclick="compile()">
    <div class="run">Run!</div>
  </div>
</div>

<script>
let shadx = 6;
let box = document.getElementById("start").getBoundingClientRect();
let shady = Number(document.getElementById("start").style.transform.replace("translate(6px, ", "").replace("px)", ""))+box.height;
let blocks = 0;
let stackblocks = 0;

creator(document.getElementById("for"));
creator(document.getElementById("bac"));
creator(document.getElementById("rig"));
creator(document.getElementById("lef"));
creator(document.getElementById("forun"));

window.onscroll = function(ev) {
  if ((window.innerHeight + Math.round(window.scrollY)) >= document.body.offsetHeight) {
    document.body.style.minHeight = Number(document.body.style.minHeight.replace('vh', ''))+30+"vh";
  }
};

function compile(){
  let comp = "";
  let wrong = 0;
  for(let i = 1; i < stackblocks+1; i++){
    let fir = document.getElementsByClassName("block "+i)[0].children[0].children[0].value;
    let sec = document.getElementsByClassName("block "+i)[0].children[0].children[1].value;
    if(fir == ''){
      document.getElementsByClassName("block "+i)[0].children[0].children[0].style.border = "3px dotted #DD00AA";
      wrong = 1;
    }
    if(sec == ''){
      document.getElementsByClassName("block "+i)[0].children[0].children[1].style.border = "3px dotted #DD00AA";
      wrong = 1;
    }
    let type = document.getElementsByClassName("block "+i)[0].children[0].innerText;
    if( type.includes("forw") &&  !(type.includes("until"))){
      comp = comp+"f"+fir+"U"+sec+"U";
    }else if(type.includes("back")){
      comp = comp+"b"+fir+"U"+sec+"U";
    }else if(type.includes("left")){
      comp = comp+"l"+fir+"U"+sec+"U";
    }else if(type.includes("right")){
      comp = comp+"r"+fir+"U"+sec+"U";
    }else if(type.includes("until")){
      comp = comp+"F"+fir+"U"+sec+"U";
    }
    console.log(comp);
  }
  if(wrong == 1){
    alert("Please fill in all the boxes.");
  }else{
    var xmlHttp = new XMLHttpRequest();
    var ur = window.location.origin+"/run";
    xmlHttp.open("GET", ur, true);
    xmlHttp.onload = (e) => {
      if (xmlHttp.readyState === 4) {
        if (xmlHttp.status === 200) {
          if(xmlHttp.responseText === "running"){
            alert("Another code is running.");
          }else{
            var http = new XMLHttpRequest();
            var url = window.location.origin+"/api";
            var params = 'message='+comp;
            http.open('POST', url, true);
            http.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
            http.send(params);
          }
        }
      }
    };
    xmlHttp.send(null);
  }
}

function creator(ate){
create();
  create();
  function create(){
    if(document.getElementsByClassName(("block onpile "+ate.id)).length < 3){
      var clone = ate.cloneNode(true);
      clone.zIndex = 3;
      clone.className = ("block onpile "+ate.id);
      clone.removeAttribute("id");
      ate.after(clone);
      dragElement(clone);
      clone.onmouseup = create;
    }
  }
}

function verify(evt) {
  var charCode = evt.key;
  if(Number(charCode) > -1 && Number(charCode) < 10 || charCode == "Delete" || charCode == "Backspace" || charCode == "ArrowUp" || charCode == "ArrowDown" || charCode == "ArrowLeft" || charCode == "ArrowRight"){
    if(Number(evt.target.value+evt.key) > 10 || Number(evt.target.value+evt.key) < 1){
      return false;
    }
    evt.target.style.border = "3px solid #d3d3d3";
    return true;
  }else{
    return false;
  }
}

function verify2(evt) {
  var charCode = evt.key;
  if(Number(charCode) > -1 && Number(charCode) < 10 || charCode == "Delete" || charCode == "Backspace" || charCode == "." || charCode == "ArrowUp" || charCode == "ArrowDown" || charCode == "ArrowLeft" || charCode == "ArrowRight"){
    if(Number(evt.target.value+evt.key) > 10 || Number(evt.target.value+evt.key) < 0){
      return false;
    }
    evt.target.style.border = "3px solid #d3d3d3";
    return true;
  }else{
    return false;
  }
}

function verify3(evt) {
  var charCode = evt.key;
  if(Number(charCode) > -1 && Number(charCode) < 10 || charCode == "Delete" || charCode == "Backspace" || charCode == "." || charCode == "ArrowUp" || charCode == "ArrowDown" || charCode == "ArrowLeft" || charCode == "ArrowRight"){
    if(Number(evt.target.value+evt.key) > 100 || Number(evt.target.value+evt.key) < 0){
      return false;
    }
    evt.target.style.border = "3px solid #d3d3d3";
    return true;
  }else{
    return false;
  }
}

function dragElement(elmnt) {
  var pos1 = 0, pos2 = 0, pos3 = 0, pos4 = 0;
  var tempstackblocks = stackblocks+1;
  elmnt.onmousedown = dragMouseDown;
  function dragMouseDown(e) {
    if(elmnt.className.includes("block onpile")){
      elmnt.className = "block notstacked";
    }
    blocks++;
    elmnt.style.zIndex = blocks+2;
    e = e || window.event;
    e.preventDefault();
    var element2Rect = elmnt.children[0].children[0].getBoundingClientRect();
    var element2Rect2 = elmnt.children[0].children[1].getBoundingClientRect();
    var isTouching = (
      (e.clientY < element2Rect.top && e.clientY > element2Rect.bottom) ||
      (e.clientX > element2Rect.left && e.clientX < element2Rect.right)
    );
    var isTouching2 = (
      (e.clientY < element2Rect2.top && e.clientY > element2Rect2.bottom) ||
      (e.clientX > element2Rect2.left && e.clientX < element2Rect2.right)
    );
    if(isTouching){
      elmnt.children[0].children[0].select();
      return;
    }
    if(isTouching2){
    elmnt.children[0].children[1].select();
      return;
    }
    if(e.ctrlKey){
      if(elmnt.className != "block notstacked"){
        if(elmnt.className.replace("block ", "") != stackblocks){
          for(let i = stackblocks; i > Number(elmnt.className.replace("block ", "")); i--){
            if(document.getElementsByClassName("block "+(i-1))[0] != null){
              document.getElementsByClassName("block "+i)[0].style.transform = document.getElementsByClassName("block "+(i-1))[0].style.transform;
            }else{
              var ely;
              var reg = /(, )+([-\d])+/g;
              document.getElementsByClassName("block "+i)[0].style.transform.replace(reg, function(m){ ely = Number(m.replace(', ', ''));});
              document.getElementsByClassName("block "+i)[0].style.transform = "translate(6px, "+(ely-document.getElementsByClassName("block "+i)[0].getBoundingClientRect().height)+"px)";
            }
          }
          for(let i = Number(elmnt.className.replace("block ", ""))+1; i < stackblocks+1; i++){
            document.getElementsByClassName("block "+i)[0].className = "block "+(i-1);
          }
        }
        stackblocks--;
        box = elmnt.getBoundingClientRect();
        shady -= box.height;
      }
      elmnt.remove();
      return;
    }
    if(elmnt.className != "block notstacked"){
      if(elmnt.className.replace("block ", "") != stackblocks){
        alert("You can only move the block at the very end. Ctrl+click to delete a block.");
        return;
      }
      stackblocks--;
      box = elmnt.getBoundingClientRect();
      shady -= box.height;
    }
    pos3 = e.clientX;
    pos4 = e.clientY;
    document.onmouseup = closeDragElement;
    document.onmousemove = elementDrag;
  }

  function elementDrag(e) {
    e = e || window.event;
    e.preventDefault();
    pos1 = pos3 - e.clientX;
    pos2 = pos4 - e.clientY;
    pos3 = e.clientX;
    pos4 = e.clientY;
    elmnt.style.zIndex = blocks+2;
    var elx, ely;
    var reg = /(, )+([-\d])+/g;
    elmnt.style.transform.replace(reg, function(m){ ely = Number(m.replace(', ', ''));});
    reg = /([(])+([-\d])+/g;
    elmnt.style.transform.replace(reg, function(m){ elx = Number(m.replace('(', ''));});
    elmnt.style.transform = "translate("+(elx - pos1)+"px, "+(ely - pos2) + "px)";
    if(Math.abs((ely - pos2)-shady) < 20 && Math.abs((elx - pos1)-shadx) < 50){
      if(document.getElementById("shadow") == null){
        var clone = elmnt.cloneNode(true);
        clone.id = "shadow";
        clone.style.transform = "translate("+shadx+"px, "+shady+"px)";
        clone.style.filter = "blur(3px) grayscale(100%)";
        clone.style.zIndex = -1;
        elmnt.after(clone);
      }
    }else{
      if(document.getElementById("shadow") != null){
        document.getElementById("shadow").remove();
      }
    }
  }

  function closeDragElement() {
    const element1Rect = elmnt.getBoundingClientRect();
    const element2Rect = document.getElementById("bin").getBoundingClientRect();

    const isTouching = !(
      element1Rect.bottom < element2Rect.top ||
      element1Rect.top > element2Rect.bottom ||
      element1Rect.right < element2Rect.left ||
      element1Rect.left > element2Rect.right
    );
    if(isTouching){
      elmnt.remove();
      if(document.getElementById("shadow") != null){
        document.getElementById("shadow").remove();
      }
    }

    if(document.getElementById("shadow") != null){
      stackblocks++;
      document.getElementById("shadow").remove();
      elmnt.style.transform = "translate("+shadx+"px, "+shady+"px)";
      elmnt.className = "block "+stackblocks;
      box = elmnt.getBoundingClientRect();
      shady = shady+box.height;
    }else if(elmnt.className != "block notstacked"){
      elmnt.className = "block notstacked";
    }
    document.onmouseup = null;
    document.onmousemove = null;
  }
}
</script>
</body>
</html>
)===";

const char *html2 = R"===(
<!DOCTYPE html>
<html>
<body style="min-height: 100vh;">

<h1>Sensors</h1>

<p>Ultrasonic sensor: <span id="read"></span>cm</p>
<script>
setInterval(function(){
  var xmlHttp = new XMLHttpRequest();
  var ur = window.location.origin+"/ultra";
  xmlHttp.open("GET", ur, true);
  xmlHttp.onload = (e) => {
    if (xmlHttp.readyState === 4) {
      if (xmlHttp.status === 200) {
        document.getElementById("read").innerText = xmlHttp.responseText;
      }
    }
  };
  xmlHttp.send(null);
}, 100);
</script>
</body>
</html>
)===";

void defaultPage() {
  webServer.send(200, "text/html", html);
}

void api() {
  if(webServer.hasArg("message")){
    String instruct = webServer.arg("message");
    mySerial.println(instruct);
  }
  webServer.send(200, "text/plain", "ok");
}

void running(){
  if(run == 1){
    webServer.send(200, "text/plain", "running");
  }else{
    webServer.send(200, "text/plain", "idle");
  }
}

void ultread(){
  webServer.send(200, "text/plain", String(ultra));
}

void info(){
  webServer.send(200, "text/html", html2);
}

void setup() {
  Serial.begin(115200);
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  mySerial.begin(4800);
  delay(500);
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid);
  dnsServer.start(DNS_PORT, "*", apIP);
  webServer.on("/api", api);
  webServer.on("/run", running);
  webServer.on("/ultra", ultread);
  webServer.on("/info", info);
  webServer.onNotFound(defaultPage);
  webServer.begin();
}

void loop() {
  if(mySerial.available()){
    String data = mySerial.readStringUntil('\n');
    data.remove(data.length()-1);
    if(data.equals("Idle")){
      run = 0;
    }else if (data.equals("Running")){
      run = 1;
    }else if (data.indexOf("INFO") > -1){
      data.remove(0, 4);
      ultra = atoi(data.c_str());
    }
  }
  if (WiFi.softAPgetStationNum() != 0){
    dnsServer.processNextRequest();
    webServer.handleClient();
  }
}
