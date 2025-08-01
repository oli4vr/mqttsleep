/*
  mqttsleep.c
  by Olivier Van Rompuy
  olivier.van.rompuy@iriscorporate.com

  Usage : mqttsleep [signal] [process]

  Practically it will subscribe to the MQTT topic /IFX-SLEEPER/[process] and
  wait until a message with payload "[signal]" is received. When this happens
  the program will terminate.

*/
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include <mosquitto.h>

#define mqtt_host "localhost"
#define mqtt_port 1883

#define KEEPALIVE 28800

#define LOGINFO  "/var/log/mqttsleep.info"
#define LOGERROR "/var/log/mqttsleep.err"

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

static int run = 1;
char topic[128]={0};
char trmsg[128]={0};

static int sigrec=0;

void handle_signal(int s)
{
 sigrec=s;
 run = 0;
}

void bogus_signal(int s)
{
 run = 1;
}

void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
 char tmp[256];
 bool match = 0;

 mosquitto_topic_matches_sub(topic, message->topic, &match);
 if (match) {
  if (strcmp(trmsg,(char*) message->payload)==0) {
   run=0;
   sprintf(tmp,"mqttsleep Topic : %s  --> Match message %s\n",topic,(char*) message->payload);
   loginfo(tmp);
  } else {
   sprintf(tmp,"mqttsleep Topic : %s  --> Received message %s  --> But waiting for %s\n",topic,(char*) message->payload,trmsg);
   loginfo(tmp);
  }
 }

}

int mqtt_waitfor(char * t, char *tm)
{
 uint8_t reconnect = true;
 char clientid[24];
 struct mosquitto *mosq;
 int rc = 0;
 char tmp[256];

 strcpy(topic,t);
 strcpy(trmsg,tm);

 signal(SIGINT, bogus_signal);
 signal(SIGTERM, handle_signal);
 signal(SIGALRM, handle_signal);

 sprintf(tmp,"mqttsleep Topic : %s  --> Start waiting for message %s\n",topic,trmsg);
 loginfo(tmp);

 mosquitto_lib_init();

 memset(clientid, 0, 24);
 snprintf(clientid, 23, "mqttsleep_%d", getpid());
 mosq = mosquitto_new(clientid, true, 0);

 if(mosq){
  mosquitto_message_callback_set(mosq, message_callback);

     rc = mosquitto_connect(mosq, mqtt_host, mqtt_port, KEEPALIVE);

  mosquitto_subscribe(mosq, NULL, topic, 0);

  while(run){
   rc = mosquitto_loop(mosq, -1, 1);
   if(run && rc){
    sprintf(tmp,"mqttsleep Topic : %s  --> Connection error!\n",topic);
    logerror(tmp);
    sleep(1);
    if (mosquitto_reconnect(mosq)==MOSQ_ERR_SUCCESS) {
     sprintf(tmp,"mqttsleep Topic : %s  --> Reconnected\n",topic);
     logerror(tmp);
     mosquitto_subscribe(mosq, NULL, topic, 0);
    }
   }
  }
  mosquitto_destroy(mosq);
 }

 mosquitto_lib_cleanup();
 if (sigrec) {
  sprintf(tmp,"mqttsleep Topic : %s  --> Terminating because of received signal %d\n",topic,sigrec);
  logerror(tmp);
 }
 sprintf(tmp,"mqttsleep Topic : %s  --> Stopped waiting for message %s\n",topic,trmsg);
 loginfo(tmp);

 return rc;
}

int main(int argc, char *argv[])
{
 char m[128];
 if (argc<3) {
  fprintf(stderr,"mqttwake      by Olivier Van Rompuy (Westpole Belgium)\n");
  fprintf(stderr,"Syntax : %s Signal ID\n\n",argv[0]);
  return -1;
 }
 sprintf(m,"/IFX-SLEEPER/%s",argv[2]);
 mqtt_waitfor(m,argv[1]);

 return 0;
}

