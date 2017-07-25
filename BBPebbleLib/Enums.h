/*
 * Enums.h
 *
 *  Created on: 2015-03-16
 *      Author: James
 */

#ifndef ENUMS_H_
#define ENUMS_H_

#include <QtGlobal>

namespace Enums
{

    namespace Client {
        static const quint32 UNKNOWN = 0;
        static const quint32 IOS = 1;
        static const quint32 ANDROID = 2;
        static const quint32 OSX = 3;
        static const quint32 LINUX = 4;
        static const quint32 WINDOWS = 5;

        static const quint32 TELEPHONY = 16;
        static const quint32 SMS = 32;
        static const quint32 GPS = 64;
        static const quint32 BTLE = 128;
        //static const quint32 CAMERA_FRONT = 240; Doesn't make sense as it'd mess up the bitmask, but it's apparently true.
        static const quint32 CAMERA_REAR = 256;
        static const quint32 ACCEL = 512;
        static const quint32 GYRO = 1024;
        static const quint32 COMPASS = 2048;
    }

    namespace Session {
        static const quint32 GAMMA_RAY = 0x80000000;
    }

    namespace MediaControl {
        static const quint8 None = 0;
        static const quint8 PlayPause = 1;
        static const quint8 Pause = 2;
        static const quint8 Play = 3;
        static const quint8 Next = 4;
        static const quint8 Previous = 5;
        static const quint8 VolumeUp = 6;
        static const quint8 VolumeDown = 7;
        static const quint8 GetNowPlaying = 8;
        static const quint8 SendNowPlaying = 9;
    }

    namespace Endpoint {
        static const quint16 Firmware = 1;
        static const quint16 Time = 11;
        static const quint16 FirmwareVersion = 16;
        static const quint16 PhoneVersion = 17;
        static const quint16 SystemMessage = 18;
        static const quint16 MusicControl = 32;
        static const quint16 PhoneControl = 33;
        static const quint16 ApplicationMessage = 48;
        static const quint16 Launcher = 49;
        static const quint16 AppCustomize = 50;
        static const quint16 Logs = 2000;
        static const quint16 Ping = 2001;
        static const quint16 LogDump = 2002;
        static const quint16 Reset = 2003;
        static const quint16 App = 2004;
        static const quint16 Mfg = 2004;
        static const quint16 AppLogs = 2006;
        static const quint16 Notification = 3000;
        static const quint16 Resource = 4000;
        static const quint16 SysReg = 5000;
        static const quint16 FctReg = 5001;
        static const quint16 AppManager = 6000;
        static const quint16 RunKeeper = 7000;
        static const quint16 DataLog = 6778;
        static const quint16 SCREENSHOT = 8000;
        static const quint16 CoreDump = 9000;
        static const quint16 BlobDB = 45531;
        static const quint16 PutBytes = 48879;
        static const quint16 MaxEndpoint = 65535; //quint16 max value
    }

    namespace PhoneControlCommands {
        static const quint8 ANSWER = 1;
        static const quint8 HANGUP = 2;
        static const quint8 GET_STATE = 3;
        static const quint8 INCOMING_CALL = 4;
        static const quint8 OUTGOING_CALL = 5;
        static const quint8 MISSED_CALL = 6;
        static const quint8 RING = 7;
        static const quint8 START = 8;
        static const quint8 END = 9;
    }

    namespace LogLevel { //PORTED FROM C# and TYPE WASN'T DEFINED. ASSUMED TO INT32
        static const quint32 Unknown = -1;
        static const quint32 All = 0;
        static const quint32 Error = 1;
        static const quint32 Warning = 50;
        static const quint32 Information = 100;
        static const quint32 Debug = 200;
        static const quint32 Verbose = 250;
    }

    namespace AppMessage {
        static const quint8 Push = 0x01;
        static const quint8 Request = 0x02;
        static const quint8 Ack = 0xFF;
        static const quint8 Nack = 0x7F;
    }

    namespace NotificationTypes {
        static const quint8 Email = 0;
        static const quint8 SMS = 1;
        static const quint8 Facebook = 2;
        static const quint8 Twitter = 3;
    }
}

#endif /* ENUMS_H_ */
