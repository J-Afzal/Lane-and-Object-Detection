import argparse

from argparse import ArgumentParser, Namespace
from performance_graphs.performance_graphs import PerformanceGraphs


def parse_cli_args() -> Namespace:
    parser: ArgumentParser = argparse.ArgumentParser(description="Generates graphs based on performance tests data")

    parser.add_argument(
        "-p",
        "--platform",
        type=str,
        metavar="",
        help="the platform to generate the graphs for. Use 'all' to generate for all platforms.",
    )
    parser.add_argument(
        "-d",
        "--database",
        type=str,
        metavar="",
        help="the path to the SQLite database containing the performance tests data.",
    )
    parser.add_argument("-o", "--output", type=str, metavar="", help="the output directory to save the performance graphs.")

    return parser.parse_args()


if __name__ == "__main__":
    cli_args: Namespace = parse_cli_args()
    performance_graphs: PerformanceGraphs = PerformanceGraphs(cli_args.platform, cli_args.database, cli_args.output)
    performance_graphs.generate_performance_graphs()
