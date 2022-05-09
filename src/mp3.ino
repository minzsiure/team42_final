void mp3_setup()
{
    mySoftwareSerial.begin(9600, SERIAL_8N1, 18, 17); // speed, type, RX, TX
    Serial.println();
    Serial.println(F("DFRobot DFPlayer Mini Demo"));
    Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
    delay(1000);
    if (!myDFPlayer.begin(mySoftwareSerial))
    { // Use softwareSerial to communicate with mp3.

        Serial.println(myDFPlayer.readType(), HEX);
        Serial.println(F("Unable to begin:"));
        Serial.println(F("1.Please recheck the connection!"));
        Serial.println(F("2.Please insert the SD card!"));
        while (true)
            ;
    }
    Serial.println(F("DFPlayer Mini online."));

    myDFPlayer.setTimeOut(500); // Set serial communictaion time out 500ms

    myDFPlayer.volume(25);   // Set volume value (0~30).
    myDFPlayer.volumeUp();   // Volume Up
    myDFPlayer.volumeDown(); // Volume Down
    myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
    myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);

    Serial.println(F("readState--------------------"));
    Serial.println(myDFPlayer.readState()); // read mp3 state
    Serial.println(F("readVolume--------------------"));
    Serial.println(myDFPlayer.readVolume()); // read current volume
    Serial.println(F("readFileCounts--------------------"));
    Serial.println(myDFPlayer.readFileCounts()); // read all file counts in SD card
    Serial.println(F("--------------------"));

    // myDFPlayer.play(1);
    // delay(20000);
}