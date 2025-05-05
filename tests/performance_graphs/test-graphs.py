# data from https://allisonhorst.github.io/palmerpenguins/

import matplotlib.pyplot
import numpy

from dataclasses import dataclass

TITLE_FONT = {"family": "arial", "color": "black", "weight": "bold", "size": 24}
LABEL_FONT = {"family": "arial", "color": "black", "weight": "normal", "size": 18}
TICK_FONT_SIZE = 14
LEGEND_FONT_SIZE = 14

BAR_WIDTH = 0.25

FIGURE_WIDTH = 16
FIGURE_HEIGHT = 9

################################################################################################################################


@dataclass
class FrameTimeData:
    platform_name: str
    number_of_frames: int
    frame_times: dict[str, list[list[float]]]
    unit: str


@dataclass
class FramesPerSecondData:
    platform_name: str
    test_names: list[str]
    average_frames_per_second: dict[str, list[float]]


################################################################################################################################

is_multi_platform = False

################################################################################################################################

if is_multi_platform:
    data: FramesPerSecondData = FramesPerSecondData(
        platform_name="All Platforms",
        test_names=[
            "No YOLOv4",
            "YOLOv4-tiny 288",
            "YOLOv4-tiny 320",
            "YOLOv4-tiny 416",
            "YOLOv4-tiny 512",
            "YOLOv4-tiny 608",
            "YOLOv4 288",
            "YOLOv4 320",
            "YOLOv4 416",
            "YOLOv4 512",
            "YOLOv4 608",
        ],
        average_frames_per_second={
            "Ubuntu Desktop": [89.0, 68.2, 66.9, 59.1, 29.2, 41.7, 20.7, 20.3, 16.1, 13.2, 8.5],
            "Windows Desktop": [61.3, 43.4, 43.1, 38.6, 26.7, 30.1, 16.2, 18.0, 13.7, 9.3, 7.6],
            "Jetson Nano": [12.7, 8.8, 8.7, 7.5, 6.7, 5.2, 2.4, 2.2, 1.6, 1.3, 0.8],
        },
    )

else:
    data: FramesPerSecondData = FramesPerSecondData(
        platform_name="Windows Desktop",
        test_names=[
            "No YOLOv7",
            "YOLOv7-tiny 288",
            "YOLOv7-tiny 320",
            "YOLOv7-tiny 416",
            "YOLOv7-tiny 512",
            "YOLOv7-tiny 608",
            "YOLOv7 288",
            "YOLOv7 320",
            "YOLOv7 416",
            "YOLOv7 512",
            "YOLOv7 608",
        ],
        average_frames_per_second={
            "No YOLOv7": [61.3],
            "YOLOv7 with CUDA": [43.4, 43.1, 38.6, 17.4, 30.1, 16.2, 18.0, 13.7, 9.3, 7.6],
            "YOLOv7 without CUDA": [40.0, 37.7, 31.7, 26.7, 22.3, 12.1, 10.5, 7.0, 5.2, 3.9],
        },
    )

################################################################################################################################

figure_bar, axes_bar = matplotlib.pyplot.subplots()
figure_bar.set_figwidth(FIGURE_WIDTH)  # TODO for later: global
figure_bar.set_figheight(FIGURE_HEIGHT)  # TODO for later: global


if is_multi_platform:
    x_axis = numpy.arange(len(data.test_names))
    x_ticks = numpy.arange(len(data.test_names))
    multiplier = -1
    fps_data = data.average_frames_per_second
    axes_bar.set_prop_cycle(color=["#888888", "#0000AA", "#00AA00"])
else:
    x_axis = numpy.arange(len(data.test_names))[1:]
    x_ticks = numpy.arange(len(data.test_names))
    multiplier = -0.5
    fps_data = dict(list(data.average_frames_per_second.items())[1:])
    axes_bar.set_prop_cycle(color=["#000000", "#00AA00", "#AA0000"])

    rects = axes_bar.bar(
        x=data.test_names[0],
        height=data.average_frames_per_second[data.test_names[0]],
        width=BAR_WIDTH,
        label=data.test_names[0],
    )
    axes_bar.bar_label(rects, label_type="edge", padding=1)

for category, average_fps in fps_data.items():
    offset = BAR_WIDTH * multiplier
    rects = axes_bar.bar(x=x_axis + offset, height=average_fps, width=BAR_WIDTH, label=category)
    axes_bar.bar_label(rects, label_type="edge", padding=1)
    multiplier += 1

