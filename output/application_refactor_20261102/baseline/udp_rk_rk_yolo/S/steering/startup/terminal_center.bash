# BEGIN STEERING_TERMINAL_CENTER
# First terminal of a new S boot / M session runs qualified endpoint homing.
# Keep the chassis stationary until homing completes. No zero is fabricated.
if [[ $- == *i* ]] && [[ -z ${STEERING_TERMINAL_CENTER_SKIP:-} ]]; then
    if [[ -f /home/linaro/S/steering/tools/calibration/terminal_home.py ]]; then
        mkdir -p "$HOME/.cache/steering"
        nohup sudo -n python3 /home/linaro/S/steering/tools/calibration/terminal_home.py \
            --fixed-chassis --endstops-verified \
            >> "$HOME/.cache/steering/terminal-home.log" 2>&1 < /dev/null &
    fi
fi
# END STEERING_TERMINAL_CENTER
