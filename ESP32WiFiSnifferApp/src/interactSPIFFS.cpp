#if defined(Slave_3) || defined(Slave_4)


/* Header file belonging to this implementation */
#include "interactSPIFFS.hpp"

/* External heaader files */
//...

/* Project header files */
#include "interactOLED.hpp"


void initializeSIPFFS()
{
    // clean FS, for testing purpose
    //SPIFFS.format();

    Serial.println();
    Serial.println( "Mounting SPIFFS..." );

    if ( SPIFFS.begin() )
    {
        Serial.println( "Successfully mount file system!" );
        OLEDdisplayForSPIFFSInit();
    }
    else
    {
        Serial.println( "ERROR - failed to mount SPIFFS!" );
        for(;;);   // if failed, do nothing.
    }

} // initializeSIPFFS


void listDir(fs::FS &fs, const char * dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if (!root) {
        Serial.println("- failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file) {
        if (file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }

} // listDir


void readFile(fs::FS &fs, const char * path)
{
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }

    file.close();

} // readFile


void writeFile(fs::FS &fs, const char * path, const char * message)
{
    Serial.printf( "Writing file: %s\n", path );

    File file = fs.open(path, FILE_WRITE);
    if(!file) {
        Serial.println( "Failed to open file for writing." );
        return;
    }
    if(file.print(message))
    {
        Serial.println( "File written." );
    }
    else
    {
        Serial.println( "Write failed." );
    }

    file.close();

} // writeFile


void appendFile(fs::FS &fs, const char * path, const char * message)
{
    // Serial.printf("Appending to file: %s\n", path);
    File file = fs.open(path, FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file for appending.");
        return;
    }

    if (file.print(message)) {
    //Serial.println("Message appended");
    } else {
        Serial.println("Append failed.");
    }

    file.close();

} // appendFile


void logSPISSF(std::string dataMessage, const char* fileToLogTo)
{
    appendFile( SPIFFS, fileToLogTo, dataMessage.c_str() );

} // logSPISSF


void logSPIFFSFileHeader( std::string headerText )
{
    writeFile( SPIFFS, "/WiFiDeviceData.txt", headerText.c_str() );

} // logSPIFFSFileHeader


void renameFile(fs::FS &fs, const char * path1, const char * path2)
{
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2))
    {
        Serial.println("- file renamed");
    }
    else
    {
        Serial.println("- rename failed");
    }

} // renameFile


void deleteFile(fs::FS &fs, const char * path)
{
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path))
    {
        Serial.println("- file deleted");
    }
    else
    {
        Serial.println("- delete failed");
    }

} // deleteFile


void testFileIO(fs::FS &fs, const char * path)
{
    Serial.printf("Testing file I/O with %s\r\n", path);

    static uint8_t buf[512];
    size_t len = 0;
    File file = fs.open(path, FILE_WRITE);
    if(!file)
    {
        Serial.println("- failed to open file for writing");
        return;
    }

    size_t i;
    Serial.print("- writing" );
    uint32_t start = millis();
    for (size_t i = 0; i < 2048; i++)
    {
        if ((i & 0x001F) == 0x001F)
        {
            Serial.print(".");
        }
        file.write(buf, 512);
    }
    Serial.println("");
    uint32_t end = millis() - start;
    Serial.printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);
    file.close();

    file = fs.open(path);
    start = millis();
    end = start;
    i = 0;
    if (file && !file.isDirectory())
    {
        len = file.size();
        size_t flen = len;
        start = millis();
        Serial.print("- reading" );
        while(len)
        {
            size_t toRead = len;
            if(toRead > 512)
            {
                toRead = 512;
            }
            file.read(buf, toRead);
            if ((i++ & 0x001F) == 0x001F)
            {
                Serial.print(".");
            }
            len -= toRead;
        }
        Serial.println("");
        end = millis() - start;
        Serial.printf("- %u bytes read in %u ms\r\n", flen, end);
        file.close();
    }
    else
    {
        Serial.println("- failed to open file for reading");
    }

} // testFileIO


#endif