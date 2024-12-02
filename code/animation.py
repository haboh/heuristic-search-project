from collections import namedtuple
from typing import List

from PIL import ImageDraw
from PIL.Image import Image, new

from io import BytesIO
from urllib.request import urlopen
from PIL import Image
from PIL import ImageDraw


point = namedtuple("point", "x y")

def save_animation(images, output_filename, quality):
    output_gif_name = f"{output_filename}.gif"
    images[0].save(
        output_gif_name,
        save_all=True,
        append_images=images[1:],
        optimize=False,
        duration=200,
        loop=0,
    )


def draw_grid(draw_obj: ImageDraw, obstacles: List[point], scale: int):
    for obstacle in obstacles:
        row = obstacle.y
        col = obstacle.x
        top_left = (col * scale, row * scale)
        bottom_right = ((col + 1) * scale - 1, (row + 1) * scale - 1)
        draw_obj.rectangle(
            [top_left, bottom_right], fill=(70, 80, 80, 255), width=0.0
        )

def draw_agent(draw_obj: ImageDraw, agent: point, scale: int):
    row = agent.y
    col = agent.x
    center = (col * scale + 0.5 * scale, row * scale + 0.5 * scale)
    draw_obj.circle(
        radius=scale / 2, xy=center, fill=(255, 00, 00, 255), width=0.0
    )

def draw_visible_area(img, agent: point, visible_radius: int, scale: int):
    overlay = Image.new('RGBA', img.size, (255, 255, 255, 0))
    draw_obj = ImageDraw.Draw(overlay)  # Create a context for drawing things on it.
    row = agent.y
    col = agent.x
    top_left = ((col - visible_radius + 1) * scale, (row - visible_radius + 1) * scale)
    bottom_right = ((col + visible_radius) * scale - 1, (row + visible_radius) * scale - 1)
    draw_obj.rectangle(
        [top_left, bottom_right], fill=(200, 200, 00, 80), width=0.0
    )
    return overlay


steps = []
with open("out") as f:
    lines = f.readlines()
for line in lines[1:1000]:
    tokens = list(map(int, line.split()))
    pos = point(tokens[0], tokens[1])
    new_obstacles = []
    for i in range(2, len(tokens), 2):
        new_obstacles.append(point(tokens[i], tokens[i + 1]))
    print(pos, new_obstacles)
    steps.append((pos, new_obstacles))


width, height, visible_radius = list(map(int, lines[0].split()))
scale = 10

obstacles = []
frames = []
for i, step in enumerate(steps):
    im = new("RGBA", (width * scale, height * scale), color=(234, 237, 237, 255))
    draw = ImageDraw.Draw(im, "RGBA")
    agent_pos = step[0]
    new_obstacles = step[1]
    obstacles += new_obstacles
    print(obstacles)
    draw_grid(draw, obstacles, scale)
    draw_agent(draw, agent_pos, scale)
    overlay = draw_visible_area(im, agent_pos, visible_radius, scale)
    im = Image.alpha_composite(im, overlay)
    if new_obstacles:
        im.save(f"snaps/out{i}.png")
    frames.append(im)

save_animation(frames, "anim", 1)