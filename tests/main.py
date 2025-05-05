import sys

from performance_graphs.performance_graphs import PerformanceGraphs

if __name__ == '__main__':
    performance_graphs = PerformanceGraphs(cli_args=sys.argv)
    performance_graphs.generate_performance_graphs()
