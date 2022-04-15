import os
import numpy
from matplotlib import pyplot


def generic(platform, platform_file_name, platform_directory, bar_width, title_font, label_font, tick_font_size, legend_font_size, bar_font_size, yolo_types, yolo_types_index, jetson_nano=False):
    # Files
    frame_times = []
    fps_no_yolo = 0
    fps_no_cuda = [0]
    fps_cuda = [0]

    for index, file_name in enumerate(os.listdir(platform_directory)):
        with open(os.path.join(os.getcwd(), platform_directory + file_name), 'r') as file:
            current_frame_times = file.read().split('\n')
            current_frame_times.pop()
            frame_times.append(list(map(float, current_frame_times)))

            if index == 0:
                fps_no_yolo = 1000 / (sum(map(float, current_frame_times)) / len(current_frame_times))
            elif jetson_nano:
                fps_cuda.append(1000 / (sum(map(float, current_frame_times)) / len(current_frame_times)))
            elif index % 2 == 0:
                fps_cuda.append(1000 / (sum(map(float, current_frame_times)) / len(current_frame_times)))
            else:
                fps_no_cuda.append(1000 / (sum(map(float, current_frame_times)) / len(current_frame_times)))

    # Frame Times
    frame_numbers = list(range(len(frame_times[0])))
    fig1, ax1 = pyplot.subplots(1, 1)
    fig1.set_figwidth(15)
    fig1.set_figheight(10)

    for index, frame_time in enumerate(frame_times):
        if index == 0:
            ax1.plot(frame_numbers, frame_time, color='#000000', linewidth=1)
        elif jetson_nano:
            ax1.plot(frame_numbers, frame_time, color='#00AA00', linewidth=1)
        elif index % 2 == 0:
            ax1.plot(frame_numbers, frame_time, color='#00AA00', linewidth=1)
        else:
            ax1.plot(frame_numbers, frame_time, color='#AA0000', linewidth=1)

    ax1.set_title(platform + ' Frame Times', fontdict=title_font)
    ax1.set_xlabel('Frame Number', fontdict=label_font)
    ax1.set_ylabel('Time to compute frame (ms)', fontdict=label_font)
    ax1.set_xlim(0, len(frame_times[0]))
    if jetson_nano:
        ax1.set_ylim(0, 1400)
    else:
        ax1.set_ylim(0, 300)
    ax1.xaxis.set_tick_params(labelsize=tick_font_size)
    ax1.yaxis.set_tick_params(labelsize=tick_font_size)
    leg = ax1.legend(['No YOLOv4', 'YOLOv4 without CUDA', 'YOLOv4 with CUDA'], prop={'size': legend_font_size}, loc='upper right')
    for line in leg.get_lines():
        line.set_linewidth(2)
    ax1.grid()
    pyplot.tight_layout()
    pyplot.savefig('graphs/frame_time_' + platform_file_name + '.png')

    # FPS
    fig2, ax2 = pyplot.subplots(1, 1)
    fig2.set_figwidth(15)
    fig2.set_figheight(10)

    if jetson_nano:
        pad = 0
    else:
        pad = bar_width / 2

    ax2.bar('No YOLOv4', fps_no_yolo, width=bar_width, color='#000000', label='No YOLOv4')
    ax2.text(0, fps_no_yolo, str(f'{fps_no_yolo:.1f}'), ha='center', va='bottom', fontsize=bar_font_size)

    ax2.bar(yolo_types_index - pad, fps_cuda, width=bar_width, color='#00AA00', label='YOLOv4 with CUDA')
    for index, value in enumerate(fps_cuda):
        if index != 0:
            ax2.text(index - pad, value, str(f'{value:.1f}'), ha='center', va='bottom', fontsize=bar_font_size)

    if not jetson_nano:
        ax2.bar(yolo_types_index + pad, fps_no_cuda, width=bar_width, color='#AA0000', label='YOLOv4 without CUDA')
        for index, value in enumerate(fps_no_cuda):
            if index != 0:
                ax2.text(index + pad, value, str(f'{value:.1f}'), ha='center', va='bottom', fontsize=bar_font_size)

    ax2.set_title(platform + ' FPS', fontdict=title_font)
    ax2.set_xlabel('YOLOv4 Type', fontdict=label_font)
    ax2.set_ylabel('Frames Per Second (FPS)', fontdict=label_font)
    ax2.set_xlim(-0.5, 10.5)
    if jetson_nano:
        ax2.set_ylim(0, 16)
    else:
        ax2.set_ylim(0, 100)
    ax2.set_xticks(ticks=yolo_types_index, labels=yolo_types, rotation=45, ha='center')
    ax2.xaxis.set_tick_params(labelsize=tick_font_size)
    ax2.yaxis.set_tick_params(labelsize=tick_font_size)
    ax2.legend(prop={'size': legend_font_size}, loc='upper right')
    ax2.grid()
    ax2.set_axisbelow(True)
    pyplot.tight_layout()
    pyplot.savefig('graphs/fps_' + platform_file_name + '.png')


