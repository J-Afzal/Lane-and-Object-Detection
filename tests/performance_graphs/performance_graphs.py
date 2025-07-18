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
        is_multi_platform (bool): Whether the current data is from multiple platforms.
        number_of_frames (int): The number of frames present in each frame time list.
        frame_times (dict[str, list[list[float]]]): A dictionary where keys are the either the platform name or the test type name and the values are a list of the frame times.
        unit (str): The unit of the frame times to be used for y-axis display purposes.
    """

    platform_name: str
    is_multi_platform: bool
    number_of_frames: int
    frame_times: dict[str, list[list[float]]]
    unit: str


@dataclasses.dataclass
class FramesPerSecondData:
    """FPS data to generate FPS graphs.

    Attributes:
        platform_name (str): The name of the platform.
        is_multi_platform (bool): Whether the current data is from multiple platforms.
        test_names (str): The name of the tests.
        average_frames_per_second (dict[str, list[float]]): A dictionary where keys are the either the platform name or the test type and the values are a list of the FPS values.
    """

    platform_name: str
    is_multi_platform: bool
    test_names: list[str]
    average_frames_per_second: dict[str, list[float]]


class PerformanceGraphs:
    """Generates performance graphs based on performance test data stored within the SQLite database.

    Attributes:
        graph_output_directory (str): The path to the directory in which to save the performance graphs.
        sqlite_database (str): SQLite wrapper class for all the SQLite databases.
    """

    def __init__(self, database_file_paths: str, graph_output_directory: str):
        """Constructs the class and initialise the SQLite database.

        Args:
            database_file_paths (str): A comma separated list to the SQLite database file paths for each platform.
            graph_output_directory (str): The path to the directory in which to save the performance graphs.
        """
        # Constants
        self.TITLE_FONT: dict = {"family": "arial", "color": "black", "weight": "bold", "size": 24}
        self.LABEL_FONT: dict = {"family": "arial", "color": "black", "weight": "normal", "size": 18}
        self.TICK_FONT_SIZE: int = 14
        self.LEGEND_FONT_SIZE: int = 14

        self.BAR_WIDTH: float = 0.25

        self.FIGURE_WIDTH: int = 16
        self.FIGURE_HEIGHT: int = 9

        # Attributes
        self.__graph_output_directory: str = graph_output_directory

        # Setup database connection
        self.__sqlite_database = self.SQLiteDatabase(database_file_paths)

    def generate_performance_graphs(self) -> None:
        """Generates all the performance graphs."""
        all_platforms_database_data: list[tuple[FrameTimeData, FramesPerSecondData]] = (
            self.__sqlite_database.get_frame_time_data()
        )

        for platform_database_data in all_platforms_database_data:
            self.__generate_platform_performance_graphs(platform_database_data)

    def __generate_platform_performance_graphs(self, database_data: tuple[FrameTimeData, FramesPerSecondData]):
        """Generates the performance graphs for a specific platform.

        Args:
            database_data (tuple[FrameTimeData, FramesPerSecondData]): A tuple containing the frame time and FPS data needed to generate FPS graphs.
        """

        (frame_time_data, frames_per_second_data) = database_data

        # Line graph
        figure_line, axes_line = matplotlib.pyplot.subplots()
        figure_line.set_figwidth(self.FIGURE_WIDTH)
        figure_line.set_figheight(self.FIGURE_HEIGHT)

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
            f"{self.__graph_output_directory}/{frame_time_data.platform_name.lower().replace(' ', '_').replace('.', '_')}_frame_time_graph.png"
        )

        # Bar chart
        figure_bar, axes_bar = matplotlib.pyplot.subplots()
        figure_bar.set_figwidth(self.FIGURE_WIDTH)
        figure_bar.set_figheight(self.FIGURE_HEIGHT)

        if frames_per_second_data.is_multi_platform:
            x_axis = numpy.arange(len(frames_per_second_data.test_names))
            x_ticks = numpy.arange(len(frames_per_second_data.test_names))
            multiplier = -1
            fps_data = frames_per_second_data.average_frames_per_second
        else:
            x_axis = numpy.arange(len(frames_per_second_data.test_names))[1:]
            x_ticks = numpy.arange(len(frames_per_second_data.test_names))
            multiplier = -0.5
            fps_data = dict(list(frames_per_second_data.average_frames_per_second.items())[1:])

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
            f"{self.__graph_output_directory}/{frames_per_second_data.platform_name.lower().replace(' ', '_').replace('.', '_')}_fps_graph.png"
        )

    class SQLiteDatabase:
        """SQLite wrapper class. This class is in the private scope as the implementation is PerformanceGraphs-specific.

        Attributes:
            database_file_paths (list[str]): The SQLite database file paths for each platform.
        """

        def __init__(self, database_file_paths: str):
            """Constructs the class.

            Args:
                database_file_paths (str): A comma separated list to the SQLite database file paths for each platform.

            Raises:
                DatabaseFileNotFoundError: If the SQLite database file is not found.
            """

            self.database_file_paths: list[str] = database_file_paths.split(",")

            for database_file_path in self.database_file_paths:
                if not os.path.isfile(database_file_path):
                    raise self.DatabaseFileNotFoundError(f"SQLite database file '{database_file_path}' not found!")

        def get_frame_time_data(self) -> list[tuple[FrameTimeData, FramesPerSecondData]]:
            """Opens/closes a connection to the SQLite database, performs data quality tests, and if data quality tests pass
            then it gets the the data needed to generate performance graphs.

            Returns:
                list[tuple[FrameTimeData, FramesPerSecondData]]: The data needed to generate the frame times and FPS performance graphs.
            """
            self.__validate_data()
            return self.__get_data()

        def __validate_data(self) -> None:
            """Opens/closes a connection to all SQLite databases and performs data quality tests.

            Raises:
                DataQualityError: If there are data quality violations.
            """

            yolo_names: list[str] = []
            time_units: list[str] = []
            time_unit_conversions: list[int] = []
            frame_counts: list[int] = []
            tests_names: list[list[str]] = []

            error_message: str = ""

            # Platform agnostic checks
            for database_file_path in self.database_file_paths:
                try:
                    sqlite_connection: sqlite3.Connection = sqlite3.connect(database=database_file_path)
                    sqlite_cursor: sqlite3.Cursor = sqlite_connection.cursor()

                    # Confirm only a single platform name exists across tests
                    sqlite_cursor.execute(
                        """
                        SELECT DISTINCT
                            PlatformName
                        FROM
                            FrameTimes
                        """
                    )
                    returned_platform_names: list[str] = sqlite_cursor.fetchall()
                    if len(returned_platform_names) > 1:
                        error_message += f"\n{database_file_path}: Expected identical platform names across all tests but found {len(returned_platform_names)} platform names!"

                    # Confirm only a single YOLO name exists across tests
                    sqlite_cursor.execute(
                        """
                        SELECT DISTINCT
                            YoloName
                        FROM
                            FrameTimes
                        """
                    )
                    returned_yolo_names: list[str] = sqlite_cursor.fetchall()
                    if len(returned_yolo_names) > 1:
                        error_message += f"\n{database_file_path}: Expected identical YOLO names across all tests but found {len(returned_yolo_names)} YOLO names!"

                    yolo_names += returned_yolo_names

                    # Confirm only a single time unit exists across tests
                    sqlite_cursor.execute(
                        """
                        SELECT DISTINCT
                            TimeUnit
                        FROM
                            FrameTimes
                        """
                    )
                    returned_time_units: list[str] = sqlite_cursor.fetchall()
                    if len(returned_time_units) > 1:
                        error_message += f"\n{database_file_path}: Expected identical time units across all tests but found {len(returned_time_units)} time units!"

                    time_units += returned_time_units

                    # Confirm only a single time unit conversion exists across tests
                    sqlite_cursor.execute(
                        """
                        SELECT DISTINCT
                            TimeUnitConversion
                        FROM
                            FrameTimes
                        """
                    )
                    returned_time_unit_conversions: list[str] = sqlite_cursor.fetchall()
                    if len(returned_time_unit_conversions) > 1:
                        error_message += f"\n{database_file_path}: Expected identical time unit conversions across all tests but found {len(returned_time_unit_conversions)} time unit conversions!"

                    time_unit_conversions += returned_time_unit_conversions

                    # Confirm frame counts match across all tests
                    sqlite_cursor.execute(
                        """
                        SELECT DISTINCT
                            COUNT(*)
                        FROM
                            FrameTimes
                        GROUP BY
                            ObjectDetectorType,
                            ObjectDetectorBackEnd,
                            ObjectDetectorBlobSize,
                            Repetition
                        """
                    )
                    returned_frame_counts: list[str] = sqlite_cursor.fetchall()
                    if len(returned_frame_counts) > 1:
                        error_message += f"\n{database_file_path}: Expected identical frame counts across all tests but found {len(returned_frame_counts)} frame counts!"

                    frame_counts += returned_frame_counts

                    # Confirm tests match across all platforms
                    sqlite_cursor.execute(
                        """
                        SELECT DISTINCT
                            ObjectDetectorType,
                            ObjectDetectorBackEnd,
                            ObjectDetectorBlobSize
                        FROM
                            FrameTimes
                        """
                    )
                    tests_names.append(sqlite_cursor.fetchall())

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
                        error_message += f"\n{database_file_path}: Expected a values between 0, 1, and 2 for ObjectDetectorType but got other values!"

                    sqlite_cursor.execute(
                        """
                        SELECT
                            *
                        FROM
                            FrameTimes
                        WHERE
                            ObjectDetectorBackEnd NOT IN (0, 1, 2, 3)
                        """
                    )
                    if len(sqlite_cursor.fetchall()) > 0:
                        error_message += f"\n{database_file_path}: Expected a values between 0, 1, and 2 for ObjectDetectorBackEnd but got other values!"

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
                        error_message += f"\n{database_file_path}: Expected a values between 0, 288, 320, 416, 512, and 608 for ObjectDetectorBlobSize but got other values!"

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
                        error_message += f"\n{database_file_path}: Expected ObjectDetectorBackEnd and ObjectDetectorBlobSize to be zero when ObjectDetectorType is zero but found them to be non-zero!"

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
                        error_message += f"\n{database_file_path}: Expected ObjectDetectorBackEnd and ObjectDetectorBlobSize to be non-zero when ObjectDetectorType is non-zero but found them to be zero!"

                except sqlite3.Error as e:
                    raise e

                finally:
                    if sqlite_connection:
                        sqlite_cursor.close()
                        sqlite_connection.close()

            # Multi-platform checks
            if len(set(yolo_names)) > 1:
                error_message += (
                    f"\nAll: Expected identical YOLO names across all platforms but found {len(set(yolo_names))} YOLO names!"
                )

            if len(set(time_units)) > 1:
                error_message += (
                    f"\nAll: Expected identical time units across all platforms but found {len(set(time_units))} time units!"
                )

            if len(set(time_unit_conversions)) > 1:
                error_message += f"\nAll: Expected identical time unit conversions across all platforms but found {len(set(time_unit_conversions))} time unit conversions!"

            if len(set(frame_counts)) > 1:
                error_message += f"\nAll: Expected identical frame counts across all platforms but found {len(set(frame_counts))} frame counts!"

            for i in range(0, len(tests_names) - 1):
                if sorted(tests_names[i]) != sorted(tests_names[i + 1]):
                    error_message += (
                        f"\nAll: Expected identical test names across all platforms but found different test names!"
                    )
                    break

            if error_message:
                raise self.DataQualityError("The following violations have been found in the database data:" + error_message)

        def __get_data(self) -> list[tuple[FrameTimeData, FramesPerSecondData]]:
            """Gets the data needed to generate performance graphs.

            Returns:
                list[tuple[FrameTimeData, FramesPerSecondData]]: The data needed to generate the frame times and FPS performance graphs.
            """

            # Get platform agnostic data
            try:
                sqlite_connection: sqlite3.Connection = sqlite3.connect(database=self.database_file_paths[0])
                sqlite_cursor: sqlite3.Cursor = sqlite_connection.cursor()

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
                all_tests: list[tuple[int]] = sqlite_cursor.fetchall()

            except sqlite3.Error as e:
                raise e

            finally:
                if sqlite_connection:
                    sqlite_cursor.close()
                    sqlite_connection.close()

            # Get platform specific data
            all_platforms_database_data: list[tuple[FrameTimeData, FramesPerSecondData]] = []

            for database_file_path in self.database_file_paths:
                all_platforms_database_data.append(
                    self.__get_platform_specific_data(
                        database_file_path, yolo_name, time_unit, time_unit_conversion, number_of_frames, test_names, all_tests
                    )
                )

            if len(self.database_file_paths) > 1:
                all_platforms_database_data.append(
                    self.__get_all_platform_data(
                        self.database_file_paths,
                        time_unit,
                        time_unit_conversion,
                        number_of_frames,
                        test_names,
                        all_tests,
                    )
                )

            return all_platforms_database_data

        def __get_platform_specific_data(
            self,
            database_file_path: str,
            yolo_name: str,
            time_unit: str,
            time_unit_conversion: int,
            number_of_frames: int,
            test_names: list[str],
            all_tests: list[tuple[int]],
        ) -> tuple[FrameTimeData, FramesPerSecondData]:
            """Gets the platform specific data needed to generate performance graphs for a specific platform.

            Args:
                database_file_path (str): The SQLite database path for the current platform.
                yolo_name (str): The name of the current YOLO version (e.g. YOLOv7)
                time_unit (str): The unit of the frame times (e.g. ms)
                time_unit_conversion (int): The conversion factor from time_unit to seconds (e.g. 1000)
                number_of_frames (int): The number of frames within a single test
                test_names (list[str]): The name of all tests
                all_tests (list[tuple[int]]): The properties of all tests

            Returns:
                tuple[FrameTimeData, FramesPerSecondData]: The data needed to generate the frame times and FPS performance graphs for a specific platform.
            """

            frame_times: dict[str, list[list[float]]] = {}
            average_frames_per_second: dict[str, list[float]] = {}

            try:
                sqlite_connection: sqlite3.Connection = sqlite3.connect(database=database_file_path)
                sqlite_cursor: sqlite3.Cursor = sqlite_connection.cursor()

                sqlite_cursor.execute(
                    """
                    SELECT DISTINCT
                        PlatformName
                    FROM
                        FrameTimes
                    """
                )
                platform_name: str = sqlite_cursor.fetchall()[0][0]
                is_multi_platform: bool = False

                frame_times[f"No {yolo_name}"] = []
                frame_times[f"{yolo_name} (CPU)"] = []
                frame_times[f"{yolo_name} (GPU)"] = []
                frame_times[f"{yolo_name} (CUDA)"] = []

                for test in all_tests:
                    sqlite_cursor.execute(
                        f"""
                        SELECT
                            AVG(FrameTime)
                        FROM
                            FrameTimes
                        WHERE
                            ObjectDetectorType     = {test[0]}
                        AND ObjectDetectorBackEnd  = {test[1]}
                        AND ObjectDetectorBlobSize = {test[2]}
                        GROUP BY
                            FrameNumber
                        """
                    )

                    if test[0] == 0:
                        frame_times[f"No {yolo_name}"].append([row[0] for row in sqlite_cursor.fetchall()])
                    elif test[1] == 1:
                        frame_times[f"{yolo_name} (CPU)"].append([row[0] for row in sqlite_cursor.fetchall()])
                    elif test[1] == 2:
                        frame_times[f"{yolo_name} (GPU)"].append([row[0] for row in sqlite_cursor.fetchall()])
                    elif test[1] == 3:
                        frame_times[f"{yolo_name} (CUDA)"].append([row[0] for row in sqlite_cursor.fetchall()])

            except sqlite3.Error as e:
                raise e

            finally:
                if sqlite_connection:
                    sqlite_cursor.close()
                    sqlite_connection.close()

            for key, value in frame_times.items():
                if not value:
                    continue

                average_frames_per_second[key] = []
                for frame_time in value:
                    average_frames_per_second[key].append(round(1 / (statistics.mean(frame_time) / time_unit_conversion), 1))

            frame_time_data: FrameTimeData = FrameTimeData(
                platform_name, is_multi_platform, number_of_frames, frame_times, time_unit
            )
            frames_per_second_data: FramesPerSecondData = FramesPerSecondData(
                platform_name, is_multi_platform, test_names, average_frames_per_second
            )

            return (frame_time_data, frames_per_second_data)

        def __get_all_platform_data(
            self,
            database_file_paths: list[str],
            time_unit: str,
            time_unit_conversion: int,
            number_of_frames: int,
            test_names: list[str],
            all_tests: list[tuple[int]],
        ) -> tuple[FrameTimeData, FramesPerSecondData]:
            """Gets the platform specific data needed to generate performance graphs for multiple platforms.

            Args:
                database_file_path (str): The SQLite database path for the multiple platforms.
                time_unit (str): The unit of the frame times (e.g. ms)
                time_unit_conversion (int): The conversion factor from time_unit to seconds (e.g. 1000)
                number_of_frames (int): The number of frames within a single test
                test_names (list[str]): The name of all tests
                all_tests (list[tuple[int]]): The properties of all tests

            Returns:
                tuple[FrameTimeData, FramesPerSecondData]: The data needed to generate the frame times and FPS performance graphs for a specific platform.
            """

            frame_times: dict[str, list[list[float]]] = {}
            average_frames_per_second: dict[str, list[float]] = {}

            platform_name: str = "All Platforms"
            is_multi_platform: bool = True

            for database_file_path in database_file_paths:
                try:
                    sqlite_connection: sqlite3.Connection = sqlite3.connect(database=database_file_path)
                    sqlite_cursor: sqlite3.Cursor = sqlite_connection.cursor()

                    sqlite_cursor.execute(
                        """
                        SELECT DISTINCT
                            PlatformName
                        FROM
                            FrameTimes
                        """
                    )
                    current_platform_name: str = sqlite_cursor.fetchall()[0][0]

                    current_platform_frame_times: list[list[float]] = []

                    for test in all_tests:
                        sqlite_cursor.execute(
                            f"""
                            SELECT
                                AVG(FrameTime)
                            FROM
                                FrameTimes
                            WHERE
                                ObjectDetectorType     = {test[0]}
                            AND ObjectDetectorBackEnd  = {test[1]}
                            AND ObjectDetectorBlobSize = {test[2]}
                            GROUP BY
                                FrameNumber
                            """
                        )
                        current_platform_frame_times.append([row[0] for row in sqlite_cursor.fetchall()])

                    frame_times[current_platform_name] = current_platform_frame_times

                    sqlite_cursor.execute(
                        f"""
                        WITH Cte AS
                        (
                            SELECT
                                ObjectDetectorType                                      AS ObjectDetectorType,
                                ObjectDetectorBackEnd                                   AS ObjectDetectorBackEnd,
                                ObjectDetectorBlobSize                                  AS ObjectDetectorBlobSize,
                                ROUND(1 / (AVG(FrameTime) / {time_unit_conversion}), 1) AS AverageFPS

                            FROM
                                FrameTimes
                            GROUP BY
                                ObjectDetectorType,
                                ObjectDetectorBackEnd,
                                ObjectDetectorBlobSize
                            ORDER BY
                                ObjectDetectorType     ASC,
                                ObjectDetectorBackEnd  ASC,
                                ObjectDetectorBlobSize ASC
                        )
                        SELECT
                            MAX(AverageFPS)
                        FROM
                            Cte
                        GROUP BY
                            ObjectDetectorType,
                            ObjectDetectorBlobSize
                        ORDER BY
                            ObjectDetectorType     ASC,
                            ObjectDetectorBlobSize ASC
                        """
                    )
                    average_frames_per_second[current_platform_name] = [row[0] for row in sqlite_cursor.fetchall()]

                except sqlite3.Error as e:
                    raise e

                finally:
                    if sqlite_connection:
                        sqlite_cursor.close()
                        sqlite_connection.close()

            frame_time_data: FrameTimeData = FrameTimeData(
                platform_name, is_multi_platform, number_of_frames, frame_times, time_unit
            )
            frames_per_second_data: FramesPerSecondData = FramesPerSecondData(
                platform_name, is_multi_platform, test_names, average_frames_per_second
            )

            return (frame_time_data, frames_per_second_data)

        class DatabaseFileNotFoundError(Exception):
            """Thrown when the SQLite database file is not found."""

            pass

        class DataQualityError(Exception):
            """Thrown when there are data quality errors in the SQLite database."""

            pass
