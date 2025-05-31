import argparse

from argparse import ArgumentParser, Namespace
from performance_graphs.performance_graphs import PerformanceGraphs


def parse_cli_args() -> Namespace:
    """Parses CLI arguments and returns them within a `Namespace` object.

    Returns:
        Namespace: The parsed CLI arguments.
    """
    parser: ArgumentParser = argparse.ArgumentParser(description="Generates graphs based on performance tests data")

    parser.add_argument(
        "-d",
        "--database",
        type=str,
        metavar="",
        help="a comma separated list to the SQLite database file paths for each platform, if single file given then graphs are generated for a single platform",
    )
    parser.add_argument(
        "-o", "--output", type=str, metavar="", help="the path to the directory in which to save the performance graphs"
    )

    return parser.parse_args()


if __name__ == "__main__":
    cli_args: Namespace = parse_cli_args()
    performance_graphs: PerformanceGraphs = PerformanceGraphs(cli_args.database, cli_args.output)
    performance_graphs.generate_performance_graphs()
