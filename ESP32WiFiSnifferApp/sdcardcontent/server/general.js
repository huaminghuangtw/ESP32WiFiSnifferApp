var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

var demo = undefined; // If true, it displays the demo_device_position on the map.
var demo_device_position = [2.89, 1.92];

var RSSIc = undefined; //dBm
var pathLossExp = undefined;

var masterCoords = [undefined, undefined]; //m

var numberOfSlaves = undefined;
var espSlaveXcoords = []; //m
var espSlaveYcoords = []; //m

var roomContourXCoords = []; //m
var roomContourYCoords = []; //m

var roomMaxLength = undefined; // m
var roomMaxWidth = undefined; // m
var edgeGapSize = 0.05; // portion of canvas height/width



window.addEventListener('load', onLoad);


function onLoad(event) {
    parseConfigJSON();
    parseRoomContourJSON();
    initWebSocket();
    initButtons();
    // Display the Logs tab with measurement data as default:
    // by making an automated click on the tab button.
    document.getElementById("measTableTab").click();

} // onLoad	


function onOpen(event) {
    console.log('Connection opened');

} // onOpen


function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);

} // onClose


function onMessage(event) {
    var state;
    if (event.data == "1") {
        state = "ON";
        document.getElementById('ledState').innerHTML = state;
    } else if (event.data == "0") {
        state = "OFF";
        document.getElementById('ledState').innerHTML = state;
    } else if (event.data == "stream started") {
        console.log("Stream started.");
    } else {
        let data = JSON.parse(event.data);
        appendDataToLogsTable(data);

        console.log(data.time);
        console.log(data.MAC);

        appendPosToPosTable(data);
    }

} // onMessage


function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;

} // initWebSocket


function initButtons() {
    document.getElementById('ledButton').addEventListener('click', toggle);

} // initButtons


function toggle() {
    websocket.send('toggle');

} // toggle


function dataStreamStartOrStop() {
    var elem = document.getElementById("startButton");
    if (elem.value == "Start") {
        elem.value = "Stop";
    } else {
        elem.value = "Start";
    }
    websocket.send('streamData');

} // dataStreamStartOrStop


function loadRoomContourJSON(callback) {
    var xobj = new XMLHttpRequest();
    xobj.open('GET', 'room.json', true);
    xobj.onreadystatechange = function() {
        if (xobj.readyState == 4 && xobj.status == "200") {
            callback(xobj.responseText);
        }
    };
    xobj.send(null);

} // loadRoomContourJSON


function parseRoomContourJSON() {
    var roomContourJson;

    function callbackfunc(response) {

        roomContourJson = JSON.parse(response);

        var roomContourCoords = roomContourJson.room_contour;

        for (var i = 0; i < roomContourCoords.length; i++) {
            roomContourXCoords[i] = roomContourJson.room_contour[i].x;
            roomContourYCoords[i] = roomContourJson.room_contour[i].y;
        }

        console.log(Math.max(...roomContourXCoords));
        roomMaxLength = Math.max(...roomContourXCoords);
        roomMaxWidth = Math.max(...roomContourYCoords);

    }

    loadRoomContourJSON(callbackfunc);

} // parseRoomContourJSON


function loadConfigJSON(callback) {
    var xobj = new XMLHttpRequest();
    xobj.overrideMimeType("application/json");
    xobj.open('GET', 'config.json', true);
    xobj.onreadystatechange = function() {
        if (xobj.readyState == 4 && xobj.status == "200") {
            callback(xobj.responseText);
        }
    };
    xobj.send(null);

} // loadConfigJSON


function parseConfigJSON() {
    var configJson;

    function callbackfunc(response) {

        configJson = JSON.parse(response);

        demo = configJson.demo;

        pathLossExp = configJson.path_loss_exp;
        RSSIc = configJson.RSSI_1m;

        masterCoords[0] = configJson.master_coords.x;
        masterCoords[1] = configJson.master_coords.y;

        numberOfSlaves = configJson.number_of_slaves;

        for (var i = 0; i < numberOfSlaves; i++) {
            espSlaveXcoords[i] = configJson.slave_coords[i].x;
            espSlaveYcoords[i] = configJson.slave_coords[i].y;
        }

    }

    loadConfigJSON(callbackfunc);

} // parseConfigJSON