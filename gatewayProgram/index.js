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

//#region creazione porta seriale
const port = new SerialPort(serialPort, {
  baudRate: 115200
});
//#endregion

//#region collegamento IoT Hub
var client = Client.fromConnectionString(deviceConnectionString, Protocol);
//#endregion

//#region apertura porta seriale
port.open(function (err) {
    if (err) {
        return console.log('Error opening port: ', err.message);
    }
    port.write('COM2 Open');
});

// gli errori di apertura vengono intrecettati da questa funzione
port.on('error', function(err) {
    console.log('Error: ', err.message);
});
//#endregion

//#region connessione con IoT Hub
client.open(function(err) {
    if (err) {
        console.error('could not open IotHub client');
    }  else {
        console.log('client opened');
  
        //#region creazione Device Twin
        client.getTwin(function(err, twin) {
            if (err) {
                console.error('could not get twin');
            } else {
                console.log('twin created');

                //#region ricezione aggiornamenti Device Twin
                twin.on('properties.desired', function(delta) {
                    //#region desired property per uC1
                    if(delta.hasOwnProperty('Micro1'))
                    {
                        //legge il valore attuale della proprietà
                        let Value = delta.Micro1;
                        let report;
                        if(twin.properties.reported.hasOwnProperty('Micro1'))
                        {
                            report = twin.properties.reported.Micro1;
                        }
                        
                        if(Value!= report)
                        {
                            
                            var ByteArray = [0x01];
                            var buffer = new Buffer.from(Value, 'utf8');
                            for (var i = 0; i < buffer.length; i++) {
                                ByteArray.push(buffer[i]);
                            }
                            port.write(ByteArray, function(err) {
                                if (err) {
                                    return console.log('Error on write: ', err.message);
                                }
                            });
                            
                            let patch = {
                                "Micro1":Value
                            }

                            twin.properties.reported.update(patch, function (err) {
                                if (err) throw err;
                                console.log('Properties have been reported for component');
                            });
                        }
                    }
                    //#endregion

                    //#region desired property per uC2
                    if(delta.hasOwnProperty('Micro2'))
                    {
                        let Value = delta.Micro2;
                        let report;
                        if(twin.properties.reported.hasOwnProperty('Micro2'))
                        {
                            report = twin.properties.reported.Micro2;
                        }

                        if(Value!= report)
                        {
                            var ByteArray = [0x02];
                            var buffer = new Buffer.from(Value, 'utf8');
                            for (var i = 0; i < buffer.length; i++) {
                                ByteArray.push(buffer[i]);
                            }
                            port.write(ByteArray, function(err) {
                                if (err) {
                                    return console.log('Error on write: ', err.message);
                                }
                            });
                            
                            let patch = {
                                "Micro2":Value
                            }
                            
                            twin.properties.reported.update(patch, function (err) {
                                if (err) throw err;
                                console.log('Properties have been reported for component');
                            });
                        }
                    }
                    //#endregion

                    //#region desired property per uC3
                    if(delta.hasOwnProperty('Micro3'))
                    {
                        let Value = delta.Micro3;
                        let report;
                        if(twin.properties.reported.hasOwnProperty('Micro3'))
                        {
                            report = twin.properties.reported.Micro3;
                        }

                        if(Value!= report)
                        {
                            var ByteArray = [0x03];
                            var buffer = new Buffer.from(Value, 'utf8');
                            for (var i = 0; i < buffer.length; i++) {
                                ByteArray.push(buffer[i]);
                            }
                            port.write(ByteArray, function(err) {
                                if (err) {
                                    return console.log('Error on write: ', err.message);
                                }
                            });
                            
                            let patch = {
                                "Micro3":Value
                            }
                            
                            twin.properties.reported.update(patch, function (err) {
                                if (err) throw err;
                                console.log('Properties have been reported for component');
                            });
                        }
                    }
                    //#endregion
                });
                //#endregion
            }
        });
        //#endregion
    }
});
//#endregion