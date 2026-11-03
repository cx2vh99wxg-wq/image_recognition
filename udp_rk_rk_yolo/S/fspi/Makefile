# 说明：FSPI 车辆控制模块 Makefile
# 独立构建车辆控制程序和静态库
# 说明：作者：科研轮转
# 说明：本版更新：2026年9月28日

# 选择 C 和 C++ 工具链
CC = gcc
.DEFAULT_GOAL := all


# 传给编译器的参数
CFLAGS = -Wall -O2 -std=c11 -MMD -MP

# 工程目录
SRCDIR = csrc
INCDIR = include
BUILDDIR = build

$(BUILDDIR)/test_traffic_go: test_traffic_go.c ../include/traffic_go_rule.h ../include/traffic_light_ipc.h ../include/traffic_light.h
	@mkdir -p $(BUILDDIR)
	$(CC) -std=c11 -Wall -Wextra -Werror -I../include $< -o $@

test-traffic-go: $(BUILDDIR)/test_traffic_go
	./$(BUILDDIR)/test_traffic_go

$(BUILDDIR)/test_traffic_go_monitor: test_traffic_go_monitor.c $(SRCDIR)/main.c $(SRCDIR)/fspi_module.c ../include/traffic_go_rule.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -Wextra -ffunction-sections -fdata-sections $(INCLUDES) $< -Wl,--gc-sections -o $@

test-traffic-go-monitor: $(BUILDDIR)/test_traffic_go_monitor
	./$(BUILDDIR)/test_traffic_go_monitor

$(BUILDDIR)/test_speed_transport: test_speed_transport.c $(SRCDIR)/fspi_module.c $(INCDIR)/fspi_module.h ../include/speed_status.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -Wextra -ffunction-sections -fdata-sections $(INCLUDES) $< -Wl,--gc-sections -o $@

test-speed: $(BUILDDIR)/test_speed_transport
	./$(BUILDDIR)/test_speed_transport

$(BUILDDIR)/test_control_status: test_control_status.c $(SRCDIR)/fspi_module.c $(INCDIR)/fspi_module.h ../include/control_status.h ../include/speed_status.h
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -Wextra -ffunction-sections -fdata-sections $(INCLUDES) $< -Wl,--gc-sections -o $@

test-control-status: $(BUILDDIR)/test_control_status
	./$(BUILDDIR)/test_control_status

# 生成的应用名称
TARGET_APP = $(BUILDDIR)/vehicle_control

$(BUILDDIR)/test_lane_rule: test_lane_rule.c ../include/lane_rule.h ../include/line_style_ipc.h ../include/line_style.h
	@mkdir -p $(BUILDDIR)
	$(CC) -std=c11 -Wall -Wextra -Werror -I../include $< -o $@

test-lane-rule: $(BUILDDIR)/test_lane_rule
	./$(BUILDDIR)/test_lane_rule
TARGET_TEST = $(BUILDDIR)/test_brake_control
TARGET_SWITCH_TEST = $(BUILDDIR)/test_auto_brake_switch
TARGET_LANE_TEST = $(BUILDDIR)/test_lane_steering
TARGET_TRANSPORT_TEST = $(BUILDDIR)/test_smdt_transport

# 说明：源文件（添加共享内存源文件）
FSPI_SOURCES = $(SRCDIR)/fspi_module.c
MAIN_SOURCES = $(SRCDIR)/main.c
SHARED_MEMORY_SOURCES = ../csrc/shared_memory.c

