#include "steering_response.h"
#include <assert.h>
#include <stdio.h>
static void put(const char *path,const char *text){FILE *f=fopen(path,"w");assert(f);fputs(text,f);assert(!fclose(f));}
int main(int argc,char **argv){
    assert(argc==2);const char *p=argv[1];steering_response_model_t m;steering_response_choice_t c;
    put(p,"STEER_RESPONSE_TABLE_V2 2\n"
        "1 30000 10 3 3 2 2 2 -1 1 3 -3 3 0 2.2 2 2 2\n"
        "1 30000 10 3 3 2 2 2 4 6 3 3 9 0 2.2 2 2 2\n");
    assert(steering_response_load(&m,p)==STEER_OK&&m.count==2);
    assert(steering_response_choose(&m,0,5,1,-18,18,60000,3,10,&c)==STEER_OK&&c.duration_us==30000);
    assert(steering_response_choose(&m,2,6,1,-18,18,60000,3,10,&c)==STEER_ERR_NOT_READY); /* unobserved local start */
    assert(steering_response_choose(&m,4,6,1,-18,18,60000,3,10,&c)==STEER_OK);
    assert(steering_response_choose(&m,4,6,1,-18,18,60000,3,0,&c)==STEER_ERR_NOT_READY);
    put(p,"STEER_RESPONSE_TABLE_V2 1\n1 30000 10 3 3 2 2 2 16 17 3 15 21 -0.1 4 2 2 2\n");
    assert(steering_response_load(&m,p)==STEER_OK);
    assert(steering_response_choose(&m,16,18,1,-18,18,60000,3,10,&c)==STEER_ERR_NOT_READY); /* transient exceeds envelope */
    put(p,"STEER_RESPONSE_TABLE_V2 2\n"
        "1 19000 10 3 3 2 2 2 -12.5 -11.5 3 -15 -9 0 2.1 2 2 2\n"
        "2 25000 10 3 3 4 4 4 -11 -9.1 3 -15 -9 -5.5 0 4 4 4\n");
    assert(steering_response_load(&m,p)==STEER_OK);
    assert(steering_response_choose(&m,-12,-15,2,-18,18,60000,3,10,&c)==STEER_ERR_NOT_READY);
    assert(steering_response_choose_reposition(&m,-12,-15,2,-18,18,60000,3,10,&c)==STEER_OK&&c.direction==STEER_LEFT&&c.duration_us==19000);
    assert(steering_response_choose(&m,-10,-15,2,-18,18,60000,3,10,&c)==STEER_OK&&c.direction==STEER_RIGHT);
    put(p,"STEER_RESPONSE_TABLE_V2 3\n"
        "1 19000 10 3 3 3 3 3 -1 1 3 -3 3 0 3.2 3 3 3\n"
        "1 19000 10 9 0 0 0 0 4 6 9 3 9 0 0 0 0 0 0 0 0 0 0 0\n"
        "1 30000 10 3 3 1.5 3 2.1666667 -1 1 3 -3 3 0 3.2 1.5 2 3\n");
    assert(steering_response_load(&m,p)==STEER_OK);
    assert(steering_response_choose(&m,0,10,1,-18,18,60000,3,10,&c)==STEER_OK&&c.duration_us==30000);
    put(p,"STEER_RESPONSE_TABLE_V2 2\n"
        "1 30000 10 3 3 2 2 2 -1 1 3 -3 3 0 2.2 2 2 2\n"
        "1 30000 10 3 3 2 2 2 0 1 3 0 3 0 2.2 2 2 2\n");
    assert(steering_response_load(&m,p)==STEER_ERR_ARGUMENT);
    put(p,"STEER_RESPONSE_TABLE_V3 1\nBURST 200000 7 0 138 220 172 400 6 10 9 13\n"
        "1 30000 10 3 3 2 2 2 -1 1 3 -3 3 0 2.2 2 2 2\n");
    assert(steering_response_load(&m,p)==STEER_OK&&m.burst_max_us==200000&&m.burst_session==7&&m.burst_guard_ms[1]==400);
    put(p,"STEER_RESPONSE_TABLE_V3 1\nBURST 251000 7 0 138 220 172 400 6 10 9 13\n"
        "1 30000 10 3 3 2 2 2 -1 1 3 -3 3 0 2.2 2 2 2\n");
    assert(steering_response_load(&m,p)==STEER_ERR_ARGUMENT);
    put(p,"STEER_RESPONSE_TABLE_V3 1\nBURST 200000 7 0 138 220 172 160 6 10 9 13\n"
        "1 30000 10 3 3 2 2 2 -1 1 3 -3 3 0 2.2 2 2 2\n");
    assert(steering_response_load(&m,p)==STEER_ERR_ARGUMENT);
    put(p,"");assert(steering_response_load(&m,p)==STEER_ERR_ARGUMENT);
    remove(p);puts("PASS C partition table: disjoint current zones, actual-start support, scheduling isolation, transient envelope and malformed-input rejection");return 0;
}
