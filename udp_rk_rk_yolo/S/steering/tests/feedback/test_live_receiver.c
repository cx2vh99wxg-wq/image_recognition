#include "steering_feedback.h"
#include "steering_feedback_wire.h"
#include "steering_platform.h"
#include <assert.h>
#include <stdio.h>
#ifndef _WIN32
#include <arpa/inet.h>
#include <sys/socket.h>
#endif
int main(void)
{
    steering_feedback_t in={7,1,1,-1234,20,0,1000000},out={0};uint8_t wire[48];
    assert(steering_feedback_encode(wire,sizeof(wire),&in)==STEER_OK);
    assert(steering_feedback_ingest(wire,sizeof(wire),steering_now_ns())==STEER_OK);
    assert(steering_feedback_read(&out)==STEER_OK&&out.angle_mdeg==-1234&&out.flags==1);
    assert(steering_feedback_ingest(wire,sizeof(wire),steering_now_ns())==STEER_ERR_STALE);
    wire[24]^=1;assert(steering_feedback_ingest(wire,sizeof(wire),steering_now_ns())!=STEER_OK);
    steering_sleep_ms(110);assert(steering_feedback_read(&out)==STEER_ERR_STALE&&!out.flags);
    steering_feedback_close();in.flags=5;in.sequence++;
    assert(steering_feedback_encode(wire,sizeof(wire),&in)==STEER_OK);
    assert(steering_feedback_ingest(wire,sizeof(wire),steering_now_ns())==STEER_OK);
    assert(steering_feedback_read(&out)==STEER_ERR_STALE&&!out.flags);steering_feedback_close();
#ifndef _WIN32
    int sock=socket(AF_INET,SOCK_DGRAM,0);assert(sock>=0);
    struct sockaddr_in address={0};address.sin_family=AF_INET;address.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
    assert(bind(sock,(struct sockaddr*)&address,sizeof(address))==0);socklen_t length=sizeof(address);
    assert(getsockname(sock,(struct sockaddr*)&address,&length)==0);
    unsigned port=ntohs(address.sin_port);close(sock);
    assert(steering_feedback_open("127.0.0.1","127.0.0.1",port,100)==STEER_OK);
    assert(steering_feedback_open("127.0.0.1","127.0.0.1",port,100)==STEER_ERR_BUSY);
    sock=socket(AF_INET,SOCK_DGRAM,0);assert(sock>=0);in.flags=1;in.sequence++;
    assert(steering_feedback_encode(wire,sizeof(wire),&in)==STEER_OK);
    assert(sendto(sock,wire,48,0,(struct sockaddr*)&address,sizeof(address))==48);
    assert(steering_feedback_read(&out)==STEER_OK&&out.sequence==in.sequence&&out.angle_mdeg==-1234);
    close(sock);steering_feedback_close();
#endif
    puts("PASS live feedback receiver: real signed angles, duplicate/CRC/simulation rejection, expiry, one socket owner");return 0;
}
