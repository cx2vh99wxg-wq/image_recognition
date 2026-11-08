#!/usr/bin/env python3
"""Cap HDMI presentation at 1080p60 without changing cameras, models or DSI."""
import os
import re
import subprocess

def plan(text):
    commands=[]
    for block in re.split(r'(?=^\S+ (?:connected|disconnected))',text,flags=re.M):
        match=re.match(r'(HDMI[\w-]*) connected( primary)? (\d+)x(\d+)\+',block)
        if not match:
            continue
        name,primary,width,height=match.groups();width,height=int(width),int(height)
        args=['xrandr','--output',name]
        if not primary:args+=['--primary']
        mode=re.search(r'^\s+1920x1080\s+.*\b60\.0\d',block,re.M)
        current=re.search(r'^\s+1920x1080\s+([^\n]*)',block,re.M)
        at60=bool(current and re.search(r'60\.0\d[^\s]*\*',current[1]))
        if mode and (width>1920 or height>1080 or (width==1920 and height==1080 and not at60)):
            args+=['--mode','1920x1080','--rate','60']
        if len(args)>3:commands.append(args)
    return commands

def prepare():
    if os.environ.get('VISION_KEEP_DISPLAY_MODE')=='1' or not os.environ.get('DISPLAY'):
        return
    try:
        text=subprocess.check_output(['xrandr','--query'],text=True,timeout=3)
        for args in plan(text):
            subprocess.run(args,check=True,timeout=3)
            print('[DISPLAY] HDMI presentation <=1920x1080@60; camera/model sizes and DSI unchanged',flush=True)
    except (OSError,subprocess.SubprocessError) as error:
        print('[DISPLAY] keeping current mode:',error,flush=True)

if __name__=='__main__':prepare()
