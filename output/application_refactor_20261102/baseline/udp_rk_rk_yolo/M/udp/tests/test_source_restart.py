#!/usr/bin/env python3
"""Linux/WSL only: compile real M IPC/sender against private test keys.
No board, network socket or production shared segment is opened.
"""
from pathlib import Path
import os
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
base = 0x62000000 + ((os.getpid() & 0xFFFF) << 4)
source = r'''
#define main production_sender_main
#include "SENDER_SOURCE"
#undef main
#include <cassert>
#include <sys/wait.h>
struct Producer { pid_t pid; int command, reply; };
static Producer start_producer(unsigned char value, uint32_t curve_id) {
    int commands[2], replies[2]; assert(!pipe(commands) && !pipe(replies));
    pid_t pid = fork(); assert(pid >= 0);
    if (!pid) {
        close(commands[1]); close(replies[0]);
        SharedMemoryManager writer;
        assert(!shared_memory_init(&writer, SHM_TYPE_PCIE, 1));
        assert(!curve_detection_shm_init(1));
        std::vector<uint8_t> frame(PCIE_FRAME_SIZE, value);
        assert(!pcie_shared_memory_write_frame(&writer, frame.data(), 640, 480));
        assert(!curve_detection_update_source(curve_id, CURVE_LEFT, 80, 11, monotonic_ns()-100000000ULL));
        char ready='R'; assert(write(replies[1], &ready, 1)==1);
        char command;
        while (read(commands[0], &command, 1)==1) {
            if (command=='Q') break;
            if (command=='K') { assert(!sem_wait(writer.image_sem)); assert(write(replies[1], &ready, 1)==1); }
            if (command=='R') {
                for (uint32_t id=100; id<40000; ++id)
                    assert(!curve_detection_update_source(id, (CurveType)(id%4), id%101, id%361, 900000000000ULL+id));
                assert(write(replies[1], &ready, 1)==1);
            }
        }
        curve_detection_shm_cleanup(); shared_memory_cleanup(&writer);
        _exit(0);
    }
    close(commands[0]); close(replies[1]); char ready;
    assert(read(replies[0], &ready, 1)==1);
    return Producer{pid,commands[1],replies[0]};
}
static void stop_producer(Producer p, bool crash) {
    if(crash) {
        char command='K', ready;
        assert(write(p.command,&command,1)==1 && read(p.reply,&ready,1)==1);
        assert(!kill(p.pid,SIGKILL));
    } else { char command='Q'; assert(write(p.command,&command,1)==1); }
    int status; assert(waitpid(p.pid,&status,0)==p.pid);
    if(!crash) assert(WIFEXITED(status)&&WEXITSTATUS(status)==0);
    close(p.command);close(p.reply);
}
static void check_image(SourceConnection& source, uint8_t value) {
    std::vector<uint8_t> out(PCIE_FRAME_SIZE);uint32_t w,h,id;
    assert(!pcie_shared_memory_read_frame(&source.image,out.data(),&w,&h,&id));
    assert(w==640&&h==480&&id==1);
    for(uint8_t byte:out)assert(byte==value);
    assert(pcie_shared_memory_read_frame(&source.image,out.data(),&w,&h,&id)==1);
}
int main() {
    assert(sizeof(SharedMemoryControl)==104&&offsetof(PCIeSharedMemory,image_data)==104);
    assert(sizeof(CurveDetectionSharedMemory)==72);
    NetworkManager net={};net.session_id=100;net.wire_sequence=54321;
    SourceConnection source;
    auto first=start_producer(0x11,11);
    uint64_t now=monotonic_ns();probe_source(source,net,now);assert(source.attached&&source.curve_attached);
    auto generation=source.generation;auto session=net.session_id;check_image(source,0x11);
    uint32_t id,confidence,angle;CurveType type;uint64_t capture;
    assert(!curve_detection_read_source(&id,&type,&confidence,&angle,&capture)&&id==11&&confidence==80&&capture);
    stop_producer(first,false);
    now+=200000000ULL;probe_source(source,net,now);assert(!source.attached);
    auto second=start_producer(0x22,1);
    now+=200000000ULL;probe_source(source,net,now);assert(source.attached&&source.generation!=generation&&net.session_id!=session);
    generation=source.generation;session=net.session_id;check_image(source,0x22);
    assert(!curve_detection_read_source(&id,&type,&confidence,&angle,&capture)&&id==1);
    stop_producer(second,true); // old reader still owns a permanently locked, unlinked semaphore.
    auto third=start_producer(0x33,1);
    now+=200000000ULL;probe_source(source,net,now);assert(source.attached&&source.generation!=generation&&net.session_id!=session);
    check_image(source,0x33);assert(net.wire_sequence==54321); // no source id reset leaks to wire ordering.
    session=net.session_id;now+=200000000ULL;probe_source(source,net,now);assert(net.session_id==session);
    char command='R';assert(write(third.command,&command,1)==1);unsigned coherent=0,contended=0;
    for(unsigned attempt=0;attempt<200000;++attempt) {
        if(curve_detection_read_source(&id,&type,&confidence,&angle,&capture)) { ++contended;continue; }
        if(id<100)continue;
        assert((uint32_t)type==id%4&&confidence==id%101&&angle==id%361&&capture==900000000000ULL+id);
        ++coherent;
    }
    char ready;assert(read(third.reply,&ready,1)==1);assert(coherent);
    stop_producer(third,false);detach_source(source);
    printf("PASS producer normal/crash restart, same-SHM new semaphore, continuous wire sequence, NN coherent=%u bounded_contention=%u ABI104/72\n",coherent,contended);
}
'''
with tempfile.TemporaryDirectory(prefix="fpga-source-restart-") as tmp:
    path = Path(tmp)
    header = (root / "include/shared_memory.h").read_text()
    (path / "shared_memory.h").write_text(header.replace("#define SHM_KEY_BASE 0x12345678", "#define SHM_KEY_BASE " + hex(base)))
    (path / "test.cpp").write_text(source.replace("SENDER_SOURCE", str(root / "udp/csrc/main.cpp")))
    try:
        subprocess.run(["gcc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror", "-pthread", "-I"+tmp,
                        "-c", str(root / "csrc/shared_memory.c"), "-o", tmp+"/shared.o"], check=True)
        subprocess.run(["g++", "-std=c++11", "-O2", "-Wall", "-Wextra", "-Werror", "-pthread",
                        "-I"+tmp, "-I"+str(root/"udp/include"), tmp+"/test.cpp", tmp+"/shared.o", "-lrt",
                        "-o", tmp+"/test"], check=True)
        subprocess.run([tmp+"/test"], check=True, timeout=20)
    finally:
        for offset in (1, 2, 6):
            subprocess.run(["ipcrm", "-M", hex(base+offset)], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        for kind, offset in (("PCIe", 1), ("UDP", 2)):
            semaphore = Path("/dev/shm") / ("sem.M_%s_image_%08X" % (kind, base+offset))
            semaphore.unlink(missing_ok=True)
