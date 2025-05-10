import dataclasses
import os
import sqlite3

from typing import Any


@dataclasses.dataclass
class FrameTimeData:
    platform_name: str
    number_of_frames: int
    frame_times: dict[str, list[list[float]]]
    unit: str


@dataclasses.dataclass
class FramesPerSecondData:
    platform_name: str
    test_names: list[str]
    average_frames_per_second: dict[str, list[float]]


try:
    platform_name: str = "all"
    database_file_path: str = "sqlite.db"

    if os.path.isfile(database_file_path):
        os.remove(database_file_path)

    sqlite_connection: sqlite3.Connection = sqlite3.connect(database=database_file_path)
    sqlite_cursor: sqlite3.Cursor = sqlite_connection.cursor()

    sqlite_cursor.execute(
        """
        CREATE TABLE IF NOT EXISTS FrameTimes
        (
            Id                      INTEGER PRIMARY KEY NOT NULL,
            Platform                TEXT                NOT NULL,
            YoloName                TEXT                NOT NULL,
            ObjectDetectorType      INTEGER             NOT NULL,
            ObjectDetectorBackEnd   INTEGER             NOT NULL,
            ObjectDetectorBlobSize  INTEGER             NOT NULL,
            Repetition              INTEGER             NOT NULL,
            FrameNumber             INTEGER             NOT NULL,
            FrameTime               INTEGER             NOT NULL,
            TimeUnit                TEXT                NOT NULL
        );
        """
    )

    sqlite_cursor.execute(
        """
        INSERT INTO
        FrameTimes(Platform, YoloName, ObjectDetectorType, ObjectDetectorBackEnd, ObjectDetectorBlobSize, Repetition, FrameNumber, FrameTime, TimeUnit)
        VALUES
            ("Windows Desktop", "YOLOv7", 0                 , 0                   , 0                    , 0        , 1         , 10      , "ms"),
            ("Windows Desktop", "YOLOv7", 0                 , 0                   , 0                    , 0        , 2         , 20      , "ms"),
            ("Windows Desktop", "YOLOv7", 0                 , 0                   , 0                    , 1        , 1         , 30      , "ms"),
            ("Windows Desktop", "YOLOv7", 0                 , 0                   , 0                    , 1        , 2         , 40      , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 1                   , 288                  , 0        , 1         , 210     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 1                   , 288                  , 0        , 2         , 220     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 1                   , 288                  , 1        , 1         , 225     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 1                   , 288                  , 1        , 2         , 225     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 2                   , 288                  , 0        , 1         , 245     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 2                   , 288                  , 0        , 2         , 254     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 2                   , 288                  , 1        , 1         , 267     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 2                   , 288                  , 1        , 2         , 243     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 1                   , 288                  , 0        , 1         , 276     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 1                   , 288                  , 0        , 2         , 278     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 1                   , 288                  , 1        , 1         , 256     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 1                   , 288                  , 1        , 2         , 289     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 2                   , 288                  , 0        , 1         , 288     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 2                   , 288                  , 0        , 2         , 297     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 2                   , 288                  , 1        , 1         , 298     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 2                   , 288                  , 1        , 2         , 283     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 1                   , 320                  , 0        , 1         , 310     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 1                   , 320                  , 0        , 2         , 320     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 1                   , 320                  , 1        , 1         , 325     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 1                   , 320                  , 1        , 2         , 325     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 2                   , 320                  , 0        , 1         , 345     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 2                   , 320                  , 0        , 2         , 354     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 2                   , 320                  , 1        , 1         , 367     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 2                   , 320                  , 1        , 2         , 343     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 1                   , 320                  , 0        , 1         , 376     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 1                   , 320                  , 0        , 2         , 378     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 1                   , 320                  , 1        , 1         , 356     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 1                   , 320                  , 1        , 2         , 389     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 2                   , 320                  , 0        , 1         , 388     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 2                   , 320                  , 0        , 2         , 397     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 2                   , 320                  , 1        , 1         , 398     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 2                   , 320                  , 1        , 2         , 383     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 1                   , 416                  , 0        , 1         , 410     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 1                   , 416                  , 0        , 2         , 420     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 1                   , 416                  , 1        , 1         , 425     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 1                   , 416                  , 1        , 2         , 425     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 2                   , 416                  , 0        , 1         , 445     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 2                   , 416                  , 0        , 2         , 454     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 2                   , 416                  , 1        , 1         , 467     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 2                   , 416                  , 1        , 2         , 443     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 1                   , 416                  , 0        , 1         , 476     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 1                   , 416                  , 0        , 2         , 478     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 1                   , 416                  , 1        , 1         , 456     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 1                   , 416                  , 1        , 2         , 489     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 2                   , 416                  , 0        , 1         , 488     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 2                   , 416                  , 0        , 2         , 497     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 2                   , 416                  , 1        , 1         , 498     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 2                   , 416                  , 1        , 2         , 483     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 1                   , 512                  , 0        , 1         , 510     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 1                   , 512                  , 0        , 2         , 520     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 1                   , 512                  , 1        , 1         , 525     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 1                   , 512                  , 1        , 2         , 525     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 2                   , 512                  , 0        , 1         , 545     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 2                   , 512                  , 0        , 2         , 554     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 2                   , 512                  , 1        , 1         , 567     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 2                   , 512                  , 1        , 2         , 543     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 1                   , 512                  , 0        , 1         , 576     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 1                   , 512                  , 0        , 2         , 578     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 1                   , 512                  , 1        , 1         , 556     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 1                   , 512                  , 1        , 2         , 589     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 2                   , 512                  , 0        , 1         , 588     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 2                   , 512                  , 0        , 2         , 597     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 2                   , 512                  , 1        , 1         , 598     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 2                   , 512                  , 1        , 2         , 583     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 1                   , 608                  , 0        , 1         , 610     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 1                   , 608                  , 0        , 2         , 620     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 1                   , 608                  , 1        , 1         , 625     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 1                   , 608                  , 1        , 2         , 625     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 2                   , 608                  , 0        , 1         , 645     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 2                   , 608                  , 0        , 2         , 654     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 2                   , 608                  , 1        , 1         , 667     , "ms"),
            ("Windows Desktop", "YOLOv7", 1                 , 2                   , 608                  , 1        , 2         , 643     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 1                   , 608                  , 0        , 1         , 676     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 1                   , 608                  , 0        , 2         , 678     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 1                   , 608                  , 1        , 1         , 656     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 1                   , 608                  , 1        , 2         , 689     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 2                   , 608                  , 0        , 1         , 688     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 2                   , 608                  , 0        , 2         , 697     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 2                   , 608                  , 1        , 1         , 698     , "ms"),
            ("Windows Desktop", "YOLOv7", 2                 , 2                   , 608                  , 1        , 2         , 683     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 0                 , 0                   , 0                    , 0        , 1         , 110      , "ms"),
            ("Jetson Nano"    , "YOLOv7", 0                 , 0                   , 0                    , 0        , 2         , 120      , "ms"),
            ("Jetson Nano"    , "YOLOv7", 0                 , 0                   , 0                    , 1        , 1         , 130      , "ms"),
            ("Jetson Nano"    , "YOLOv7", 0                 , 0                   , 0                    , 1        , 2         , 140      , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 1                   , 288                  , 0        , 1         , 1210     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 1                   , 288                  , 0        , 2         , 1220     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 1                   , 288                  , 1        , 1         , 1225     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 1                   , 288                  , 1        , 2         , 1225     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 2                   , 288                  , 0        , 1         , 1245     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 2                   , 288                  , 0        , 2         , 1254     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 2                   , 288                  , 1        , 1         , 1267     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 2                   , 288                  , 1        , 2         , 1243     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 1                   , 288                  , 0        , 1         , 1276     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 1                   , 288                  , 0        , 2         , 1278     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 1                   , 288                  , 1        , 1         , 1256     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 1                   , 288                  , 1        , 2         , 1289     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 2                   , 288                  , 0        , 1         , 1288     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 2                   , 288                  , 0        , 2         , 1297     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 2                   , 288                  , 1        , 1         , 1298     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 2                   , 288                  , 1        , 2         , 1283     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 1                   , 320                  , 0        , 1         , 1310     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 1                   , 320                  , 0        , 2         , 1320     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 1                   , 320                  , 1        , 1         , 1325     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 1                   , 320                  , 1        , 2         , 1325     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 2                   , 320                  , 0        , 1         , 1345     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 2                   , 320                  , 0        , 2         , 1354     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 2                   , 320                  , 1        , 1         , 1367     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 2                   , 320                  , 1        , 2         , 1343     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 1                   , 320                  , 0        , 1         , 1376     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 1                   , 320                  , 0        , 2         , 1378     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 1                   , 320                  , 1        , 1         , 1356     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 1                   , 320                  , 1        , 2         , 1389     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 2                   , 320                  , 0        , 1         , 1388     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 2                   , 320                  , 0        , 2         , 1397     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 2                   , 320                  , 1        , 1         , 1398     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 2                   , 320                  , 1        , 2         , 1383     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 1                   , 416                  , 0        , 1         , 1410     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 1                   , 416                  , 0        , 2         , 1420     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 1                   , 416                  , 1        , 1         , 1425     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 1                   , 416                  , 1        , 2         , 1425     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 2                   , 416                  , 0        , 1         , 1445     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 2                   , 416                  , 0        , 2         , 1454     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 2                   , 416                  , 1        , 1         , 1467     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 2                   , 416                  , 1        , 2         , 1443     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 1                   , 416                  , 0        , 1         , 1476     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 1                   , 416                  , 0        , 2         , 1478     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 1                   , 416                  , 1        , 1         , 1456     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 1                   , 416                  , 1        , 2         , 1489     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 2                   , 416                  , 0        , 1         , 1488     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 2                   , 416                  , 0        , 2         , 1497     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 2                   , 416                  , 1        , 1         , 1498     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 2                   , 416                  , 1        , 2         , 1483     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 1                   , 512                  , 0        , 1         , 1510     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 1                   , 512                  , 0        , 2         , 1520     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 1                   , 512                  , 1        , 1         , 1525     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 1                   , 512                  , 1        , 2         , 1525     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 2                   , 512                  , 0        , 1         , 1545     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 2                   , 512                  , 0        , 2         , 1554     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 2                   , 512                  , 1        , 1         , 1567     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 2                   , 512                  , 1        , 2         , 1543     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 1                   , 512                  , 0        , 1         , 1576     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 1                   , 512                  , 0        , 2         , 1578     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 1                   , 512                  , 1        , 1         , 1556     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 1                   , 512                  , 1        , 2         , 1589     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 2                   , 512                  , 0        , 1         , 1588     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 2                   , 512                  , 0        , 2         , 1597     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 2                   , 512                  , 1        , 1         , 1598     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 2                   , 512                  , 1        , 2         , 1583     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 1                   , 608                  , 0        , 1         , 1610     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 1                   , 608                  , 0        , 2         , 1620     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 1                   , 608                  , 1        , 1         , 1625     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 1                   , 608                  , 1        , 2         , 1625     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 2                   , 608                  , 0        , 1         , 1645     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 2                   , 608                  , 0        , 2         , 1654     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 2                   , 608                  , 1        , 1         , 1667     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 1                 , 2                   , 608                  , 1        , 2         , 1643     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 1                   , 608                  , 0        , 1         , 1676     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 1                   , 608                  , 0        , 2         , 1678     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 1                   , 608                  , 1        , 1         , 1656     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 1                   , 608                  , 1        , 2         , 1689     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 2                   , 608                  , 0        , 1         , 1688     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 2                   , 608                  , 0        , 2         , 1697     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 2                   , 608                  , 1        , 1         , 1698     , "ms"),
            ("Jetson Nano"    , "YOLOv7", 2                 , 2                   , 608                  , 1        , 2         , 1683     , "ms")
        """
    )
    sqlite_connection.commit()

    # Error checking logic
    error_message: str = ""

    # Confirm only a single YOLO name exists across tests
    sqlite_cursor.execute(
        """
        SELECT DISTINCT
            YoloName
        FROM
            FrameTimes
        """
    )
    if len(sqlite_cursor.fetchall()) > 1:
        error_message += "\nExpected identical YOLO names across all tests but found different YOLO names!"

    # Confirm only a single time unit exists across tests
    sqlite_cursor.execute(
        """
        SELECT DISTINCT
            TimeUnit
        FROM
            FrameTimes
        """
    )
    if len(sqlite_cursor.fetchall()) > 1:
        error_message += "\nExpected identical time units across all tests but found different time units!"

    # Confirm frame counts match across all tests
    sqlite_cursor.execute(
        """
        SELECT DISTINCT
            COUNT(*)
        FROM
            FrameTimes
        GROUP BY
            Platform,
            ObjectDetectorType,
            ObjectDetectorBackEnd,
            ObjectDetectorBlobSize,
            Repetition
        """
    )
    if len(sqlite_cursor.fetchall()) > 1:
        error_message += "\nExpected identical frame counts across all tests but found different frame counts!"

    # Confirm tests match across all platforms
    sqlite_cursor.execute(
        """
        WITH Cte AS
        (
            SELECT DISTINCT
                Platform,
                ObjectDetectorType,
                ObjectDetectorBackEnd,
                ObjectDetectorBlobSize
            FROM
                FrameTimes
        )
        SELECT DISTINCT COUNT(*) FROM Cte GROUP BY Platform
        """
    )
    if len(sqlite_cursor.fetchall()) > 1:
        error_message += "\nExpected identical tests across all platform but found different tests!"

    # Confirm enums are between the expected range
    sqlite_cursor.execute(
        """
        SELECT
            *
        FROM
            FrameTimes
        WHERE
            ObjectDetectorType NOT IN (0, 1, 2)
        """
    )
    if len(sqlite_cursor.fetchall()) > 0:
        error_message += "\nExpected a values between 0, 1, and 2 for ObjectDetectorType but got other values!"

    sqlite_cursor.execute(
        """
        SELECT
            *
        FROM
            FrameTimes
        WHERE
            ObjectDetectorBackEnd NOT IN (0, 1, 2)
        """
    )
    if len(sqlite_cursor.fetchall()) > 0:
        error_message += "\nExpected a values between 0, 1, and 2 for ObjectDetectorBackEnd but got other values!"

    sqlite_cursor.execute(
        """
        SELECT
            *
        FROM
            FrameTimes
        WHERE
            ObjectDetectorBlobSize NOT IN (0, 288, 320, 416, 512, 608)
        """
    )
    if len(sqlite_cursor.fetchall()) > 0:
        error_message += (
            "\nExpected a values between 0, 288, 320, 416, 512, and 608 for ObjectDetectorBlobSize but got other values!"
        )

    # Confirm ObjectDetectorBackEnd and ObjectDetectorBlobSize are non-zero if ObjectDetectorType is non-zero
    sqlite_cursor.execute(
        """
        SELECT
            *
        FROM
            FrameTimes
        WHERE
            ObjectDetectorType = 0
        AND (ObjectDetectorBackEnd != 0 OR ObjectDetectorBlobSize != 0)
        """
    )
    if len(sqlite_cursor.fetchall()) > 0:
        error_message += "\nExpected ObjectDetectorBackEnd and ObjectDetectorBlobSize to be zero when ObjectDetectorType is zero but found them to be non-zero!"

    # Confirm ObjectDetectorBackEnd and ObjectDetectorBlobSize are zero if ObjectDetectorType is zero
    sqlite_cursor.execute(
        """
        SELECT
            *
        FROM
            FrameTimes
        WHERE
            ObjectDetectorType != 0
        AND (ObjectDetectorBackEnd = 0 OR ObjectDetectorBlobSize = 0)
        """
    )
    if len(sqlite_cursor.fetchall()) > 0:
        error_message += "\nExpected ObjectDetectorBackEnd and ObjectDetectorBlobSize to be non-zero when ObjectDetectorType is non-zero but found them to be zero!"

    if platform_name != "all":
        # Confirm the current platform is found within the data
        sqlite_cursor.execute(
            f"""
            SELECT
                *
            FROM
                FrameTimes
            WHERE
                Platform = '{platform_name}'
            """
        )
        if len(sqlite_cursor.fetchall()) > 0:
            error_message += f"\nExpected the platform '{platform_name}' to be found in the database but it was not found!"

    if error_message:
        raise Exception("The following violations have been found in the data:" + error_message)

    # Get data logic
    platform_display_name: str = "All Platforms" if platform_name == "all" else platform_name

    sqlite_cursor.execute(
        """
        SELECT DISTINCT
            COUNT(*)
        FROM
            FrameTimes
        GROUP BY
            Platform,
            ObjectDetectorType,
            ObjectDetectorBackEnd,
            ObjectDetectorBlobSize,
            Repetition
        """
    )
    number_of_frames: int = int(sqlite_cursor.fetchall()[0][0])

    frame_times: dict[str, list[list[float]]] = {}
    # need to think above averages for each test
    # # TODO 1: Get all platforms
    # # Get all tests (platform-agnostic)
    # # For each platform
    #     # For each test
    #         # Get all frame times for test and average it
    #         # Append the averaged frame time list to the list within the dictionary key for current platform

    sqlite_cursor.execute(
        """
        SELECT DISTINCT
            TimeUnit
        FROM
            FrameTimes
        """
    )
    unit: str = sqlite_cursor.fetchall()[0][0]

    sqlite_cursor.execute(
        """
        SELECT DISTINCT
            YoloName
        FROM
            FrameTimes
        """
    )
    yolo_name: str = sqlite_cursor.fetchall()[0][0]

    sqlite_cursor.execute(
        """
        SELECT DISTINCT
            ObjectDetectorType,
            ObjectDetectorBlobSize
        FROM
            FrameTimes
        ORDER BY
            ObjectDetectorType     ASC,
            ObjectDetectorBlobSize ASC
        """
    )
    test_names: list[str] = []
    for row in sqlite_cursor.fetchall():
        test_name: str = ""

        if row[0] == 0:
            test_names.append(f"No {yolo_name}")
            continue
        elif row[0] == 1:
            test_name += f"{yolo_name}-tiny "
        else:  # 2
            test_name += f"{yolo_name} "

        test_name += str(row[1])
        test_names.append(test_name)

    # average_frames_per_second: dict[str, list[float]] =
    # *****just use frame time data******
    # # TODO 2: if all platforms:
    #     # Get all platforms
    #     # Get all tests (platform-agnostic and without backend preference)
    #         # Order them based on ObjectDetectorType and ObjectDetectorBlobSize
    #     # For each platform
    #         # For each test
    #             # calculate the average FPS between each backend preference
    #             # append highest fps average to list within the dictionary key for current platform
    # # else:
    #     # Get all tests (platform-agnostic and with backend preference)
    #     # for each test
    #         # calculate average FPS between repetitions
    #         # Add average fps to one of three dictionary keys "No YOLOv7", "YOLOv7 with CUDA" or "YOLOv7 without CUDA"

except sqlite3.Error as e:
    print(e)
    raise e

finally:
    if sqlite_connection:
        sqlite_cursor.close()
        sqlite_connection.close()
