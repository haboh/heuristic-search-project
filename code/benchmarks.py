from collections import namedtuple
import json
import subprocess
from typing import List, Optional, Tuple
from pathlib import Path
import os
from tqdm import tqdm
import matplotlib.pyplot as plt
from animation import create_animation

Task = namedtuple("task", "begin_x begin_y end_x end_y")

benchmarks_folder = Path(".") / "output" / "benchmarks"


def read_moving_ai_scene_file(path: Path) -> List[Tuple[int, Task]]:
    with open(path) as f:
        lines = list(f)[1:]
        result = []
        for line in lines:
            tokens = line.split("\t")
            task = Task(
                int(tokens[4]),
                int(tokens[5]),
                int(tokens[6]),
                int(tokens[7]),
            )
            result.append((int(tokens[0]), task))
        return result


def task_to_arguments(t: Task, radius: float, algorithm: str) -> List[str]:
    return [
        "--begin-x",
        str(t.begin_x),
        "--begin-y",
        str(t.begin_y),
        "--end-x",
        str(t.end_x),
        "--end-y",
        str(t.end_y),
        "--radius",
        str(radius),
        "--algorithm",
        algorithm,
    ]


def execute_task(
    map_file: Path,
    task: Task,
    radius: float,
    algorithm: str,
    output_file: Path,
    animation_path: Optional[str] = None,
):
    args = (
        task_to_arguments(task, radius, algorithm)
        + ["--map", str(map_file)]
        + ["--search-result-output", str(output_file)]
    )
    if animation_path is not None:
        args += ["--animation-data-path", "output/tmp.json"]

    print(args)
    result = subprocess.run(["output/main"] + args, timeout=300)
    if animation_path is not None:
        with open("output/tmp.json") as f:
            json_data = json.load(f)
            create_animation(json_data, animation_path)
    return result.returncode
    return -1


def create_comparative_plot(
    log_folder: Path,
    field: str,
    used_algorithms: List[str],
    output_path: Path,
):
    total_result = []
    labels = []
    for subfolder_name in used_algorithms:
        folder = log_folder / subfolder_name
        try:
            folder_result = []
            for file in os.listdir(folder):
                try:
                    with open(folder / file) as f:
                        data = json.load(f)
                        folder_result.append(data[field])
                except Exception:
                    pass
            total_result.append(folder_result)
            labels.append(subfolder_name)
        except Exception:
            pass
    plt.boxplot(total_result, tick_labels=labels)
    plt.savefig(str(output_path))
    plt.close()
    # plt.show()
    return folder_result


algorithms = "dynswsffp", "astar", "dstarlite"
benchmark_folder = Path(".") / "output" / "benchmarks"
run_names = ["brc504d", "den401d", "NewYork_1_256", "Labyrinth"]
# den401d is typical buizlding room layoutm, NewYork --a typical city,  brc504d -- a typical maze-like
radii = [5, 10, 15, 20, 50]


def run_experiments():
    subprocess.run(["make", "all"])
    for radius in radii:
        for run_name in run_names:
            print(f"working with radius={radius} run_name={run_name}")
            run_folder = benchmark_folder / f"r{radius}" / run_name
            scene_file = Path(".") / "data" / f"{run_name}.map.scen"
            map_file = Path(".") / "data" / f"{run_name}.map"
            step = 20 if run_name == "Labyrinth" else 5
            tasks = read_moving_ai_scene_file(scene_file)[::step]
            # shutil.rmtree(benchmark_folder)
            for algorithm in algorithms:
                number = 0
                print(algorithm)
                if (
                    algorithm == "dynswsffp"
                    and radius <= 50
                    and run_name == "Labyrinth"
                ):
                    continue
                for batch, task in tqdm(tasks):
                    output_folder = run_folder / algorithm
                    os.makedirs(output_folder, exist_ok=True)
                    output_file = output_folder / f"n{number}.json"
                    if not os.path.isfile(output_file):
                        retcode = execute_task(
                            map_file, task, radius, algorithm, output_file
                        )
                        if retcode != 0:
                            data = {}
                            data["killed"] = True
                            with open(output_file, "w") as f:
                                json.dump(data, f, ensure_ascii=False, indent=4)
                    number += step


def create_all_plots():
    for radius in radii:
        for run_name in run_names:
            for used_algorithms in [algorithms, algorithms[1:]]:
                log_folder = benchmark_folder / f"r{radius}" / run_name
                output_folder = Path(".") / "output" / "plots" / f"r{radius}"
                os.makedirs(output_folder, exist_ok=True)
                output_path = output_folder / f"{run_name}-{str(used_algorithms)}.png"
                create_comparative_plot(
                    log_folder, "time_millis", used_algorithms, output_path
                )


if __name__ == "__main__":
    data_path = Path("data")
    execute_task(
        data_path / "brc504d.map",
        Task(116, 98, 153, 174),
        5,
        "dstarlite",
        Path("output") / "output.json",
        "hello.gif",
    )
