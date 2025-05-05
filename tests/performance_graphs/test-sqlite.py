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
            Unit                    TEXT                NOT NULL
        );
        """
    )

    sqlite_cursor.execute(
        """
        INSERT INTO
        FrameTimes(Platform, YoloName, ObjectDetectorType, ObjectDetectorBackEnd, ObjectDetectorBlobSize, Repetition, FrameNumber, FrameTime, Unit)
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
    sqlite_cursor.execute("SELECT DISTINCT YoloName FROM FrameTimes")
    if len(sqlite_cursor.fetchall()) > 1:
        error_message += "More than one YOLO name exists in given tests!\n"

    # Confirm only a single time unit exists across tests
    sqlite_cursor.execute("SELECT DISTINCT Unit FROM FrameTimes")
    if len(sqlite_cursor.fetchall()) > 1:
        error_message += "More than one time unit exists in given tests!\n"

    # Confirm ObjectDetectorBackEnd and ObjectDetectorBlobSize are non-zero if ObjectDetectorType is non-zero
    sqlite_cursor.execute(
        "SELECT * FROM FrameTimes WHERE ObjectDetectorType = 0 AND (ObjectDetectorBackEnd != 0 OR ObjectDetectorBlobSize != 0)"
    )
    if len(sqlite_cursor.fetchall()) > 0:
        error_message += (
            "ObjectDetectorBackEnd and ObjectDetectorBlobSize are non-zero when ObjectDetectorType is zero in given tests!\n"
        )

    # Confirm ObjectDetectorBackEnd and ObjectDetectorBlobSize are zero if ObjectDetectorType is zero
    sqlite_cursor.execute(
        "SELECT * FROM FrameTimes WHERE ObjectDetectorType != 0 AND (ObjectDetectorBackEnd = 0 OR ObjectDetectorBlobSize = 0)"
    )
    if len(sqlite_cursor.fetchall()) > 0:
        error_message += (
            "ObjectDetectorBackEnd and ObjectDetectorBlobSize are zero when ObjectDetectorType is non-zero in given tests!\n"
        )

    # Confirm enums between expected range
    sqlite_cursor.execute("SELECT DISTINCT ObjectDetectorType FROM FrameTimes EXCEPT SELECT 0, 1, 2")
    if len(sqlite_cursor.fetchall()) > 0:
        error_message += f"Expected a values between 0, 1, and 2 for ObjectDetectorType but got other values!\n"

    sqlite_cursor.execute("SELECT DISTINCT ObjectDetectorBackEnds FROM FrameTimes EXCEPT SELECT 0, 1, 2")
    if len(sqlite_cursor.fetchall()) > 0:
        error_message += f"Expected a values between 0, 1, and 2 for ObjectDetectorBackEnds but got other values!\n"

    sqlite_cursor.execute("SELECT DISTINCT ObjectDetectorBlobSizes FROM FrameTimes EXCEPT SELECT 0, 288, 320, 416, 512, 608")
    if len(sqlite_cursor.fetchall()) > 0:
        error_message += (
            f"Expected a values between 0, 288, 320, 416, 512, and 608 for ObjectDetectorBlobSizes but got other values!\n"
        )

    # TODO: Check all tests have the same number_of_frames

    if error_message:
        raise Exception(error_message)

    # Get data logic
    platform_name: str = "all"

    platform_display_name: str = "All Platforms" if platform_name == "all" else platform_name
    number_of_frames: int =
    frame_times: dict[str, list[list[float]]] =
    unit: str =

    test_names: str =
    average_frames_per_second: dict[str, list[float]] =

except sqlite3.Error as e:
    print(e)
    raise e

finally:
    if sqlite_connection:
        sqlite_cursor.close()
        sqlite_connection.close()
