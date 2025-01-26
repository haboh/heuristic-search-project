from collections import namedtuple
import subprocess
from typing import List, Tuple
from pathlib import Path
import os
import shutil
from tqdm import tqdm

Task = namedtuple('task', 'begin_x begin_y end_x end_y')


def read_moving_ai_scene_file(path: Path) -> List[Tuple[int, Task]]:
    with open(path) as f:
        lines = list(f)[1:]
        result = []
        for line in lines:
            tokens = line.split('\t')
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
        '--begin-x',
        str(t.begin_x),
        '--begin-y',
        str(t.begin_y),
        '--end-x',
        str(t.end_x),
        '--end-y',
        str(t.end_y),
        '--radius',
        str(radius),
        '--algorithm',
        algorithm,
    ]


def run(map_file: Path, task: Task, radius: float, algorithm: str, output_file: Path):
    args = (
        task_to_arguments(task, radius, algorithm)
        + ['--map', str(map_file)]
        + ['--search-result-output', str(output_file)]
    )
    print(' '.join(args))
    subprocess.run(['output/main'] + args)


if __name__ == '__main__':
    subprocess.run(['make', 'all'])
    run_name = 'arena'
    scene_file = Path('.') / 'data' / f'{run_name}.map.scen'
    map_file = Path('.') / 'data' / f'{run_name}.map'
    step = 20
    tasks = read_moving_ai_scene_file(scene_file)[::step][:10]
    benchmark_folder = Path('.') / 'output' / 'benchmarks'
    shutil.rmtree(benchmark_folder)
    for algorithm in 'astar', 'dynswsffp', 'dstarlite':
        number = 0
        print(algorithm)
        for batch, task in tqdm(tasks):
            # print(batch, task, algorithm)
            output_folder = benchmark_folder / run_name / algorithm
            os.makedirs(output_folder, exist_ok=True)
            output_file = output_folder / f'n={number} alg={algorithm}.json'
            run(map_file, task, 5, algorithm, output_file)
            number += step
