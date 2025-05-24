import dataclasses
import matplotlib.pyplot
import numpy
import os
import sqlite3
import statistics


@dataclasses.dataclass
class FrameTimeData:
    """Frame time data to generate frame time graphs.

    Attributes:
        platform_name (str): The name of the platform.
        number_of_frames (int): The number of frames present in each frame time list.
        frame_times (dict[str, list[list[float]]]): A dictionary where keys are the either the platform name or the test type name and the values are a list of the frame times.
        unit (str): The unit of the frame times to be used for y-axis display purposes.
    """

    platform_name: str
    number_of_frames: int
    frame_times: dict[str, list[list[float]]]
    unit: str


@dataclasses.dataclass
class FramesPerSecondData:
    """FPS data to generate FPS graphs.

    Attributes:
        platform_name (str): The name of the platform.
        test_names (str): The name of the tests.
        average_frames_per_second (dict[str, list[float]]): A dictionary where keys are the either the platform name or the test type and the values are a list of the FPS values.
    """

    platform_name: str
    test_names: list[str]
    average_frames_per_second: dict[str, list[float]]


class PerformanceGraphs:
    """Generates performance graphs based on performance test data stored within the SQLite database.

    Attributes:
        platform_name (str): The name of the platform.
        database_file_path (str): The path to the SQLite database file.
        graph_output_directory (str): The path to the directory in which to save the performance graphs.
        is_multi_platform (bool): Whether the PerformanceGraphs class is configured for multiple platforms.
    """

    def __init__(self, platform_name: str, database_file_path: str, graph_output_directory: str):
        """Constructs the class and initialise the SQLite database.

        Args:
            platform_name (str): The name of the platform.
            database_file_path (str): The path to the SQLite database file.
            graph_output_directory (str): The path to the directory in which to save the performance graphs.
        """
        self.platform_name: str = platform_name
        self.database_file_path: str = database_file_path
        self.graph_output_directory: str = graph_output_directory

        self.is_multi_platform: bool = True if self.platform_name == "all" else False

        # Constants
        self.TITLE_FONT: dict = {"family": "arial", "color": "black", "weight": "bold", "size": 24}
        self.LABEL_FONT: dict = {"family": "arial", "color": "black", "weight": "normal", "size": 18}
        self.TICK_FONT_SIZE: int = 14
        self.LEGEND_FONT_SIZE: int = 14

        self.BAR_WIDTH: float = 0.25

        self.FIGURE_WIDTH: int = 16
        self.FIGURE_HEIGHT: int = 9

        # Setup database connection
        self.sqlite_database = self.SQLiteDatabase(self.database_file_path)

    def generate_performance_graphs(self) -> None:
        """Generates the performance graphs."""
        frame_time_data, frames_per_second_data = self.sqlite_database.get_frame_time_data(self.platform_name)

        # Line graph
        figure_line, axes_line = matplotlib.pyplot.subplots()
        figure_line.set_figwidth(self.FIGURE_WIDTH)
        figure_line.set_figheight(self.FIGURE_HEIGHT)

        if self.is_multi_platform:
            axes_line.set_prop_cycle(color=["#888888", "#0000AA", "#00AA00"])
        else:
            axes_line.set_prop_cycle(color=["#000000", "#00AA00", "#AA0000"])

        x = numpy.arange(frame_time_data.number_of_frames)

        for category, frame_times in frame_time_data.frame_times.items():
            for index, value in enumerate(frame_times):
                if index == 0:
                    line = axes_line.plot(x, value, label=category)
                    line_colour = line[0].get_color()
                else:
                    axes_line.plot(x, value, c=line_colour)

        axes_line.set_title(f"{frame_time_data.platform_name} Frame Times", fontdict=self.TITLE_FONT)

        axes_line.set_xlabel("Frame Number", fontdict=self.LABEL_FONT)
        axes_line.xaxis.set_tick_params(labelsize=self.TICK_FONT_SIZE)
        axes_line.set_xlim(0, frame_time_data.number_of_frames - 1)

        axes_line.set_ylabel(f"Time to compute frame ({frame_time_data.unit})", fontdict=self.LABEL_FONT)
        axes_line.yaxis.set_tick_params(labelsize=self.TICK_FONT_SIZE)

        axes_line.legend(prop={"size": self.LEGEND_FONT_SIZE}, loc="upper right")

        axes_line.grid()
        axes_line.set_axisbelow(True)
        figure_line.tight_layout()

        figure_line.savefig(
            f"{self.graph_output_directory}/{frame_time_data.platform_name.lower().replace(' ', '_')}_frame_time_graph.png"
        )

        # Bar chart
        figure_bar, axes_bar = matplotlib.pyplot.subplots()
        figure_bar.set_figwidth(self.FIGURE_WIDTH)
        figure_bar.set_figheight(self.FIGURE_HEIGHT)

        if self.is_multi_platform:
            x_axis = numpy.arange(len(frames_per_second_data.test_names))
            x_ticks = numpy.arange(len(frames_per_second_data.test_names))
            multiplier = -1
            fps_data = frames_per_second_data.average_frames_per_second
            axes_bar.set_prop_cycle(color=["#888888", "#0000AA", "#00AA00"])
        else:
            x_axis = numpy.arange(len(frames_per_second_data.test_names))[1:]
            x_ticks = numpy.arange(len(frames_per_second_data.test_names))
            multiplier = -0.5
            fps_data = dict(list(frames_per_second_data.average_frames_per_second.items())[1:])
            axes_bar.set_prop_cycle(color=["#000000", "#00AA00", "#AA0000"])

            rects = axes_bar.bar(
                x=frames_per_second_data.test_names[0],
                height=frames_per_second_data.average_frames_per_second[frames_per_second_data.test_names[0]],
                width=self.BAR_WIDTH,
                label=frames_per_second_data.test_names[0],
            )
            axes_bar.bar_label(rects, label_type="edge", padding=1)

        for category, average_fps in fps_data.items():
            offset = self.BAR_WIDTH * multiplier
            rects = axes_bar.bar(x=x_axis + offset, height=average_fps, width=self.BAR_WIDTH, label=category)
            axes_bar.bar_label(rects, label_type="edge", padding=1)
            multiplier += 1

        axes_bar.set_title(f"{frames_per_second_data.platform_name} FPS", fontdict=self.TITLE_FONT)

        axes_bar.set_xlabel("YOLOv7 Type and Blob Size", fontdict=self.LABEL_FONT)
        axes_bar.set_xticks(ticks=x_ticks, labels=frames_per_second_data.test_names, rotation=45, ha="center")
        axes_bar.xaxis.set_tick_params(labelsize=self.TICK_FONT_SIZE)

        axes_bar.set_ylabel("Frames Per Second (FPS)", fontdict=self.LABEL_FONT)
        axes_bar.yaxis.set_tick_params(labelsize=self.TICK_FONT_SIZE)

        axes_bar.legend(prop={"size": self.LEGEND_FONT_SIZE}, loc="upper right")

        axes_bar.grid()
        axes_bar.set_axisbelow(True)
        figure_bar.tight_layout()

        figure_bar.savefig(
            f"{self.graph_output_directory}/{frames_per_second_data.platform_name.lower().replace(' ', '_')}_fps_graph.png"
        )

    class SQLiteDatabase:
        """SQLite wrapper class. This class is in the private scope as the implementation is PerformanceGraphs-specific."""

        def __init__(self, database_file_path: str):
            """Constructs the class.

            Raises:
                DatabaseFileNotFoundError: If the SQLite database file is not found.
            """
            if not os.path.isfile(database_file_path):
                raise self.DatabaseFileNotFoundError(f"SQLite database file '{database_file_path}' not found!")

            self.database_file_path: str = database_file_path

        class DatabaseFileNotFoundError(Exception):
            """Thrown when the SQLite database file is not found."""

            pass

        class DataQualityError(Exception):
            """Thrown when there are data quality errors in the SQLite database."""

            pass

        def __validate_data(self, platform_name: str) -> None:
            """Opens/closes a connection to the SQLite database and performs data quality tests.

            Args:
                platform_name (str): The name of the platform.

            Raises:
                DataQualityError: If there are data quality violations.
            """
            try:
                sqlite_connection: sqlite3.Connection = sqlite3.connect(database=self.database_file_path)
                sqlite_cursor: sqlite3.Cursor = sqlite_connection.cursor()

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

                # Confirm only a single time unit conversion exists across tests
                sqlite_cursor.execute(
                    """
                    SELECT DISTINCT
                        TimeUnitConversion
                    FROM
                        FrameTimes
                    """
                )
                if len(sqlite_cursor.fetchall()) > 1:
                    error_message += (
                        "\nExpected identical time unit conversions across all tests but found different time unit conversions!"
                    )

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
                    error_message += "\nExpected a values between 0, 288, 320, 416, 512, and 608 for ObjectDetectorBlobSize but got other values!"

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
                    if len(sqlite_cursor.fetchall()) == 0:
                        error_message += (
                            f"\nExpected the platform '{platform_name}' to be found in the database but it was not found!"
                        )

                if error_message:
                    raise self.DataQualityError("The following violations have been found in the data:" + error_message)

            except sqlite3.Error as e:
                raise e

            finally:
                if sqlite_connection:
                    sqlite_cursor.close()
                    sqlite_connection.close()

        def __get_data(self, platform_name: str) -> tuple[FrameTimeData, FramesPerSecondData]:
            """Gets the data needed to generate performance graphs.

            Args:
                platform_name (str): The name of the platform.

            Returns:
                tuple[FrameTimeData, FramesPerSecondData]: The data needed to generate the frame times and FPS performance
                graphs.
            """
            try:
                sqlite_connection: sqlite3.Connection = sqlite3.connect(database=self.database_file_path)
                sqlite_cursor: sqlite3.Cursor = sqlite_connection.cursor()

                platform_display_name: str = "All Platforms" if platform_name == "all" else platform_name

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
                        TimeUnit
                    FROM
                        FrameTimes
                    """
                )
                time_unit: str = sqlite_cursor.fetchall()[0][0]

                sqlite_cursor.execute(
                    """
                    SELECT DISTINCT
                        TimeUnitConversion
                    FROM
                        FrameTimes
                    """
                )
                time_unit_conversion: int = sqlite_cursor.fetchall()[0][0]

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

                sqlite_cursor.execute(
                    """
                    SELECT DISTINCT
                        ObjectDetectorType,
                        ObjectDetectorBackEnd,
                        ObjectDetectorBlobSize
                    FROM
                        FrameTimes
                    ORDER BY
                        ObjectDetectorType     ASC,
                        ObjectDetectorBackEnd  ASC,
                        ObjectDetectorBlobSize ASC
                    """
                )
                all_tests: list[str] = sqlite_cursor.fetchall()

                sqlite_cursor.execute(
                    """
                    SELECT DISTINCT
                        Platform
                    FROM
                        FrameTimes
                    ORDER BY
                        Platform
                    """
                )
                all_platforms: list[str] = [row[0] for row in sqlite_cursor.fetchall()]

                frame_times: dict[str, list[list[float]]] = {}

                if platform_name == "all":
                    for platform in all_platforms:
                        current_platform_frame_times: list[list[float]] = []

                        for test in all_tests:
                            sqlite_cursor.execute(
                                f"""
                                SELECT
                                    AVG(FrameTime)
                                FROM
                                    FrameTimes
                                WHERE
                                    Platform               = '{platform}'
                                AND ObjectDetectorType     = {test[0]}
                                AND ObjectDetectorBackEnd  = {test[1]}
                                AND ObjectDetectorBlobSize = {test[2]}
                                GROUP BY
                                    FrameNumber
                                """
                            )
                            current_platform_frame_times.append([row[0] for row in sqlite_cursor.fetchall()])

                        frame_times[platform] = current_platform_frame_times

                else:
                    frame_times[f"No {yolo_name}"] = []
                    frame_times[f"{yolo_name} (GPU)"] = []
                    frame_times[f"{yolo_name} (CPU)"] = []

                    for test in all_tests:
                        sqlite_cursor.execute(
                            f"""
                            SELECT
                                AVG(FrameTime)
                            FROM
                                FrameTimes
                            WHERE
                                Platform               = '{platform_name}'
                            AND ObjectDetectorType     = {test[0]}
                            AND ObjectDetectorBackEnd  = {test[1]}
                            AND ObjectDetectorBlobSize = {test[2]}
                            GROUP BY
                                FrameNumber
                            """
                        )

                        if test[0] == 0:
                            frame_times[f"No {yolo_name}"].append([row[0] for row in sqlite_cursor.fetchall()])
                        elif test[1] == 1:
                            frame_times[f"{yolo_name} (GPU)"].append([row[0] for row in sqlite_cursor.fetchall()])
                        elif test[1] == 2:
                            frame_times[f"{yolo_name} (CPU)"].append([row[0] for row in sqlite_cursor.fetchall()])

                average_frames_per_second: dict[str, list[float]] = {}

                for key, value in frame_times.items():
                    average_frames_per_second[key] = []
                    for frame_time in value:
                        average_frames_per_second[key].append(
                            round(1 / (statistics.mean(frame_time) / time_unit_conversion), 1)
                        )

                frame_time_data: FrameTimeData = FrameTimeData(platform_display_name, number_of_frames, frame_times, time_unit)
                frames_per_second_data: FramesPerSecondData = FramesPerSecondData(
                    platform_display_name, test_names, average_frames_per_second
                )

                return frame_time_data, frames_per_second_data

            except sqlite3.Error as e:
                raise e

            finally:
                if sqlite_connection:
                    sqlite_cursor.close()
                    sqlite_connection.close()

        def get_frame_time_data(self, platform_name: str) -> tuple[FrameTimeData, FramesPerSecondData]:
            """Opens/closes a connection to the SQLite database, performs data quality tests, and if data quality tests pass
            then it gets the the data needed to generate performance graphs.

            Args:
                platform_name (str): The name of the platform.

            Returns:
                tuple[FrameTimeData, FramesPerSecondData]: The data needed to generate the frame times and FPS performance
                graphs.
            """
            self.__validate_data(platform_name)
            return self.__get_data(platform_name)
