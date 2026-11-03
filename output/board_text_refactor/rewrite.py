from pathlib import Path
import re, sys, base64, json, hashlib

root=Path('udp_rk_rk_yolo')
work=Path(__file__).resolve().parent
board_work=work.parent/'board_source_sync'
literal = re.compile(r'(?P<lead>\becho(?:\s+-[neE]+)*\s+")(?P<body>(?:[^"\\\n]|\\.)*)(?P<end>")')
c_log = re.compile(r'(?m)^(?P<lead>\s*(?:std::)?(?:printf\(\s*|fprintf\(\s*(?:stdout|stderr)\s*,\s*)")(?P<body>(?:[^"\\\n]|\\.)*)(?P<end>")')
protected = re.compile(r'\$\([^)]*\)|\$\{[^}]*\}|\$[\w@<*?]+|\\[a-zA-Z0-9]+(?:\[[0-9;]*[A-Za-z])?|%[-+.\dhljztL]*[A-Za-z%]')
rewrites = [
('欢迎使用','欢迎进入'),('Welcome to','Console for'),('Build System','Build Console'),
('Author:','Maintainer:'),('Date:','Recorded:'),('Time:','Local time:'),('Modules:','Components:'),
('Rendering Support:','Renderer option:'),('YOLO Detection: Enabled','YOLO detector: on'),
('Building','Preparing'),('build complete','build finished'),('Build successful','Build ready'),
('Creating','Preparing'),('Installing','Setting up'),('Installation complete','Installation finished'),
('Testing','Checking'),('Running','Launching'),('Usage:','Invocation:'),('Run:','Start with:'),
('Cleaning','Removing'),('Compiler:','Toolchain:'),('Build Directory:','Build output:'),
('Target:','Build target:'),('Linking:','Writing executable:'),('Compiling','Translating'),
('检测到交互式登录，准备启动','交互终端就绪，即将启动'),('切换到','进入'),
('未找到','未定位到'),('请手动切换到','请先进入'),('然后运行','接着执行'),
('当前工作目录','项目所在目录'),('检测到Makefile，准备启动系统','构建入口已就绪，即将启动'),
('系统将在3秒后自动启动','等待三秒后将执行'),('如需取消，请按','可使用下列按键取消'),
('倒计时','剩余等待'),('秒...','秒…'),('清除行内容','刷新倒计时'),
('正在启动','开始运行'),('请确认您在正确的','请核对当前路径是否为'),('如果需要手动启动，请执行','可按下面的方式手动启动'),
('编译完成','构建结束'),('编译成功','构建通过'),('编译失败','构建未通过'),('编译','构建'),
('链接目标程序','生成目标可执行文件'),('链接应用程序','生成应用可执行文件'),
('启动','运行'),('正在','开始'),('错误:','异常:'),('警告:','注意:'),
('清理','移除'),('创建','建立'),('显示','呈现'),('停止','结束'),('已启动','已开始运行'),
('初始化成功','初始化完成'),('初始化失败','初始化未完成'),('执行失败','执行未完成'),
('成功','已完成'),('失败','未成功'),('使用方法','操作方法'),('控制说明','控制按键'),
('请检查','请核对'),('请确认','请核实'),('当前','此时'),('共享内存','共享内存'),
('可执行文件','程序文件'),('项目目录','工程目录'),('运行程序','运行应用'),
('构建目录','产物目录'),('测试','验证'),('帮助','用法'),('配置','设置'),
('连接','接入'),('检测','识别'),('接收','收取'),('发送','发出'),
]
comment_map={
 '基本配置':'目标、源码及产物路径', '目录配置':'工程路径约定', '文件配置':'参与构建的文件',
 '编译选项':'传给编译器的参数', '编译器设置':'选择 C 和 C++ 工具链', '包含路径':'头文件搜索范围',
 '依赖关系':'跟踪头文件变动', '默认目标':'直接调用 make 时采用的入口',
 '源文件':'选择需要编译的源码', '目标程序':'生成的应用名称', '编译规则':'各类输入的产物生成步骤',
 '清理':'回收编译产物', '安装':'将产物放入安装路径', '测试':'可单独调用的验证入口',
 '~/.bashrc: executed by bash(1) for non-login shells.':'非登录 Bash 会读取此文件以配置会话。',
 "If not running interactively, don't do anything":'仅在交互会话中初始化终端。',
 'append to the history file, don\'t overwrite it':'会话历史追加到已有文件。',
 'Auto-start M-End FPGA System on login':'交互登录后按 M 端流程运行。',
 'Auto-start S-End FPGA System on login':'交互登录后按 S 端流程运行。',
 '开机自动启动M端FPGA系统':'发送端启动段。',
 '开机自动启动S端FPGA系统':'接收端启动段。',
 '检查是否在交互式终端中':'自动运行仅由交互终端触发。',
 '执行make run':'进入 Makefile 的运行入口。',
 '3秒倒计时':'启动前留出三秒取消窗口。',
}
def wording(body, fallback=False):
    if not re.search(r'[A-Za-z\u4e00-\u9fff]',body) or re.fullmatch(r'\s*(?:make|cd|sudo|\.\/).*',body) or body.lstrip().startswith(('[','***','██')):
        return body
    pieces=[]; last=0
    def edit(text):
        for old,new in rewrites:
            text=text.replace(old,new)
        return text
    for m in protected.finditer(body):
        pieces.extend([edit(body[last:m.start()]),m[0]])
        last=m.end()
    pieces.append(edit(body[last:]))
    new=''.join(pieces)
    if fallback and new==body and re.search(r'[\u4e00-\u9fff]',body):
        new=re.sub(r'^(\s*(?:===\s*)?)',r'\1说明：',body,count=1)
    assert protected.findall(new)==protected.findall(body)
    return new