# 说明：对象文件
FSPI_OBJECTS = $(FSPI_SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
MAIN_OBJECTS = $(MAIN_SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
SHARED_MEMORY_OBJECTS = $(SHARED_MEMORY_SOURCES:../csrc/%.c=$(BUILDDIR)/%.o)
STEERING_NAMES = steering_controller steering_response steering_keepalive steering_runtime
STEERING_OBJECTS = $(addprefix $(BUILDDIR)/controller_,$(addsuffix .o,$(STEERING_NAMES))) $(BUILDDIR)/feedback_steering_feedback.o $(BUILDDIR)/feedback_feedback_tracker.o $(BUILDDIR)/steering_actuator.o $(BUILDDIR)/steering_wire.o
ALL_OBJECTS = $(FSPI_OBJECTS) $(MAIN_OBJECTS) $(SHARED_MEMORY_OBJECTS) $(STEERING_OBJECTS)

# 说明：包含路径（添加上级include目录）
INCLUDES = -I$(INCDIR) -I../include -I../steering/controller -I../steering/actuator -I../steering/feedback -I../../common/steering/contracts

# 说明：链接库（添加pthread和rt库用于共享内存）
LIBS = -lm -lpthread -lrt

# 各类输入的产物生成步骤
.PHONY: all app clean help install test

# 默认目标：构建应用程序
all: app

$(BUILDDIR)/test_spi_owner: test_spi_owner.c $(SRCDIR)/fspi_module.c | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@
test-spi-owner: $(BUILDDIR)/test_spi_owner
	"$(abspath $(BUILDDIR)/test_spi_owner)"

# 构建应用程序
app: $(TARGET_APP) $(BUILDDIR)/speed_gear_status $(BUILDDIR)/control_link_status

$(BUILDDIR)/control_link_status: $(SRCDIR)/control_link_status.c $(BUILDDIR)/fspi_module.o ../include/control_status.h
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCDIR)/control_link_status.c $(BUILDDIR)/fspi_module.o $(LIBS) -o $@

$(BUILDDIR)/speed_gear_status: $(SRCDIR)/speed_gear_status.c $(BUILDDIR)/fspi_module.o
	$(CC) $(CFLAGS) $(INCLUDES) $^ $(LIBS) -o $@

# 应用程序构建规则
$(TARGET_APP): $(ALL_OBJECTS) | $(BUILDDIR)
	@echo "生成应用程序文件: $@"
	$(CC) $(ALL_OBJECTS) $(LIBS) -o $@
	@echo "应用程序构建结束: $@"

# C源文件构建规则
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	@echo "构建C文件: $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# 共享内存源文件构建规则
$(BUILDDIR)/%.o: ../csrc/%.c ../../common/perception/include/curve_shm_attach.h | $(BUILDDIR)
	@echo "构建共享内存文件: $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/controller_%.o: ../steering/controller/%.c $(wildcard ../steering/controller/*.h) | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
$(BUILDDIR)/feedback_%.o: ../steering/feedback/%.c $(wildcard ../steering/feedback/*.h) | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
$(BUILDDIR)/steering_actuator.o: ../steering/actuator/steering_actuator.c ../steering/actuator/steering_actuator.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
$(BUILDDIR)/steering_wire.o: ../../common/steering/contracts/steering_feedback_wire.c | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/main.o: ../include/camera_views.h $(INCDIR)/fspi_module.h $(INCDIR)/brake_policy.h ../include/shared_memory.h ../include/lane_rule.h ../include/traffic_go_rule.h ../include/traffic_light_ipc.h ../include/traffic_light.h ../include/speed_status.h $(wildcard ../steering/controller/*.h)
$(BUILDDIR)/fspi_module.o: $(INCDIR)/fspi_module.h ../include/speed_status.h ../include/control_status.h

# 建立目录
$(BUILDDIR):
	@echo "建立build目录"
	mkdir -p $(BUILDDIR)

# 移除构建文件
clean:
	@echo "移除构建文件"
	rm -rf $(BUILDDIR)

# 行人制动策略验证无需 SPI 设备。
$(TARGET_TEST): test_brake_control.c $(INCDIR)/brake_policy.h | $(BUILDDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) $< -o $@

$(TARGET_SWITCH_TEST): test_auto_brake_switch.c $(SRCDIR)/main.c $(SRCDIR)/fspi_module.c $(INCDIR)/fspi_module.h $(INCDIR)/brake_policy.h ../include/shared_memory.h | $(BUILDDIR)
	$(CC) $(CFLAGS) -Wextra -ffunction-sections -fdata-sections $(INCLUDES) $< -Wl,--gc-sections -o $@

$(TARGET_LANE_TEST): test_lane_steering.c $(SRCDIR)/main.c $(SRCDIR)/fspi_module.c $(INCDIR)/fspi_module.h $(INCDIR)/brake_policy.h ../include/shared_memory.h | $(BUILDDIR)
	$(CC) $(CFLAGS) -Wextra -ffunction-sections -fdata-sections $(INCLUDES) $< -Wl,--gc-sections -o $@

$(TARGET_TRANSPORT_TEST): test_smdt_transport.c $(SRCDIR)/fspi_module.c $(INCDIR)/fspi_module.h | $(BUILDDIR)
	$(CC) $(CFLAGS) -Wextra $(INCLUDES) $< -o $@

test: app $(TARGET_TEST) $(TARGET_SWITCH_TEST) $(TARGET_LANE_TEST) $(TARGET_TRANSPORT_TEST)
	@echo "=== 构建验证完成 ==="
	@echo "程序文件: $(TARGET_APP)"
	@if [ -f "$(TARGET_APP)" ]; then \
		echo "构建通过!"; \
		ls -lh $(TARGET_APP); \
	else \
		echo "构建未通过!"; \
		exit 1; \
	fi
	"$(abspath $(TARGET_TEST))"
	"$(abspath $(TARGET_SWITCH_TEST))"
	"$(abspath $(TARGET_LANE_TEST))"
	"$(abspath $(TARGET_TRANSPORT_TEST))"

# 运行应用 (需要管理员权限访问SPI设备)
run: app
	@echo "=== 说明：检查SPI设备 ==="
	@if [ ! -e "/dev/spidev4.0" ]; then \
		echo "注意: SPI设备文件不存在 (/dev/spidev4.0)"; \
		echo "请核对硬件接入和驱动"; \
	else \
		echo "说明：SPI设备文件已存在"; \
	fi
	@echo "=== 说明：运行车辆控制程序 ==="
	@echo "说明：注意: 需要管理员权限访问SPI设备"
	@echo "操作方法: w=前进, s=后退, a=左转, d=右转, x=结束, q=退出"
	@echo "开始运行应用..."
	cd $(BUILDDIR) && sudo ./$(notdir $(TARGET_APP))

# 说明：安装到系统路径 (可选)
install: app
	@echo "说明：安装车辆控制程序到系统"
	sudo mkdir -p /usr/local/bin
	sudo cp $(TARGET_APP) /usr/local/bin/vehicle_control
	@echo "说明：安装完成"

# 说明：卸载
uninstall:
	@echo "说明：卸载车辆控制程序"
	sudo rm -f /usr/local/bin/vehicle_control
	@echo "说明：卸载完成"

# 用法信息
help:
	@echo "=== 说明：FSPI 车辆控制模块 Makefile ==="
	@echo "说明：可用的make目标:"
	@echo "  all     - 构建车辆控制应用程序"
	@echo "  app     - 构建车辆控制应用程序"
	@echo "  run     - 构建并运行应用程序 (需要sudo)"
	@echo "  test    - 构建验证"
	@echo "  clean   - 移除构建文件"
	@echo "  说明：install - 安装到系统 (需要sudo)"
	@echo "  说明：uninstall - 从系统卸载 (需要sudo)"
	@echo "  help    - 呈现此用法信息"
	@echo ""
	@echo "说明：输出文件:"
	@echo "  说明：应用程序: $(TARGET_APP)"
	@echo ""
	@echo "说明：快速开始:"
	@echo "  make run    # 编译并运行车辆控制程序"

# 呈现构建信息
info:
	@echo "=== FSPI模块构建信息 ==="
	@echo "说明：应用程序: $(TARGET_APP)"
	@echo "说明：源文件:   $(FSPI_SOURCES) $(MAIN_SOURCES)"
	@echo "说明：包含路径: $(INCLUDES)"
	@echo "构建选项: $(CFLAGS)"
	@echo "说明：链接库:   $(LIBS)"

# 说明：Shared headers are authoritative; rebuild their consumers when they change.
-include $(ALL_OBJECTS:.o=.d)
