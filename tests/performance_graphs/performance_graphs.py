import dataclasses
import matplotlib.pyplot
import numpy
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
        # Line graph
        frame_time_data: FrameTimeData = self.sqlite_database.get_frame_time_data(self.platform_name)

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
        frames_per_second_data: FramesPerSecondData = self.sqlite_database.get_frames_per_second_data(self.platform_name)

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

        def __get_table_data(self, platform_name) -> list[Any]:
            try:
                sqlite_connection: sqlite3.Connection = sqlite3.connect(database=self.database_file_path)
                sqlite_cursor: sqlite3.Cursor = sqlite_connection.cursor()

                if platform_name == "all":
                    sqlite_cursor.execute("SELECT * FROM FrameTimes")
                else:
                    sqlite_cursor.execute(f"SELECT * FROM FrameTimes WHERE Platform = '{platform_name}'")

                return sqlite_cursor.fetchall()

            except sqlite3.Error as e:
                print(e)
                raise e

            finally:
                if sqlite_connection:
                    sqlite_connection.close()

        def get_frame_time_data(self, platform_name: str) -> FrameTimeData:
            table_data: list[Any] = self.__get_table_data(platform_name)

            # "CREATE TABLE IF NOT EXISTS FrameTimes"
            # "("
            # "    Id                      INTEGER PRIMARY KEY NOT NULL"
            # "    Platform                TEXT                NOT NULL"
            # "    YoloType                INTEGER             NOT NULL"
            # "    ObjectDetectorType      INTEGER             NOT NULL"
            # "    ObjectDetectorBackEnd   INTEGER             NOT NULL"
            # "    ObjectDetectorBlobSize  INTEGER             NOT NULL"
            # "    Repetition              INTEGER             NOT NULL"
            # "    FrameNumber             INTEGER             NOT NULL"
            # "    FrameTime               INTEGER             NOT NULL"
            # "    Unit                    TEXT                NOT NULL"
            # ");"

            # TODO:
            # - query for data
            #
            #
            # TODO: get all frame time info for all
            return FrameTimeData()

        def get_frames_per_second_data(self, platform_name: str) -> FramesPerSecondData:
            table_data: list[Any] = self.__get_table_data(platform_name)

            # TODO: get the highest fps value for each test for each platform
            return FramesPerSecondData()
