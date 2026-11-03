# 非登录 Bash 会读取此文件以配置会话。
# 说明：see /usr/share/doc/bash/examples/startup-files (in the package bash-doc)
# 说明：for examples

# 仅在交互会话中初始化终端。
case $- in
    *i*) ;;
      *) return;;
esac

# 说明：don't put duplicate lines or lines starting with space in the history.
# 说明：See bash(1) for more options
HISTCONTROL=ignoreboth

# 会话历史追加到已有文件。
shopt -s histappend

# 说明：for setting history length see HISTSIZE and HISTFILESIZE in bash(1)
HISTSIZE=1000
HISTFILESIZE=2000

# 说明：check the window size after each command and, if necessary,
# 说明：update the values of LINES and COLUMNS.
shopt -s checkwinsize

# 说明：If set, the pattern "**" used in a pathname expansion context will
# 说明：match all files and zero or more directories and subdirectories.
#shopt -s globstar

# 说明：make less more friendly for non-text input files, see lesspipe(1)
#[ -x /usr/bin/lesspipe ] && eval "$(SHELL=/bin/sh lesspipe)"

# 说明：set variable identifying the chroot you work in (used in the prompt below)
if [ -z "${debian_chroot:-}" ] && [ -r /etc/debian_chroot ]; then
    debian_chroot=$(cat /etc/debian_chroot)
fi

# 说明：set a fancy prompt (non-color, unless we know we "want" color)
case "$TERM" in
    xterm-color|*-256color) color_prompt=yes;;
esac

# 说明：uncomment for a colored prompt, if the terminal has the capability; turned
# 说明：off by default to not distract the user: the focus in a terminal window
# 说明：should be on the output of commands, not on the prompt
#force_color_prompt=yes

if [ -n "$force_color_prompt" ]; then
    if [ -x /usr/bin/tput ] && tput setaf 1 >&/dev/null; then
	# 说明：We have color support; assume it's compliant with Ecma-48
	# 说明：(ISO/IEC-6429). (Lack of such support is extremely rare, and such
	# 说明：a case would tend to support setf rather than setaf.)
	color_prompt=yes
    else
	color_prompt=
    fi
fi

if [ "$color_prompt" = yes ]; then
    PS1='${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$ '
else
    PS1='${debian_chroot:+($debian_chroot)}\u@\h:\w\$ '
fi
unset color_prompt force_color_prompt

# 说明：If this is an xterm set the title to user@host:dir
case "$TERM" in
xterm*|rxvt*)
    PS1="\[\e]0;${debian_chroot:+($debian_chroot)}\u@\h: \w\a\]$PS1"
    ;;
*)
    ;;
esac

# 说明：enable color support of ls and also add handy aliases
if [ -x /usr/bin/dircolors ]; then
    test -r ~/.dircolors && eval "$(dircolors -b ~/.dircolors)" || eval "$(dircolors -b)"
    alias ls='ls --color=auto'
    #alias dir='dir --color=auto'
    #alias vdir='vdir --color=auto'

    #alias grep='grep --color=auto'
    #alias fgrep='fgrep --color=auto'
    #alias egrep='egrep --color=auto'
fi

# 说明：colored GCC warnings and errors
#export GCC_COLORS='error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01'

# 说明：some more ls aliases
#alias ll='ls -l'
#alias la='ls -A'
#alias l='ls -CF'

# 说明：Alias definitions.
# 说明：You may want to put all your additions into a separate file like
# 说明：~/.bash_aliases, instead of adding them here directly.
# 说明：See /usr/share/doc/bash-doc/examples in the bash-doc package.

if [ -f ~/.bash_aliases ]; then
    . ~/.bash_aliases
fi

# 说明：enable programmable completion features (you don't need to enable
# 说明：this, if it's already enabled in /etc/bash.bashrc and /etc/profile
# 说明：sources /etc/bash.bashrc).
if ! shopt -oq posix; then
  if [ -f /usr/share/bash-completion/bash_completion ]; then
    . /usr/share/bash-completion/bash_completion
  elif [ -f /etc/bash_completion ]; then
    . /etc/bash_completion
  fi
fi
if [ -f ~/setup_network.sh ]; then
    sudo bash ~/setup_network.sh
fi
if [ -f ~/pango_pci_driver.ko ]; then
    sudo insmod ~/pango_pci_driver.ko
fi

# 交互登录后按 S 端流程运行。
# 接收端启动段。
echo "======================================"
echo "Console for S-End FPGA System"
echo "======================================"
echo "Maintainer: 科研轮转"
echo "Local time: $(date)"
echo "======================================"

# 自动运行仅由交互终端触发。
if [[ $- == *i* ]]; then
    echo "交互终端就绪，即将运行S端系统..."
    echo ""
    
    # 检查此时目录，如果不在正确目录则自动切换
    if [[ "$(pwd)" != */S ]] && [[ "$(pwd)" != */S/ ]]; then
        echo "进入S端工作目录..."
        cd ~/S 2>/dev/null || cd /home/$(whoami)/S 2>/dev/null || {
            echo "注意: 未定位到S端工作目录 ~/S"
            echo "请手动进入包含Makefile的目录"
            echo "接着执行: make run"
            return
        }
    fi
    
    echo "项目所在目录: $(pwd)"
    
    # 说明：检查Makefile是否存在
    if [[ -f "Makefile" ]]; then
        echo "构建入口已就绪，即将运行..."
        echo ""
        echo "提示: 等待三秒后将执行 'make run'"
        echo "可使用下列按键取消 Ctrl+C"
        echo ""
        
        # 启动前留出三秒取消窗口。
        for i in 3 2 1; do
            echo -n "剩余等待: $i 秒…"
            sleep 1
            echo -e "\r刷新倒计时\033[K"
        done
        
        echo "开始运行S端FPGA系统..."
        echo "======================================"
        
        # 进入 Makefile 的运行入口。
        make run
        
    else
        echo "异常: 此时目录中未定位到Makefile"
        echo "请核对此时路径是否为S端工程目录中"
        echo ""
        echo "可按下面的方式手动运行:"
        echo "  cd path/to/S-End/directory"
        echo "  make run"
    fi
fi