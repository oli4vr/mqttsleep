/*
  mqttwake.c
  by Olivier Van Rompuy
  oli4vr@gmail.com

  Attempts to wake an mqttsleep process.
  Usage : mqttwake [signal] [process]

  This program will publish an MQTT message with payload "[signal]" on topic
  /IFX-SLEEPER/[process] to attempt to wake (end) the mqttsleep process that
  is subscribed to the topic.

*/
#include <stdio.h>
#include <mosquitto.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define LOGINFO  "/var/log/mqttsleep.info"
#define LOGERROR "/var/log/mqttsleep.err"

char host[64] = "localhost";

void gettime(char *s) {
 time_t t = time(NULL);
 struct tm tm = *localtime(&t);
 sprintf(s,"%04d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

int loginfo(char *msg) {
 char tmp[128];
 FILE *fp;
 gettime(tmp);
 fp=fopen(LOGINFO,"a");
 fprintf(fp,"%s %s",tmp,msg);
 fclose(fp);
}

int logerror(char *msg) {
 char tmp[128];
 FILE *fp;
 gettime(tmp);
 fp=fopen(LOGERROR,"a");
 fprintf(fp,"%s %s",tmp,msg);
 fclose(fp);
}

struct mosquitto *mosq = NULL;
char topic[128]={0};

void mosq_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
 /* Pring all log messages regardless of level. */
  char tmp[256];
  
  switch(level){
    case MOSQ_LOG_WARNING:
    case MOSQ_LOG_ERR: {
      sprintf(tmp,"mqttwake  Topic : %s  --> %i:%s\n", topic,level, str);
      logerror(tmp);
    }
  }
}

void mqtt_setup(char * t){

 char tmp[256];
 int port = 1883;
 int keepalive = 60;
 bool clean_session = true;
 strcpy(topic,t);
  
 mosquitto_lib_init();
 mosq = mosquitto_new(NULL, clean_session, NULL);
 if(!mosq){
  sprintf(tmp, "mqttwake  Topic : %s  --> Error: Out of memory.\n",topic);
  logerror(tmp);
  exit(1);
 }
  
 mosquitto_log_callback_set(mosq, mosq_log_callback);
  
 if(mosquitto_connect(mosq, host, port, keepalive)){
  sprintf(tmp, "mqttwake  Topic : %s  --> Unable to connect.\n",topic);
  logerror(tmp);
  exit(1);
 }
  int loop = mosquitto_loop_start(mosq);
  if(loop != MOSQ_ERR_SUCCESS){
    sprintf(tmp, "mqttwait  Topic : %s  --> Unable to start loop: %i\n", topic,loop);
    logerror(tmp);
    exit(1);
  }
}

int mqtt_send(char *msg){
  return mosquitto_publish(mosq, NULL, topic, strlen(msg), msg, 0, 0);
}

int mqtt_wake(char *procnr, char *trmsg)
{
  char m[128]={0};
  char tmp[256];
  int r;

  sprintf(m,"/IFX-SLEEPER/%s",procnr);

  sprintf(tmp, "mqttwake  Topic : %s  --> Publish message %s to host %s\n", m,trmsg,host);
  loginfo(tmp);

  mqtt_setup(m);

  r=mqtt_send(trmsg);
  usleep(10000);
  r=mqtt_send(trmsg);
  usleep(10000);
  r=mqtt_send(trmsg);
  usleep(10000);
  r=mqtt_send(trmsg);
  usleep(10000);
  r=mqtt_send(trmsg);
  usleep(10000);
  r=mqtt_send(trmsg);
  usleep(10000);
  r=mqtt_send(trmsg);
  usleep(10000);
  r=mqtt_send(trmsg);
  usleep(10000);
  r=mqtt_send(trmsg);
  return r;
}

void printsyntax(char *cmd) {
 fprintf(stderr,"mqttwake      by Olivier Van Rompuy\n");
 fprintf(stderr,"Syntax : %s [-i HostnameOrIP] Signal ID\n\n",cmd);
}

int main(int argc, char *argv[])
{
 if (argc<3) {
	 printsyntax(argv[0]);
	 return -1;
 }

 if (argv[1][0]=='-' && argv[1][1]=='i')
 {
         strcpy(host,argv[2]);
	 argv++;argv++;argc--;argc--;
 }

 if (argc<3) {
	 printsyntax(argv[0]);
	 return -2;
 }

 return mqtt_wake(argv[2],argv[1]);
}

