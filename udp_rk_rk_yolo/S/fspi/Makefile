# FSPI 车辆控制模块 Makefile
# 独立编译车辆控制程序和静态库
# 作者：辉哥大盗
# 日期：2025年10月20日

# 编译器设置
CC = gcc

# 编译选项
CFLAGS = -Wall -O2 -std=c11

# 项目目录
SRCDIR = csrc
INCDIR = include
BUILDDIR = build

# 目标程序
TARGET_APP = $(BUILDDIR)/vehicle_control

# 源文件（添加共享内存源文件）
FSPI_SOURCES = $(SRCDIR)/fspi_module.c
MAIN_SOURCES = $(SRCDIR)/main.c
SHARED_MEMORY_SOURCES = ../csrc/shared_memory.c

# 对象文件
FSPI_OBJECTS = $(FSPI_SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
MAIN_OBJECTS = $(MAIN_SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
SHARED_MEMORY_OBJECTS = $(SHARED_MEMORY_SOURCES:../csrc/%.c=$(BUILDDIR)/%.o)
ALL_OBJECTS = $(FSPI_OBJECTS) $(MAIN_OBJECTS) $(SHARED_MEMORY_OBJECTS)

# 包含路径（添加上级include目录）
INCLUDES = -I$(INCDIR) -I../include

# 链接库（添加pthread和rt库用于共享内存）
LIBS = -lm -lpthread -lrt

# 编译规则
.PHONY: all app clean help install test

# 默认目标：编译应用程序
all: app

# 编译应用程序
app: $(TARGET_APP)

# 应用程序编译规则
$(TARGET_APP): $(ALL_OBJECTS) | $(BUILDDIR)
	@echo "链接应用程序: $@"
	$(CC) $(ALL_OBJECTS) $(LIBS) -o $@
	@echo "应用程序编译完成: $@"

# C源文件编译规则
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	@echo "编译C文件: $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# 共享内存源文件编译规则
$(BUILDDIR)/%.o: ../csrc/%.c | $(BUILDDIR)
	@echo "编译共享内存文件: $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# 创建目录
$(BUILDDIR):
	@echo "创建build目录"
	mkdir -p $(BUILDDIR)

# 清理编译文件
clean:
	@echo "清理编译文件"
	rm -rf $(BUILDDIR)

# 快速测试编译
test: app
	@echo "=== 编译测试完成 ==="
	@echo "可执行文件: $(TARGET_APP)"
	@if [ -f "$(TARGET_APP)" ]; then \
		echo "编译成功!"; \
		ls -lh $(TARGET_APP); \
	else \
		echo "编译失败!"; \
		exit 1; \
	fi

# 运行程序 (需要管理员权限访问SPI设备)
run: app
	@echo "=== 检查SPI设备 ==="
	@if [ ! -e "/dev/spidev4.0" ]; then \
		echo "警告: SPI设备文件不存在 (/dev/spidev4.0)"; \
		echo "请检查硬件连接和驱动"; \
	else \
		echo "SPI设备文件已存在"; \
	fi
	@echo "=== 运行车辆控制程序 ==="
	@echo "注意: 需要管理员权限访问SPI设备"
	@echo "使用方法: w=前进, s=后退, a=左转, d=右转, x=停止, q=退出"
	@echo "正在启动程序..."
	cd $(BUILDDIR) && sudo ./$(notdir $(TARGET_APP))

# 安装到系统路径 (可选)
install: app
	@echo "安装车辆控制程序到系统"
	sudo mkdir -p /usr/local/bin
	sudo cp $(TARGET_APP) /usr/local/bin/vehicle_control
	@echo "安装完成"

# 卸载
uninstall:
	@echo "卸载车辆控制程序"
	sudo rm -f /usr/local/bin/vehicle_control
	@echo "卸载完成"

# 帮助信息
help:
	@echo "=== FSPI 车辆控制模块 Makefile ==="
	@echo "可用的make目标:"
	@echo "  all     - 编译车辆控制应用程序"
	@echo "  app     - 编译车辆控制应用程序"
	@echo "  run     - 编译并运行应用程序 (需要sudo)"
	@echo "  test    - 编译测试"
	@echo "  clean   - 清理编译文件"
	@echo "  install - 安装到系统 (需要sudo)"
	@echo "  uninstall - 从系统卸载 (需要sudo)"
	@echo "  help    - 显示此帮助信息"
	@echo ""
	@echo "输出文件:"
	@echo "  应用程序: $(TARGET_APP)"
	@echo ""
	@echo "快速开始:"
	@echo "  make run    # 编译并运行车辆控制程序"

# 显示编译信息
info:
	@echo "=== FSPI模块编译信息 ==="
	@echo "应用程序: $(TARGET_APP)"
	@echo "源文件:   $(FSPI_SOURCES) $(MAIN_SOURCES)"
	@echo "包含路径: $(INCLUDES)"
	@echo "编译选项: $(CFLAGS)"
	@echo "链接库:   $(LIBS)"