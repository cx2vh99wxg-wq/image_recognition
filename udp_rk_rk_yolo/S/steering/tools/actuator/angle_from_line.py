#!/usr/bin/env python3
"""Convert manual line-intersection readings to an angle; no hardware access."""
import argparse
import math


def finite_number(value):
    try:
        number = float(value)
    except ValueError as error:
        raise argparse.ArgumentTypeError('expected a number') from error
    if not math.isfinite(number):
        raise argparse.ArgumentTypeError('expected a finite number')
    return number


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--baseline-mm', type=finite_number, required=True,
                        help='positive distance b to the fixed x=-b line; specify the actual baseline')
    parser.add_argument('--zero-y-mm', type=finite_number, default=0.0,
                        help='up-positive y reading at the marked steering zero')
    readings = parser.add_mutually_exclusive_group(required=True)
    readings.add_argument('--y-mm', type=finite_number,
                        help='current up-positive y reading on the fixed x=-b line')
    readings.add_argument('--abs-y-mm', type=finite_number,
                          help='nonnegative y magnitude; requires horizontal zero and --position-side')
    parser.add_argument('--position-side', choices=('left', 'right', 'zero'),
                        help='CURRENT position relative to marked steering zero, not last movement direction')
    parser.add_argument('--left-y-sign', choices=('up', 'down'),
                        help='observed y change during a known LEFT movement; omit until calibrated')
    args = parser.parse_args()
    if args.baseline_mm <= 0:
        parser.error('--baseline-mm must be positive')

    if args.abs_y_mm is not None:
        if args.abs_y_mm < 0:
            parser.error('--abs-y-mm must be nonnegative')
        if args.position_side is None:
            parser.error('--abs-y-mm requires --position-side')
        if args.zero_y_mm != 0:
            parser.error('absolute reading mode requires a horizontal marked zero (--zero-y-mm 0)')
        if args.left_y_sign is not None:
            parser.error('--left-y-sign applies only to signed --y-mm readings')
        if args.position_side == 'zero' and args.abs_y_mm != 0:
            parser.error('--position-side zero requires --abs-y-mm 0')
        magnitude_deg = math.degrees(math.atan2(args.abs_y_mm, args.baseline_mm))
        steering_deg = -magnitude_deg if args.position_side == 'right' else magnitude_deg
        print(f'baseline_mm={args.baseline_mm:.3f} zero_y_mm=0.000 abs_y_mm={args.abs_y_mm:.3f} position_side={args.position_side}')
        print(f'steering_left_positive_deg={steering_deg:.3f}')
        print('Manual geometry only; no IMU input or actuator output.')
        return
    if args.position_side is not None:
        parser.error('--position-side applies only to --abs-y-mm readings')

    # The rotating straight mark must pass through the actual shaft centre.
    # Both line orientations must stay on the same arctangent branch.
    current = math.atan2(-args.y_mm, args.baseline_mm)
    zero = math.atan2(-args.zero_y_mm, args.baseline_mm)
    geometric_deg = math.degrees(current - zero)
    print(f'baseline_mm={args.baseline_mm:.3f} zero_y_mm={args.zero_y_mm:.3f} y_mm={args.y_mm:.3f}')
    print(f'line_ccw_from_zero_deg={geometric_deg:.3f}')
    if args.left_y_sign:
        steering_deg = geometric_deg * (-1 if args.left_y_sign == 'up' else 1)
        print(f'steering_left_positive_deg={steering_deg:.3f}')
    else:
        print('steering_left_positive_deg=UNAVAILABLE (LEFT-to-y sign not calibrated)')
    print('Manual geometry only; no IMU input or actuator output.')


if __name__ == '__main__':
    main()
