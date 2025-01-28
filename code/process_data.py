from pathlib import Path
import os
from dataclasses import dataclass
from typing import List, Optional
import json
import numpy as np
import tabulate as tb
import matplotlib.pyplot as plt


@dataclass
class Metrics:
    time_ms: float
    vertex_ops: int
    set_ops: int


@dataclass
class ExperimentEntry:
    run_name: str
    task_num: int
    radius: int
    algorithm: str
    metrics: Optional[Metrics]


def read_all_execution_data(root: Path) -> List[ExperimentEntry]:
    result = []
    for radius_folder_name in os.listdir(root):
        radius = int(radius_folder_name[1:])
        radius_folder = root / radius_folder_name
        for run_name in os.listdir(radius_folder):
            run_folder = radius_folder / run_name
            for algorithm in os.listdir(run_folder):
                algorithm_folder = run_folder / algorithm
                for run_file_name in os.listdir(algorithm_folder):
                    run_file = algorithm_folder / run_file_name
                    tasknum = int(run_file_name[1:-5])
                    with open(run_file) as f:
                        data = json.load(f)
                        metrics = None
                        if "killed" not in data:
                            metrics = Metrics(
                                data["time_millis"],
                                data["vertex_accesses"],
                                data["array_accesses"],
                            )
                        result.append(
                            ExperimentEntry(
                                run_name, tasknum, radius, algorithm, metrics
                            )
                        )

    return result


algorithms = ["dynswsffp", "astar", "dstarlite"]
main_algorithms = algorithms[1:]
map_names = ["brc504d", "den401d", "NewYork_1_256", "Labyrinth"]


def create_perf_table(data: List[ExperimentEntry]):
    header_row = [""] + map_names
    table = [header_row]
    for i, algorithm in enumerate(main_algorithms):
        row = [algorithm]
        for run_name in map_names:
            needed_data = [
                x
                for x in data
                if x.run_name == run_name
                and x.algorithm == algorithm
                and x.metrics is not None
            ]
            mean_perf = np.mean([x.metrics.set_ops for x in needed_data])
            std = np.std([x.metrics.set_ops for x in needed_data])
            row.append("%.1e" % mean_perf + "±" + "%.1e" % (2 * std))
        table.append(row)
    return tb.tabulate(table, headers="firstrow", tablefmt="latex")


def create_map_bar_plot(data: List[ExperimentEntry], title: str):
    means = []
    stds = []
    for algorithm in main_algorithms:
        perf_data = [
            x.metrics.set_ops
            for x in data
            if x.algorithm == algorithm and x.metrics is not None
        ]
        mean = np.mean(perf_data)
        std = np.std(perf_data)
        means.append(mean)
        stds.append(std)
    plt.close()
    plt.bar(main_algorithms, means)
    plt.errorbar(main_algorithms, means, yerr=stds, fmt="o", color="r")
    plt.ylabel("set_access")
    plt.title(title)
    os.makedirs("output/slide-plots", exist_ok=True)
    plt.savefig(f"output/slide-plots/{title}.png")
    # plt.show()


if __name__ == "__main__":
    print("on start")
    all_data = read_all_execution_data(Path(".") / "output" / "benchmarks")
    # small_visibility_data = [x for x in all_data if x.radius == 5]
    # large_visibility_data = [x for x in all_data if x.radius == 50]

    for radius in 5, 50:
        for map in map_names:
            # print('here!')
            map_data = [x for x in all_data if x.run_name == map and x.radius == radius]
            create_map_bar_plot(map_data, f"r{radius}-{map}")
