#!/usr/bin/env python3
"""Limit the dashboard HDMI to 1080p60; camera/model pixel sizes are unchanged."""
import os
import re
import subprocess

def prepare():
    if os.environ.get('VISION_KEEP_DISPLAY_MODE') == '1' or not os.environ.get('DISPLAY'):
        return
    try:
        text = subprocess.check_output(['xrandr', '--query'], text=True, timeout=5)
        for block in re.split(r'(?=^\S+ (?:connected|disconnected))', text, flags=re.M):
            match = re.match(r'(HDMI\S*) connected(?: primary)? (\d+)x(\d+)\+', block)
            if not match:
                continue
            args = ['xrandr', '--output', match[1], '--primary']
            if (int(match[2]) > 1920 or int(match[3]) > 1080) and re.search(r'^\s+1920x1080\s+.*\b60\.0\d', block, re.M):
                args += ['--mode', '1920x1080', '--rate', '60']
            subprocess.run(args, check=True, timeout=5)
            print('[DISPLAY] HDMI primary; dashboard target <=1920x1080@60; camera/model dimensions unchanged')
    except (OSError, subprocess.SubprocessError) as error:
        print('[DISPLAY] keeping current mode:', error)

if __name__ == '__main__':
    prepare()
