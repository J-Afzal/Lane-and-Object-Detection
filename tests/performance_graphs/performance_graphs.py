import dataclasses
import matplotlib.pyplot
import numpy
import os
import sqlite3


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


class PerformanceGraphs:
    def __init__(self, cli_args: list[str] = [], platform_name=None, database_file_path=None, graph_output_directory=None):
        if len(cli_args) > 1:
            self.__parse_cli_args(cli_args=cli_args)
            return

        if database_file_path is None:
            raise self.InvalidArgumentsError("'database_file_path' not provided!")

        if platform_name is None:
            raise self.InvalidArgumentsError("'platform_name' not provided!")

        if graph_output_directory is None:
            raise self.InvalidArgumentsError("'graph_output_directory' not provided!")

        self.platform_name = platform_name
        self.is_multi_platform = True if platform_name == "all" else False
        self.database_file_path = database_file_path
        self.graph_output_directory = graph_output_directory

        # Constants
        self.TITLE_FONT = {"family": "arial", "color": "black", "weight": "bold", "size": 24}
        self.LABEL_FONT = {"family": "arial", "color": "black", "weight": "normal", "size": 18}
        self.TICK_FONT_SIZE = 14
        self.LEGEND_FONT_SIZE = 14

        self.BAR_WIDTH = 0.25

        self.FIGURE_WIDTH = 16
        self.FIGURE_HEIGHT = 9

        # Setup database connection
        self.sqlite_database = self.SQLiteDatabase(self.database_file_path)

    def generate_performance_graphs(self):
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

    def __parse_cli_args(self, cli_args: list[str]):
        # TODO: argparse
        # self.platform_name
        # self.is_multi_platform
        # self.database_file_path
        # self.graph_output_directory
        pass

    class InvalidArgumentsError(Exception):
        pass

    class SQLiteDatabase:
        def __init__(self, database_file_path):
            if not os.path.isfile(database_file_path):
                raise self.DatabaseFileNotFoundError(f"SQLite database file '{database_file_path}' not found!")

            self.database_file_path = database_file_path

        class DatabaseFileNotFoundError(Exception):
            pass

        class InvalidDataError(Exception):
            pass

        def __validate_data(self, platform_name) -> None:
            sqlite_connection: sqlite3.Connection = sqlite3.connect(database=self.database_file_path)
            sqlite_cursor: sqlite3.Cursor = sqlite_connection.cursor()

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
                if len(sqlite_cursor.fetchall()) > 0:
                    error_message += (
                        f"\nExpected the platform '{platform_name}' to be found in the database but it was not found!"
                    )

            if error_message:
                raise self.InvalidDataError("The following violations have been found in the data:" + error_message)

        def __get_table_data(self, platform_name: str) -> tuple[FrameTimeData, FramesPerSecondData]:
            pass

        def get_frame_time_data(self, platform_name: str) -> tuple[FrameTimeData, FramesPerSecondData]:
            self.__validate_data(platform_name)
            return self.__get_table_data(platform_name)
