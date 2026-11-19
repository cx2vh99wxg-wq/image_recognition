from pathlib import Path
import glob
import json
import os
import subprocess
import sys

root = Path('/userdata/poster_frames_20261119/paired_current')
repository = Path('/home/linaro/M/pcie')
library = repository / 'lib'
clips = Path('/userdata/actor_video_1119')
output = root / 'evidence'
output.mkdir(exist_ok=True)
env = dict(os.environ, LANE_CURVE_MODE='ego-boundaries', LD_LIBRARY_PATH=str(library))
if sys.argv[1] == 'city':
    environments = [
        ('city_green', '112507_17', 17.0, [2.4, 3.6, 5.2, 6.3]),
        ('city_red', '112454_0', 0.0, [0.95, 1.65, 2.35, 3.05]),
        ('city_cars', '113343_17.5', 17.5, [1.9, 2.4, 2.75, 3.2]),
        ('city_ped', '113452_1.4', 1.4, [1.3, 1.85, 2.3, 3.4, 4.4]),
        ('city_lines', '113421_6', 6.0, [1.7, 2.8, 3.8, 4.6, 5.7]),
        ('city_taxi', '113539_17', 17.0, [1.9, 2.8, 4.6, 7.3, 11.5]),
    ]
    inputs = clips
elif sys.argv[1] == 'mountain':
    environments = [('mountain_left', 'mountain_left', 2.0, [0.7, 1.6, 2.5, 3.5]),
                    ('mountain_right', 'mountain_right', 47.0, [0.8, 1.6, 2.6, 3.7])]
    inputs = root
else:
    environments = [
        ('current_cars', '113343_17.5', 17.5, [2, 2.5, 2.9, 3.3]),
        ('current_taxi', '113539_17', 17.0, [2.1, 3.4, 5.2]),
        ('current_people', 'sidewalk_people', 68.7, [1.35, 1.7, 2.05, 2.4, 2.8]),
    ]
    inputs = None
    env.update(POSTER_ACTORS='1', TRAFFIC_NPU_BUDGET_MS='80')

binary = root/('replay_m_actors' if sys.argv[1] == 'actors' else 'replay_m')
if not binary.is_file():
    command = ['g++','-std=c++11','-O2','-pthread','-I'+str(repository/'include'),
               '-I/home/linaro/M/include','-I/home/linaro/common/perception/include',
               root/'paired_replay_m.cpp']
    command += [repository/'build'/f'{obj}.o' for obj in
                ('yolo_integration','traffic_detector','traffic_service','hdmi_front_capture')]
    subprocess.run(command+['-L'+str(library),'-lrknnrt','-lm','-ldl','-o',binary],
                   check=True, env=env)
print('M_REPLAY_BUILD_OK', flush=True)
metadata = {}
for i, (name, clip, start, timestamps) in enumerate(environments, {'city':1,'mountain':10,'actors':20}[sys.argv[1]]):
    video = (inputs if inputs else root if clip=='sidewalk_people' else clips) / (clip + '.mp4')
    if not video.is_file():
        print('MISSING', video, flush=True)
        continue
    intermediate = root / (name + '.yuv')
    subprocess.run(['ffmpeg','-v','error','-nostdin','-y','-threads','1','-i',video,
                    '-t',str(timestamps[-1] + 0.5),'-filter_threads','1','-vf',
                    'fps=12,scale=640:360,pad=640:480:0:60,setsar=1','-an','-pix_fmt','yuyv422',
                    '-f','rawvideo',intermediate],check=True)
    time_indexes = [round(t*12) for t in timestamps]
    for index in time_indexes:
        metadata[f'{name}_{index}'] = dict(video=clip, time=start + index / 12, name=name)
    print('M_REPLAY_CLIP', name, flush=True)
    try:
        subprocess.run(['unshare','--ipc','--fork',binary,intermediate,output,name,
                        str(i*10000),*map(str,time_indexes)],cwd=repository, env=env, check=True)
    finally:
        intermediate.unlink(missing_ok=True)
(output/(sys.argv[1] + '_clips.json')).write_text(json.dumps(metadata))
print('M_REPLAY_COMPLETE', len(list(output.glob('*.meta'))), flush=True)
