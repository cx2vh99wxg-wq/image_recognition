#!/bin/bash
# Sourced by both launchers. Prefer an in-tree board build over staged bin/.
# A failed rebuild must not silently run the previous display implementation.
bin_compatible() {
    case "$2" in
        planning_main|udp_m_send_main)
            local info
            # Older programs ignore unknown arguments and would start running.
            LC_ALL=C grep -aFq 'ADAS-6CH-v2' "$1" || return 1
            info="$(timeout 3 "$1" --build-info 2>/dev/null)" || return 1
            if [ -n "${REQUIRED_BUILD_FEATURE:-}" ]; then
                [[ "$info" == *"$REQUIRED_BUILD_FEATURE"* ]] || return 1
            fi
            [[ "$info" == *"ADAS-6CH-v2 layout=960x480"* ]] ;;
        *) return 0 ;;
    esac
}

bin_fresh() {
    local bin="$1" mod="$2" newer
    [ -x "$bin" ] || return 1
    # Deployments may contain binaries only. For a source checkout include
    # Makefiles and the shared PCIe capture implementation as dependencies.
    local paths=() p
    for p in "$REPO_ROOT/$mod/csrc" "$REPO_ROOT/$mod/include" "$REPO_ROOT/$mod/Makefile" \
             "$REPO_ROOT/common/csrc" "$REPO_ROOT/common/include" \
             "$REPO_ROOT/perception/csrc/pcie_capture.c" "$REPO_ROOT/perception/include"; do
        [ ! -e "$p" ] || paths+=("$p")
    done
    [ ${#paths[@]} -gt 0 ] || return 0
    newer="$(find "${paths[@]}" -type f \( -name '*.c' -o -name '*.h' -o -name Makefile \) -newer "$bin" -print -quit)"
    [ -z "$newer" ]
}

resolve_bin() {
    local name="$1" mod="$2" cand target=bin
    [ "$name" != udp_m_send_main ] || target=sender
    for cand in "$REPO_ROOT/$mod/$name" "$BIN_DIR/$name"; do
        [ -x "$cand" ] || continue
        bin_compatible "$cand" "$name" || continue
        if bin_fresh "$cand" "$mod" || [ "$NO_BUILD" = 1 ]; then
            printf '%s\n' "$cand"; return 0
        fi
    done
    if [ "$NO_BUILD" = 1 ]; then
        echo "错误：$name 不存在或不支持 ADAS-6CH-v2；请重新 make -C $mod bin。" >&2
        return 1
    fi
    echo "  编译当前源码：make -C $mod $target" >&2
    if ! make -C "$REPO_ROOT/$mod" "$target" >&2; then
        echo "错误：编译失败，停止启动；不会回退到旧版程序。" >&2
        return 1
    fi
    cand="$REPO_ROOT/$mod/$name"
    if [ -x "$cand" ] && bin_compatible "$cand" "$name"; then
        printf '%s\n' "$cand"; return 0
    fi
    echo "错误：构建结果缺失或版本不匹配：$cand" >&2
    return 1
}
