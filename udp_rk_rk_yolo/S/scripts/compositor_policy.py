#!/usr/bin/env python3
"""Avoid a second desktop compositing pass on S's opaque full-screen dashboard.

Only Xfce's per-user compositor setting is changed. Detection, control, monitor
modes and the application's own alpha drawing are independent of this setting.
Set VISION_KEEP_COMPOSITOR=1 to skip this policy. Restore desktop composition
with: xfconf-query -c xfwm4 -p /general/use_compositing --set true
"""
import os
import shutil
import subprocess


def prepare_compositor(environ=None, run=None, which=None):
    env = os.environ if environ is None else environ
    execute = subprocess.run if run is None else run
    locate = shutil.which if which is None else which
    if env.get('VISION_KEEP_COMPOSITOR') == '1' or not env.get('DISPLAY'):
        return False
    if not locate('xfconf-query'):
        return False
    command = ['xfconf-query', '-c', 'xfwm4', '-p', '/general/use_compositing']

    def call(extra):
        return execute(command + extra, env=env, text=True, stdout=subprocess.PIPE,
                       stderr=subprocess.PIPE, timeout=5)

    try:
        before = call([])
        if before.returncode == 0 and before.stdout.strip() == 'false':
            return True
        # A missing property uses the desktop default. Do not touch an unknown
        # value, or reinterpret a D-Bus/session error as an absent property.
        if before.returncode == 0 and before.stdout.strip() == 'true':
            change = call(['--set', 'false'])
        elif before.returncode != 0 and 'does not exist' in (before.stdout + before.stderr):
            change = call(['--create', '--type', 'bool', '--set', 'false'])
        else:
            print('[S DISPLAY] compositor setting unavailable; keeping desktop state')
            return False
        check = call([])
        okay = change.returncode == 0 and check.returncode == 0 and check.stdout.strip() == 'false'
        print('[S DISPLAY] desktop compositor disabled' if okay else
              '[S DISPLAY] compositor change not confirmed; detector/control unchanged')
        return okay
    except (OSError, subprocess.TimeoutExpired):
        print('[S DISPLAY] compositor query unavailable; detector/control unchanged')
        return False


if __name__ == '__main__':
    prepare_compositor()