def all_platforms(bar_width, title_font, label_font, tick_font_size, legend_font_size, bar_font_size, yolo_types, yolo_types_index):
    # Files
    frame_times_ubuntu = []
    frame_times_windows = []
    frame_times_jetson_nano = []

    fps_ubuntu = []
    fps_windows = []
    fps_jetson_nano = []

    ubuntu_files = os.listdir('ubuntu_desktop/')
    with open(os.path.join(os.getcwd(), 'ubuntu_desktop/' + ubuntu_files[0]), 'r') as file:
        frame_times = file.read().split('\n')
        frame_times.pop()
        frame_times_ubuntu.append(list(map(float, frame_times)))
        fps_ubuntu.append(1000 / (sum(map(float, frame_times)) / len(frame_times)))

    for i in range(1, 21, 2):
        with open(os.path.join(os.getcwd(), 'ubuntu_desktop/' + ubuntu_files[i]), 'r') as file:
            frame_times_without_cuda = file.read().split('\n')
            frame_times_without_cuda.pop()
        with open(os.path.join(os.getcwd(), 'ubuntu_desktop/' + ubuntu_files[i + 1]), 'r') as file:
            frame_times_with_cuda = file.read().split('\n')
            frame_times_with_cuda.pop()

        fps_without_cuda = 1000 / (sum(map(float, frame_times_without_cuda)) / len(frame_times_without_cuda))
        fps_with_cuda = 1000 / (sum(map(float, frame_times_with_cuda)) / len(frame_times_with_cuda))

        if fps_without_cuda > fps_with_cuda:
            fps_ubuntu.append(fps_without_cuda)
            frame_times_ubuntu.append(list(map(float, frame_times_without_cuda)))
        else:
            fps_ubuntu.append(fps_with_cuda)
            frame_times_ubuntu.append(list(map(float, frame_times_with_cuda)))

    windows_files = os.listdir('windows_desktop/')
    with open(os.path.join(os.getcwd(), 'windows_desktop/' + windows_files[0]), 'r') as file:
        frame_times = file.read().split('\n')
        frame_times.pop()
        frame_times_windows.append(list(map(float, frame_times)))
        fps_windows.append(1000 / (sum(map(float, frame_times)) / len(frame_times)))

    for i in range(1, 21, 2):
        with open(os.path.join(os.getcwd(), 'windows_desktop/' + windows_files[i]), 'r') as file:
            frame_times_without_cuda = file.read().split('\n')
            frame_times_without_cuda.pop()
        with open(os.path.join(os.getcwd(), 'windows_desktop/' + windows_files[i + 1]), 'r') as file:
            frame_times_with_cuda = file.read().split('\n')
            frame_times_with_cuda.pop()

        fps_without_cuda = 1000 / (sum(map(float, frame_times_without_cuda)) / len(frame_times_without_cuda))
        fps_with_cuda = 1000 / (sum(map(float, frame_times_with_cuda)) / len(frame_times_with_cuda))

        if fps_without_cuda > fps_with_cuda:
            fps_windows.append(fps_without_cuda)
            frame_times_windows.append(list(map(float, frame_times_without_cuda)))
        else:
            fps_windows.append(fps_with_cuda)
            frame_times_windows.append(list(map(float, frame_times_with_cuda)))

    for index, file_name in enumerate(os.listdir('jetson_nano/')):
        with open(os.path.join(os.getcwd(), 'jetson_nano/' + file_name), 'r') as file:
            current_frame_times = file.read().split('\n')
            current_frame_times.pop()
            frame_times_jetson_nano.append(list(map(float, current_frame_times)))
            fps_jetson_nano.append(1000 / (sum(map(float, current_frame_times)) / len(current_frame_times)))

    # Frame Times
    frame_numbers = list(range(len(frame_times_ubuntu[0])))
    fig1, ax1 = pyplot.subplots(1, 1)
    fig1.set_figwidth(15)
    fig1.set_figheight(10)

    for i in range(len(frame_times_ubuntu)):
        ax1.plot(frame_numbers, frame_times_ubuntu[i], color='#888888', linewidth=1)
        ax1.plot(frame_numbers, frame_times_windows[i], color='#0000AA', linewidth=1)
        ax1.plot(frame_numbers, frame_times_jetson_nano[i], color='#00AA00', linewidth=1)

    ax1.set_title('All Platforms Frame Times', fontdict=title_font)
    ax1.set_xlabel('Frame Number', fontdict=label_font)
    ax1.set_ylabel('Time to compute frame (ms)', fontdict=label_font)
    ax1.set_xlim(0, len(frame_times_ubuntu[0]))
    ax1.set_ylim(0, 1400)
    ax1.xaxis.set_tick_params(labelsize=tick_font_size)
    ax1.yaxis.set_tick_params(labelsize=tick_font_size)
    leg = ax1.legend(['Ubuntu Desktop', 'Windows Desktop', 'Jetson Nano'], prop={'size': legend_font_size}, loc='upper right')
    for line in leg.get_lines():
        line.set_linewidth(2)
    ax1.grid()
    pyplot.tight_layout()
    pyplot.savefig('graphs/frame_time_all.png')

    # FPS
    fig2, ax2 = pyplot.subplots(1, 1)
    fig2.set_figwidth(15)
    fig2.set_figheight(10)

    ax2.bar(yolo_types_index - bar_width, fps_ubuntu, width=bar_width, color='#888888', label='Ubuntu Desktop')
    for index, value in enumerate(fps_ubuntu):
        ax2.text(index - bar_width, value, str(f'{value:.1f}'), ha='center', va='bottom', fontsize=bar_font_size)

    ax2.bar(yolo_types_index, fps_windows, width=bar_width, color='#0000AA', label='Windows Desktop')
    for index, value in enumerate(fps_windows):
        ax2.text(index, value, str(f'{value:.1f}'), ha='center', va='bottom', fontsize=bar_font_size)

    ax2.bar(yolo_types_index + bar_width, fps_jetson_nano, width=bar_width, color='#00AA00', label='Jetson Nano')
    for index, value in enumerate(fps_jetson_nano):
        ax2.text(index + bar_width, value, str(f'{value:.1f}'), ha='center', va='bottom', fontsize=bar_font_size)

    ax2.set_title('All Platforms FPS', fontdict=title_font)
    ax2.set_xlabel('YOLOv4 Type', fontdict=label_font)
    ax2.set_ylabel('Frames Per Second (FPS)', fontdict=label_font)
    ax2.set_xlim(-0.5, 10.5)
    ax2.set_ylim(0, 100)
    ax2.set_xticks(ticks=yolo_types_index, labels=yolo_types, rotation=45, ha='center')
    ax2.xaxis.set_tick_params(labelsize=tick_font_size)
    ax2.yaxis.set_tick_params(labelsize=tick_font_size)
    ax2.legend(prop={'size': legend_font_size}, loc='upper right')
    ax2.grid()
    ax2.set_axisbelow(True)
    pyplot.tight_layout()
    pyplot.savefig('graphs/fps_all.png')


