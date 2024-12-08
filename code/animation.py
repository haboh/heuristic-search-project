from collections import namedtuple
from typing import List
import json

from PIL.Image import Image, new
import subprocess
from PIL import Image
from PIL import ImageDraw

point = namedtuple("point", "x y")


def save_animation(images, output_filename):
    n = len(images)
    max_runtime_secs = 20
    duration = min(200, max_runtime_secs * 1000 // n)
    images[0].save(
        output_filename,
        save_all=True,
        append_images=images[1:],
        optimize=False,
        duration=duration,
        loop=0,
    )


def draw_all_obstacles(img: ImageDraw, all_obstacles: List[point], scale: int):
    overlay = Image.new('RGBA', img.size, (255, 255, 255, 0))
    draw_obj = ImageDraw.Draw(overlay)
    for obstacle in all_obstacles:
        draw_cell(draw_obj, obstacle, scale, color=(70, 80, 80, 60))
    return overlay


def draw_just_expanded(img: ImageDraw, just_expanded: List[point], scale: int):
    overlay = Image.new('RGBA', img.size, (255, 255, 255, 0))
    draw_obj = ImageDraw.Draw(overlay)
    for obstacle in just_expanded:
        draw_cell(draw_obj, obstacle, scale, color=(255, 100, 0, 150))
    return overlay


def draw_known_obstacles(draw_obj: ImageDraw, obstacles: List[point], scale: int):
    for obstacle in obstacles:
        draw_cell(draw_obj, obstacle, scale, color=(70, 80, 80, 255))


def draw_predicted_path(draw_obj: ImageDraw, predicted_path: List[point], scale: int):
    for substep in predicted_path:
        center = (substep.x * scale + 0.5 * scale, substep.y * scale + 0.5 * scale)
        draw_obj.circle(
            radius=scale / 5, xy=center, fill=(255, 255, 255, 255), width=0.0
        )


def draw_cell(draw_obj: ImageDraw, p: point, scale: int, color):
    row = p.y
    col = p.x
    top_left = (col * scale, row * scale)
    bottom_right = ((col + 1) * scale - 1, (row + 1) * scale - 1)
    draw_obj.rectangle(
        [top_left, bottom_right], fill=color, width=0.0
    )


def draw_agent(draw_obj: ImageDraw, agent: point, scale: int):
    center = (agent.x * scale + 0.5 * scale, agent.y * scale + 0.5 * scale)
    draw_obj.circle(
        radius=scale / 2, xy=center, fill=(255, 00, 00, 255), width=0.0
    )


def draw_visible_area(img, agent: point, visible_radius: int, scale: int):
    overlay = Image.new('RGBA', img.size, (255, 255, 255, 0))
    draw_obj = ImageDraw.Draw(overlay)
    row = agent.y
    col = agent.x
    top_left = ((col - visible_radius + 1) * scale, (row - visible_radius + 1) * scale)
    bottom_right = ((col + visible_radius) * scale - 1, (row + visible_radius) * scale - 1)
    draw_obj.rectangle(
        [top_left, bottom_right], fill=(200, 200, 00, 80), width=0.0
    )
    return overlay


# dstar_data = json.load(open(input_filename))

def create_animation(dstar_data: json, output_name: str):
    steps = []
    for json_step in dstar_data["steps"]:
        pos = point(json_step["pos"]["x"], json_step["pos"]["y"])
        new_obstacles = []
        for obstacle in json_step["new_obstacles"]:
            new_obstacles.append(point(obstacle["x"], obstacle["y"]))
        predicted_steps = [point(substep["x"], substep["y"]) for substep in json_step["predicted_path"]]
        expanded_now = [point(substep["x"], substep["y"]) for substep in json_step["expanded_now"]]
        steps.append((pos, new_obstacles, predicted_steps, expanded_now))

    width = dstar_data['columns']
    height = dstar_data['rows']
    visible_radius = dstar_data['radius']
    start_x, start_y = dstar_data['start']['x'], dstar_data['start']['y']
    goal_x, goal_y = dstar_data['goal']['x'], dstar_data['goal']['y']
    all_obstacles = [point(json_point['x'], json_point['y']) for json_point in dstar_data["all_obstacles"]]
    scale = 10

    known_obstacles = []
    frames = []
    for i, step in enumerate(steps):
        im = new("RGBA", (width * scale, height * scale), color=(234, 237, 237, 255))
        draw = ImageDraw.Draw(im, "RGBA")
        agent_pos = step[0]
        new_obstacles = step[1]
        predicted_path = step[2]
        known_obstacles += new_obstacles

        draw_cell(draw, point(start_x, start_y), scale, (0, 200, 0))
        draw_cell(draw, point(goal_x, goal_y), scale, (0, 255, 0))
        draw_known_obstacles(draw, known_obstacles, scale)
        draw_predicted_path(draw, predicted_path, scale)
        draw_agent(draw, agent_pos, scale)
        overlay = draw_visible_area(im, agent_pos, visible_radius, scale)
        im = Image.alpha_composite(im, overlay)
        all_obstacles_overlay = draw_all_obstacles(im, all_obstacles, scale)
        im = Image.alpha_composite(im, all_obstacles_overlay)
        just_expanded = step[3]
        just_expanded_overlay = draw_just_expanded(im, just_expanded, scale)
        im = Image.alpha_composite(im, just_expanded_overlay)
        if new_obstacles:
            im.save(f"snaps/out{i}.png")
        frames.append(im)

    save_animation(frames, output_name)

def display_path(dstar_data: json, output_name: str):
    width = dstar_data['columns']
    height = dstar_data['rows']
    start_x, start_y = dstar_data['start']['x'], dstar_data['start']['y']
    goal_x, goal_y = dstar_data['goal']['x'], dstar_data['goal']['y']
    path = [point(json_point['x'], json_point['y']) for json_point in dstar_data["path"]]
    all_obstacles = [point(json_point['x'], json_point['y']) for json_point in dstar_data["all_obstacles"]]
    scale = 10

    im = new("RGBA", (width * scale, height * scale), color=(234, 237, 237, 255))
    draw = ImageDraw.Draw(im, "RGBA")

    draw_known_obstacles(draw, all_obstacles, scale)
    for p in path:
        draw_cell(draw, p, scale, (255, 0, 0))

    draw_cell(draw, point(start_x, start_y), scale, (0, 200, 0))
    draw_cell(draw, point(goal_x, goal_y), scale, (0, 255, 0))
    im.save(output_name, format="PNG")




# subprocess.run(['make', f'run', f'{run_name}',  f'{task_num}'])
def run(run_name: str, tasknum: int, radius: int, output_folder: str, animation: bool = True, show: bool = False, write_path: bool = False):
    from pathlib import Path
    Path(output_folder).mkdir(parents=True, exist_ok=True)
    subprocess.run(['make', 'all'])
    subprocess.run(['output/main', str(run_name), str(tasknum), str(radius)])
    input_filename = f'output/{run_name}-{tasknum}.json'
    dstar_data = json.load(open(input_filename))
    if animation:
        output_gif_name = output_folder + f"/{run_name}-{tasknum}.gif"
        create_animation(dstar_data, output_gif_name)
        if show:
            subprocess.run(['xviewer', output_gif_name])
    if write_path:
        display_path(dstar_data, output_folder + f"/{run_name}-{tasknum}.png")


task = namedtuple("task", "run_name begin end step radius")
tasks = [
    task("den401d", 300, 700, 50, 10),
    task("NewYork_1_256", 400, 550, 50, 20),
    task("brc504d", 100, 350, 50, 10),
]
for task in tasks:
    for i in range(task.begin, task.end, task.step):
        run(task.run_name, i, task.radius, 'output/' + task.run_name, animation=False, write_path=True)
        print(f'executed {task.run_name}-{i}')
# run("den401d", 45, 10)