axes_bar.set_title(f"{data.platform_name} FPS", fontdict=TITLE_FONT)

axes_bar.set_xlabel("YOLOv7 Type and Blob Size", fontdict=LABEL_FONT)
axes_bar.set_xticks(ticks=x_ticks, labels=data.test_names, rotation=45, ha="center")
axes_bar.xaxis.set_tick_params(labelsize=TICK_FONT_SIZE)

axes_bar.set_ylabel("Frames Per Second (FPS)", fontdict=LABEL_FONT)
axes_bar.yaxis.set_tick_params(labelsize=TICK_FONT_SIZE)

axes_bar.legend(prop={"size": LEGEND_FONT_SIZE}, loc="upper right")

axes_bar.grid()
axes_bar.set_axisbelow(True)
figure_bar.tight_layout()

figure_bar.savefig(
    f"tests/performance_graphs/{data.platform_name.lower().replace(' ', '_')}_fps.png"
)  # TODO for later: make this a cli param

################################################################################################################################

if is_multi_platform:
    data: FrameTimeData = FrameTimeData(
        platform_name="All Platforms",
        number_of_frames=10,
        frame_times={
            "Ubuntu Desktop": [[1, 3, 1, 3, 1, 3, 1, 3, 1, 3]],
            "Windows Desktop": [
                [10, 30, 10, 30, 10, 30, 10, 30, 10, 30],
                [5, 12, 12, 4, 15, 15, 7, 3, 2, 5],
                [54, 45, 23, 57, 36, 30, 78, 20, 10, 30],
            ],
            "Jetson Nano": [
                [250, 300, 250, 300, 250, 300, 250, 300, 250, 300],
                [150, 200, 220, 330, 180, 130, 150, 120, 170, 165],
                [110, 115, 110, 115, 110, 115, 110, 115, 110, 115],
            ],
        },
        unit="ms",
    )
else:
    data: FrameTimeData = FrameTimeData(
        platform_name="Windows Desktop",
        number_of_frames=10,
        frame_times={
            "No YOLOv7": [[1, 3, 1, 3, 1, 3, 1, 3, 1, 3]],
            "YOLOv7 with CUDA": [
                [10, 30, 10, 30, 10, 30, 10, 30, 10, 30],
                [5, 12, 12, 4, 15, 15, 7, 3, 2, 5],
                [54, 45, 23, 57, 36, 30, 78, 20, 10, 30],
            ],
            "YOLOv7 without CUDA": [
                [250, 300, 250, 300, 250, 300, 250, 300, 250, 300],
                [150, 200, 220, 330, 180, 130, 150, 120, 170, 165],
                [110, 115, 110, 115, 110, 115, 110, 115, 110, 115],
            ],
        },
        unit="ms",
    )

################################################################################################################################

figure_line, axes_line = matplotlib.pyplot.subplots()
figure_line.set_figwidth(FIGURE_WIDTH)
figure_line.set_figheight(FIGURE_HEIGHT)

if is_multi_platform:
    axes_line.set_prop_cycle(color=["#888888", "#0000AA", "#00AA00"])
else:
    axes_line.set_prop_cycle(color=["#000000", "#00AA00", "#AA0000"])

x = numpy.arange(data.number_of_frames)

for category, frame_times in data.frame_times.items():
    for index, value in enumerate(frame_times):
        if index == 0:
            line = axes_line.plot(x, value, label=category)
            line_colour = line[0].get_color()
        else:
            axes_line.plot(x, value, c=line_colour)

axes_line.set_title(f"{data.platform_name} Frame Times", fontdict=TITLE_FONT)

axes_line.set_xlabel("Frame Number", fontdict=LABEL_FONT)
axes_line.xaxis.set_tick_params(labelsize=TICK_FONT_SIZE)
axes_line.set_xlim(0, data.number_of_frames - 1)

axes_line.set_ylabel(f"Time to compute frame ({data.unit})", fontdict=LABEL_FONT)
axes_line.yaxis.set_tick_params(labelsize=TICK_FONT_SIZE)

axes_line.legend(prop={"size": LEGEND_FONT_SIZE}, loc="upper right")

axes_line.grid()
axes_line.set_axisbelow(True)
figure_line.tight_layout()

figure_line.savefig(
    f"tests/performance_graphs/{data.platform_name.lower().replace(' ', '_')}_frame_times.png"
)  # TODO for later: make this a cli param