def main():
    TITLE_FONT = {'family': 'arial', 'color': 'black', 'weight': 'bold', 'size': 24}
    LABEL_FONT = {'family': 'arial', 'color': 'black', 'weight': 'normal', 'size': 18}
    TICK_FONT_SIZE = 14
    LEGEND_FONT_SIZE = 14
    YOLO_TYPES = ['No YOLOv4', 'YOLOv4-tiny 288', 'YOLOv4-tiny 320', 'YOLOv4-tiny 416', 'YOLOv4-tiny 512', 'YOLOv4-tiny 608', 'YOLOv4 288', 'YOLOv4 320', 'YOLOv4 416', 'YOLOv4 512', 'YOLOv4 608']
    YOLO_TYPES_INDEX = numpy.arange(len(YOLO_TYPES))
    GENERIC_BAR_WIDTH = 0.4
    GENERIC_BAR_FONT_SIZE = 14
    ALL_BAR_WIDTH = 0.25
    ALL_BAR_FONT_SIZE = 10

    generic('Ubuntu Desktop', 'ubuntu_desktop', 'ubuntu_desktop/', GENERIC_BAR_WIDTH, TITLE_FONT, LABEL_FONT, TICK_FONT_SIZE, LEGEND_FONT_SIZE, GENERIC_BAR_FONT_SIZE, YOLO_TYPES, YOLO_TYPES_INDEX)
    generic('Windows Desktop', 'windows_desktop', 'windows_desktop/', GENERIC_BAR_WIDTH, TITLE_FONT, LABEL_FONT, TICK_FONT_SIZE, LEGEND_FONT_SIZE, GENERIC_BAR_FONT_SIZE, YOLO_TYPES, YOLO_TYPES_INDEX)
    generic('Jetson Nano', 'jetson_nano', 'jetson_nano/', GENERIC_BAR_WIDTH, TITLE_FONT, LABEL_FONT, TICK_FONT_SIZE, LEGEND_FONT_SIZE, GENERIC_BAR_FONT_SIZE, YOLO_TYPES, YOLO_TYPES_INDEX, jetson_nano=True)
    all_platforms(ALL_BAR_WIDTH, TITLE_FONT, LABEL_FONT, TICK_FONT_SIZE, LEGEND_FONT_SIZE, ALL_BAR_FONT_SIZE, YOLO_TYPES, YOLO_TYPES_INDEX)


if __name__ == '__main__':
    main()
