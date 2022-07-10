//#region require
const SerialPort = require('serialport');
var Protocol = require('azure-iot-device-mqtt').Mqtt;
var Client = require('azure-iot-device').Client;
const dotenv = require('dotenv');
//#endregion

//#region recupero variabili ambientali
dotenv.config();
var deviceConnectionString = process.env.IOTHUB_DEVICE_CONNECTION_STRING;
var serialPort = process.env.SERIAL_PORT;
//#endregion

const port = new SerialPort(serialPort, {
  baudRate: 115200
});

// create the IoTHub client
var client = Client.fromConnectionString(deviceConnectionString, Protocol);

port.open(function (err) {
    if (err) {
        return console.log('Error opening port: ', err.message);
    }
    port.write('COM2 Open');
});

// Open errors will be emitted as an error event
port.on('error', function(err) {
    console.log('Error: ', err.message);
});

// connect to the hub
client.open(function(err) {
    if (err) {
        console.error('could not open IotHub client');
    }  else {
        console.log('client opened');
  
        // Create device Twin
        client.getTwin(function(err, twin) {
            if (err) {
                console.error('could not get twin');
            } else {
                console.log('twin created');
  

                twin.on('properties.desired', function(delta) {
                    console.log('new desired properties received:');
                    console.log(JSON.stringify(delta));
                    port.write('main screen turn on', function(err) {
                        if (err) {
                            return console.log('Error on write: ', err.message);
                        }
                        console.log('message written');
                    });
                });
            }
        });
    }
});