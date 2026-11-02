# BEGIN STEERING_TERMINAL_CENTER
# First terminal after M restart declares an unchanged, fixed, centered sensor.
# The helper passively checks the M session and skips an already valid zero.
if [[ $- == *i* ]] && [[ -z ${STEERING_TERMINAL_CENTER_SKIP:-} ]]; then
    if [[ -f /home/linaro/S/steering/tools/calibration/terminal_center.py ]]; then
        mkdir -p "$HOME/.cache/steering"
        nohup sudo -n python3 /home/linaro/S/steering/tools/calibration/terminal_center.py \
            --fixed-chassis --mechanical-center --same-mount \
            >> "$HOME/.cache/steering/terminal-center.log" 2>&1 < /dev/null &
    fi
fi
# END STEERING_TERMINAL_CENTER