def comments(line):
    m=re.match(r'^(\s*(?:@)?#\s*)(.*?)(\r?\n)?$',line)
    if not m:return line
    body=m[2].strip()
    if not body or re.fullmatch(r'[#=_-]+',body) or body.startswith(('!','BEGIN STEERING','END STEERING')):return line
    if re.match(r'^(?:shopt |export |alias |force_color_prompt=|\[ )',body):return line
    if '$(' in body or line.rstrip().endswith('\\'):return line
    return m[1]+comment_map.get(body,wording(body) if wording(body)!=body else '说明：'+body)+(m[3] or '')

def transform(text, shell=False, main=False):
    out=[]
    for line in text.splitlines(keepends=True):
        if not main:
            new=comments(line)
            if new!=line:
                out.append(new);continue
        matcher=c_log if main else literal
        def adjust(m):
            b=m['body']
            # Shell command substitutions and structured logs keep their exact text.
            if ('$(' in b and ')' not in b) or (main and re.match(r'\s*(?:\[|[A-Z_]+[ :=]|===|\*\*\*)',b)):
                return m[0]
            return m['lead']+wording(b,fallback=not main)+m['end']
        # Only shell echo in recipes/standalone Bash commands.
        if main or (shell and re.match(r'\s*echo\b',line)) or (not shell and line.startswith('\t')):
            line=matcher.sub(adjust,line)
        out.append(line)
    return ''.join(out)

def code_projection(text, main=False):
    regex=c_log if main else literal
    return regex.sub(lambda m:m['lead']+'<message>'+m['end'],
        text if main else re.sub(r'(?m)^\s*(?:@)?#[^\n]*','',text))
files=sorted(root.rglob('Makefile'))+[root/b/'.bashrc' for b in 'MS']+[
    root/p for p in ('M/pcie/csrc/main.cpp','M/udp/csrc/main.cpp','S/pcie/csrc/main.cpp',
                     'S/udp/csrc/main.cpp','S/x11/csrc/main.cpp','S/fspi/csrc/main.c')]
manifest=[]
for path in files:
    rel=path.relative_to(root)
    raw=path.read_bytes()
    source=raw.decode('utf-8').replace('\r\n','\n')
    is_main=path.name in ('main.c','main.cpp')
    new=transform(source,shell=path.name=='.bashrc',main=is_main)
    assert code_projection(source,is_main)==code_projection(new,is_main),rel
    if new==source: continue
    baseline=work/'before'/rel
    baseline.parent.mkdir(parents=True,exist_ok=True)
    baseline.write_bytes(raw)
    path.write_bytes(new.replace('\n','\r\n' if b'\r\n' in raw else '\n').encode())
    manifest.append(rel.as_posix())
    print(rel,sum(a!=b for a,b in zip(source.splitlines(),new.splitlines())))
for side in 'MS':
    received=(board_work/f'active_{side}.txt').read_text(encoding='utf-8')
    raw=base64.b64decode(re.search(r'BASHRC_BEGIN\s+([A-Za-z0-9+/=]+)\s+BASHRC_END',received)[1])
    active=raw.decode('utf-8').replace('\r\n','\n')
    new=transform(active,shell=True)
    assert code_projection(active)==code_projection(new)
    (work/f'active_{side}.before').write_bytes(raw)
    (work/f'active_{side}.after').write_bytes(new.encode())
(work/'manifest.json').write_text(json.dumps(manifest,indent=2),encoding='utf-8')
