#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/input.h>
#include <string.h>


#define pwm_config "/sys/class/CLASS_PWM1/pwm0/configure"
#define pwm_period "/sys/class/CLASS_PWM1/pwm0/period"
#define pwm_ch0    "/sys/class/CLASS_PWM1/pwm0/duty_ch0"
#define pwm_ch1    "/sys/class/CLASS_PWM1/pwm0/duty_ch1"
#define pwm_ch2    "/sys/class/CLASS_PWM1/pwm0/duty_ch2"
#define pwm_ch3    "/sys/class/CLASS_PWM1/pwm0/duty_ch3"

#define dev "/dev/pwm0"

  char buff[100];

  int pwm_config_fd;
  int pwm_period_fd;
  int pwm_ch0_fd;
  int pwm_ch1_fd;
  int pwm_ch2_fd;
  int pwm_ch3_fd;


 int counter;
int main(){
  counter = 0;
  pwm_config_fd = open(pwm_config,O_RDWR | O_ASYNC | O_RDONLY);
  pwm_period_fd = open(pwm_period,O_RDWR | O_ASYNC | O_RDONLY);
  pwm_ch0_fd    = open(pwm_ch0,O_RDWR | O_ASYNC | O_RDONLY);
  pwm_ch1_fd    = open(pwm_ch1,O_RDWR | O_ASYNC | O_RDONLY);
  pwm_ch2_fd    = open(pwm_ch2,O_RDWR | O_ASYNC | O_RDONLY);
  pwm_ch3_fd    = open(pwm_ch3,O_RDWR | O_ASYNC | O_RDONLY);


  printf("FILE OPEN: %d\n\r",pwm_config_fd);
  printf("FILE OPEN: %d\n\r",pwm_period_fd);
  printf("FILE OPEN: %d\n\r",pwm_ch0_fd);
  printf("FILE OPEN: %d\n\r",pwm_ch1_fd);
  printf("FILE OPEN: %d\n\r",pwm_ch2_fd);
  printf("FILE OPEN: %d\n\r",pwm_ch3_fd);

  write(pwm_config_fd,"1",32);
  read(pwm_config_fd,&buff,32);
  printf("PWM CONFIG: %s\n",buff);

  write(pwm_period_fd,"1000",32);
  read(pwm_period_fd,&buff,32);
  printf("PWM PERIOD: %s\n",buff);

  write(pwm_ch0_fd,"0",32);
  read(pwm_ch0_fd,&buff,32);
  printf("PWM CH0: %s\n",buff);

  write(pwm_ch1_fd,"10",10);
  read(pwm_ch1_fd,&buff,10);
  printf("PWM CH1: %s\n",buff);

  write(pwm_ch2_fd,"200",32);
  read(pwm_ch2_fd,&buff,32);
  printf("PWM CH2: %s\n",buff);

  write(pwm_ch3_fd,"999",32);
  read(pwm_ch3_fd,&buff,32);
  printf("PWM CH3: %s\n",buff);

  write(pwm_config_fd,"0x1E",32);
  read(pwm_config_fd,&buff,32);
  printf("PWM CONFIG: %s\n",buff);


/*
while(1){


  sprintf(buff,"%d",counter);
  //sprintf(counter, buff, 10);
  write(pwm_ch0_fd,buff,10);
  sleep(0.8);
  if(counter > 231) counter = 0;
  counter++;

  }*/
}
