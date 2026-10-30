#include "imu_tool.h"
#include "imu_source.h"
#include "steering_angle.h"
#include "imu_stationary.h"
#include "steering_platform.h"
#include "steering_feedback_wire.h"
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <arpa/inet.h>
#include <sys/socket.h>
#endif
static volatile sig_atomic_t cancelled,center_requested,relative_zero_requested;
static void cancel_handler(int n){(void)n;cancelled=1;}
#ifndef _WIN32
static void center_handler(int n){(void)n;center_requested=1;}
static void relative_zero_handler(int n){(void)n;relative_zero_requested=1;}
#endif
static int number(const char *s,unsigned *out){
 char *e;errno=0;if(!*s||*s=='-')return 0;
 unsigned long n=strtoul(s,&e,10);if(errno||*e||n>1000000)return 0;*out=(unsigned)n;return 1;
}
static int real_number(const char *s,float *out){
 char *e;errno=0;float n=strtof(s,&e);if(errno||!*s||*e||!isfinite(n))return 0;*out=n;return 1;
}
static int load_calibration(const char *name,float axis[3],float bias[3]){
 FILE *f=fopen(name,"r");if(!f)return 0;
 char magic[16];int ok=fscanf(f,"%15s\naxis %f %f %f\nbias %f %f %f",magic,
  &axis[0],&axis[1],&axis[2],&bias[0],&bias[1],&bias[2])==7&&!strcmp(magic,"IMUCAL1");
 fclose(f);
 for(unsigned i=0;i<3;++i)if(!isfinite(axis[i])||!isfinite(bias[i])||fabsf(bias[i])>10)ok=0;
 return ok;
}
static void usage(void){
 puts("imu_dump / angle_monitor [--backend mock|fpga] [--samples N (0=continuous)]\n"
 " [--interval-ms N] [--print-every N] [--csv NEW_FILE] [--wait-device]\n"
 " [--axis x|y|z] [--axis-sign 1|-1] [--mock-rate-dps N]\n"
 " [--calibration IMUCAL1_FILE] [--fixed-chassis --zero-at-center]\n"
 " [--gravity-gain 0..0.1] [--send-to IPv4 --port 54321]\n"
 " [--stationary-hold] (bench: hold current angle after ~2s stationary evidence)\n"
 "Defaults: MOCK, 100 samples, 10ms, Z axis, no physical zero, no UDP.\n"
 "REAL FPGA: 100Hz sensor; fixed FPGA clock offset; duplicate reads are skipped.\n"
 "Only --fixed-chassis + calibration + --zero-at-center can assert ZERO_VALID.\n"
 "SIGUSR1 re-establishes center only in that explicitly armed bench mode.\n"
 "SIGUSR2 sets the current relative angle to zero; revokes ZERO_VALID, changes session.\n"
 "A single IMU does not separate vehicle motion from steering rotation.");
}
static float gravity_angle(const float reference[3],const float measured[3],const float axis[3]){
 float a[3],b[3],da=0,db=0;
 for(unsigned k=0;k<3;++k){da+=reference[k]*axis[k];db+=measured[k]*axis[k];}
 double na=0,nb=0,dot=0;
 for(unsigned k=0;k<3;++k){a[k]=reference[k]-da*axis[k];b[k]=measured[k]-db*axis[k];na+=a[k]*a[k];nb+=b[k]*b[k];dot+=a[k]*b[k];}
 if(na<9||nb<9)return NAN; /* rotation axis too close to gravity: unobservable */
 double cross=(b[1]*a[2]-b[2]*a[1])*axis[0]+(b[2]*a[0]-b[0]*a[2])*axis[1]+(b[0]*a[1]-b[1]*a[0])*axis[2];
 return (float)(atan2(cross,dot)*180/3.14159265358979323846);
}
int imu_tool_main(int argc,char **argv,int estimate){
 const char *backend="mock",*axis_name="z",*csv_name=NULL,*cal_name=NULL,*target=NULL;
 unsigned count=100,interval=10,print_every=10,port=54321;
 float mock_rate=0,sign=1,gravity_gain=0;
 int fixed_chassis=0,request_zero=0,wait_device=0,stationary_hold=0;
 for(int i=1;i<argc;++i){
  const char *key=argv[i];
  if(!strcmp(key,"--help")){usage();return 0;}
  if(!strcmp(key,"--fixed-chassis")){fixed_chassis=1;continue;}
  if(!strcmp(key,"--zero-at-center")){request_zero=1;continue;}
  if(!strcmp(key,"--wait-device")){wait_device=1;continue;}
  if(!strcmp(key,"--stationary-hold")){stationary_hold=1;continue;}
  if(i+1==argc){usage();return 2;}
  const char *v=argv[++i];
  if(!strcmp(key,"--backend"))backend=v;
  else if(!strcmp(key,"--axis"))axis_name=v;
  else if(!strcmp(key,"--csv"))csv_name=v;
  else if(!strcmp(key,"--calibration"))cal_name=v;
  else if(!strcmp(key,"--send-to"))target=v;
  else if(!strcmp(key,"--samples")){if(!number(v,&count))return 2;}
  else if(!strcmp(key,"--interval-ms")){if(!number(v,&interval))return 2;}
  else if(!strcmp(key,"--print-every")){if(!number(v,&print_every))return 2;}
  else if(!strcmp(key,"--port")){if(!number(v,&port))return 2;}
  else if(!strcmp(key,"--axis-sign")){if(!strcmp(v,"-1"))sign=-1;else if(strcmp(v,"1"))return 2;}
  else if(!strcmp(key,"--gravity-gain")){if(!real_number(v,&gravity_gain))return 2;}
  else if(!strcmp(key,"--mock-rate-dps")){if(!real_number(v,&mock_rate))return 2;}
  else {usage();return 2;}
 }
 int fpga=!strcmp(backend,"fpga");
 int idx=!strcmp(axis_name,"x")?0:!strcmp(axis_name,"y")?1:!strcmp(axis_name,"z")?2:-1;
 if((!fpga&&strcmp(backend,"mock"))||idx<0||!interval||interval>1000||!print_every||
   fabsf(mock_rate)>2000||(fpga&&mock_rate!=0)||!port||port>65535||gravity_gain<0||gravity_gain>.1f||
   (request_zero&&(!estimate||!fpga||!fixed_chassis||!cal_name))||
   (gravity_gain>0&&!request_zero)||(target&&!estimate)||
   (stationary_hold&&(!estimate||gravity_gain>0))){usage();return 2;}
 float axis[3]={0},bias[3]={0};axis[idx]=sign;
 if(cal_name&&!load_calibration(cal_name,axis,bias)){fprintf(stderr,"Invalid calibration file\n");return 2;}
 steering_angle_t integrator;
 if(steering_angle_init(&integrator,axis,bias,100)!=STEER_OK)return 2;
 imu_stationary_t hold;imu_stationary_init(&hold);
#ifndef _WIN32
 int sock=-1;
 if(target){
  struct sockaddr_in dst={0};dst.sin_family=AF_INET;dst.sin_port=htons((uint16_t)port);
  if(inet_pton(AF_INET,target,&dst.sin_addr)!=1)return 2;
  sock=socket(AF_INET,SOCK_DGRAM|SOCK_CLOEXEC,0);
  if(sock<0||connect(sock,(struct sockaddr*)&dst,sizeof(dst))){if(sock>=0)close(sock);return 1;}
 }
#else
 if(target){fprintf(stderr,"UDP sender runs on M/Linux\n");return 2;}
#endif
 imu_source_t source;
 steer_result_t rc=imu_source_open(&source,fpga?IMU_SOURCE_FPGA:IMU_SOURCE_MOCK,mock_rate);
 if(rc!=STEER_OK){
  fprintf(stderr,"FPGA/MPU unavailable: %s; no measured data produced.\n",steer_result_name(rc));
#ifndef _WIN32
  if(sock>=0)close(sock);
#endif
  return 1;
 }
 FILE *csv=NULL;
 if(csv_name){
  csv=steering_create_csv(csv_name);
  if(!csv){perror("Create CSV (new filename required)");imu_source_close(&source);return 1;}
  fputs("backend,session,seq,sample_time_ns,flags,ax_mps2,ay_mps2,az_mps2,gx_dps,gy_dps,gz_dps,temp_c,relative_deg,axis_rate_dps,zero_valid,dt_ms,seq_gap,source_age_ms,static_hold\n",csv);
  setvbuf(csv,NULL,_IOFBF,65536);
 }
 cancelled=0;center_requested=0;relative_zero_requested=0;
 signal(SIGINT,cancel_handler);signal(SIGTERM,cancel_handler);
#ifndef _WIN32
 struct sigaction action={0};sigemptyset(&action.sa_mask);action.sa_flags=SA_RESTART;
 action.sa_handler=center_handler;
 int signal_error=sigaction(SIGUSR1,&action,NULL);
 action.sa_handler=relative_zero_handler;
 if(signal_error||sigaction(SIGUSR2,&action,NULL)){
  perror("Install zero handlers");imu_source_close(&source);
  if(csv)fclose(csv);
  if(sock>=0)close(sock);
  return 1;
 }
#endif
 setvbuf(stdout,NULL,_IOLBF,0);
 printf("[%s] axis=[%.5f %.5f %.5f] bias=[%.5f %.5f %.5f] ZERO_VALID=%d; %s\n",
  fpga?"REAL FPGA":"SIMULATED MOCK",integrator.axis[0],integrator.axis[1],integrator.axis[2],bias[0],bias[1],bias[2],request_zero,
  fixed_chassis?"fixed-chassis bench declared":"relative motion only, chassis uncompensated");
 unsigned produced=0,duplicate_reads=0;uint64_t last_good=steering_now_ns(),next_sample=last_good,last_notice=0,previous_stamp=0;
 uint32_t previous_seq=0,session=(uint32_t)last_good^(uint32_t)(last_good>>32);
 if(!session)session=1;
 int have_sample=0,zero_valid=request_zero,exit_code=0;float gravity_ref[3]={0};
 for(;!cancelled&&(!count||produced<count);){
  steering_imu_sample_t sample;
  rc=imu_source_read(&source,&sample);
  uint64_t now=steering_now_ns();
  if(rc!=STEER_OK){
   if(rc==STEER_ERR_BUSY)++duplicate_reads;
   if(rc==STEER_ERR_BUSY||rc==STEER_ERR_NOT_READY||rc==STEER_ERR_IO){
    if(now-last_good>(have_sample?100000000u:2000000000u)){
     if(have_sample||!wait_device){fprintf(stderr,"IMU fault/stale: %s; ZERO_VALID revoked, no valid UDP; re-zero required.\n",steer_result_name(rc));exit_code=1;break;}
     if(now-last_notice>2000000000u){
      fpga_imu_snapshot_t status;uint64_t capture;
      if(mpu9250_fpga_snapshot(&source.bus,&status,&capture)==STEER_OK)
       fprintf(stderr,"WAIT_SENSOR who=%02X flags=%u i2c_error=%u errors=%u; no angle/valid feedback\n",status.identity,status.flags,status.error,status.errors);
      else fprintf(stderr,"WAIT_SENSOR no coherent FPGA snapshot; no valid feedback\n");
      last_notice=now;
     }
    }
    steering_sleep_ms(1);continue;
   }
   fprintf(stderr,"IMU discontinuity: %s; no valid UDP, re-zero required.\n",steer_result_name(rc));exit_code=1;break;
  }
  if(center_requested){
   center_requested=0;
   if(request_zero){
    (void)steering_angle_init(&integrator,axis,bias,100);have_sample=0;previous_stamp=0;
    if(++session==0)session=1;
    zero_valid=1;
    puts("MECHANICAL CENTER reset requested; new feedback session");
   }else puts("Center reset ignored: calibrated fixed-chassis mode was not armed");
  }
  double relative=NAN;float projected=NAN;int did_relative_zero=0;
  if(estimate){
   rc=stationary_hold ? steering_angle_update_with_hold(&integrator,&hold,&sample,&relative,&projected) :
       steering_angle_update(&integrator,&sample,&relative,&projected);
   if(rc!=STEER_OK){fprintf(stderr,"Integration fault: %s; zero lost\n",steer_result_name(rc));exit_code=1;break;}
   if(!have_sample)memcpy(gravity_ref,sample.accel_mps2,sizeof(gravity_ref));
   if(gravity_gain>0){
    double norm=0;for(unsigned k=0;k<3;++k)norm+=sample.accel_mps2[k]*sample.accel_mps2[k];
    float g_angle=gravity_angle(gravity_ref,sample.accel_mps2,integrator.axis);
    if(isfinite(g_angle)&&fabs(sqrt(norm)-9.80665)<.3&&fabsf(projected)<30){
     double nearest=g_angle+360*round((relative-g_angle)/360);
     integrator.angle_deg+=gravity_gain*(nearest-relative);relative=integrator.angle_deg;
    }
   }
  }
  if(relative_zero_requested){
   relative_zero_requested=0;
   if(estimate){
    /* Only after a valid integration update: do not hide sequence/clock faults.
     * Preserve sensor sampling, rate, timestamps and continuity state. */
    integrator.angle_deg=0;relative=0;zero_valid=0;gravity_gain=0;
    if(++session==0)session=1;
    did_relative_zero=1;
    printf("RELATIVE ZERO set: seq=%u relative_deg=0.00000 ZERO_VALID=0 session=%u\n",sample.sequence,session);
   }else puts("Relative zero ignored: imu_dump does not estimate angles");
  }
  uint64_t age_ns=fpga?source.captured_age_ns+(now>source.capture_host_ns?now-source.capture_host_ns:0):
    (now>sample.sample_time_ns?now-sample.sample_time_ns:0);
  uint32_t age_ms=(uint32_t)(age_ns/1000000);
  if(age_ms>100){fprintf(stderr,"Sample expired; zero lost\n");exit_code=1;break;}
  unsigned gap=have_sample?(sample.sequence-previous_seq-1u):0;
  double dt=previous_stamp?(sample.sample_time_ns-previous_stamp)/1e6:0;
  if(did_relative_zero||produced%print_every==0||(count&&produced+1==count))
   printf("%s seq=%u t_ns=%" PRIu64 " dt_ms=%.3f gap=%u age_ms=%u accel=[%.4f %.4f %.4f] gyro=[%.4f %.4f %.4f] temp_c=%.3f relative_deg=%.5f rate_dps=%.5f ZERO_VALID=%d static_hold=%d\n",
    fpga?"REAL":"SIMULATED",sample.sequence,sample.sample_time_ns,dt,gap,age_ms,
    sample.accel_mps2[0],sample.accel_mps2[1],sample.accel_mps2[2],sample.gyro_dps[0],sample.gyro_dps[1],sample.gyro_dps[2],sample.temperature_c,relative,projected,zero_valid,stationary_hold&&hold.holding);
  if(csv){
   fprintf(csv,"%s,%u,%u,%" PRIu64 ",%u,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.4f,%.6f,%.6f,%d,%.6f,%u,%u,%d\n",
    backend,session,sample.sequence,sample.sample_time_ns,sample.flags,sample.accel_mps2[0],sample.accel_mps2[1],sample.accel_mps2[2],
    sample.gyro_dps[0],sample.gyro_dps[1],sample.gyro_dps[2],sample.temperature_c,relative,projected,zero_valid,dt,gap,age_ms,stationary_hold&&hold.holding);
   if(ferror(csv)){perror("CSV write");exit_code=1;break;}
  }
#ifndef _WIN32
  if(sock>=0){
   if(!isfinite(relative)||fabs(relative)>2147483||fabsf(projected)>2147483){exit_code=1;break;}
   steering_feedback_t f={0};f.session_id=session;f.sequence=sample.sequence;
   f.flags=sample.flags|(zero_valid?STEER_FEEDBACK_ZERO_VALID:0);
   f.angle_mdeg=(int32_t)llround(relative*1000);f.rate_mdps=(int32_t)lroundf(projected*1000);
   f.source_age_ms=age_ms;f.sample_time_ns=sample.sample_time_ns;
   uint8_t wire[STEERING_FEEDBACK_WIRE_SIZE];
   if(steering_feedback_encode(wire,sizeof(wire),&f)!=STEER_OK){exit_code=1;break;}
   if(send(sock,wire,sizeof(wire),MSG_DONTWAIT)!=(ssize_t)sizeof(wire)&&produced%print_every==0)perror("UDP send");
  }
#endif
  previous_seq=sample.sequence;previous_stamp=sample.sample_time_ns;have_sample=1;last_good=now;++produced;
  next_sample+=(uint64_t)interval*1000000;
  if(now>next_sample+(uint64_t)interval*1000000)next_sample=now;
  while(!cancelled&&steering_now_ns()<next_sample)steering_sleep_ms(1);
 }
 mpu9250_link_stats_t stats;mpu9250_fpga_stats(&source.bus,&stats);
 printf("LINK accepted=%u duplicate_reads=%u transfers=%u io_errors=%u crc_errors=%u stale_captures=%u i2c_errors=%u\n",
  produced,duplicate_reads,stats.transfers,stats.io_errors,stats.crc_errors,stats.stale_captures,stats.i2c_errors);
 imu_source_close(&source);
#ifndef _WIN32
 if(sock>=0)close(sock);
#endif
 if(csv&&fclose(csv))exit_code=1;
 return exit_code?exit_code:cancelled?130:0;
}
