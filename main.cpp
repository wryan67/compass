#include <stdio.h> 
#include <stdint.h>
#include <math.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <log4pi.h>

using namespace std;
using namespace common;
using namespace common::utility;
using namespace common::synchronized;

Logger logger{"main"};

int compassHandle;
int compassAddress=0x1e;

int idRegA = 10;
int idRegB = 11;
int idRegC = 12;

int confRegA  = 0;
int confRegB  = 1;
int modeReg   = 2;
int statusReg = 9;

int dataXMSB = 3;
int dataXLSB = 4;
int dataZMSB = 5;
int dataZLSB = 6;
int dataYMSB = 7;
int dataYLSB = 8;




int setup() {

    if (wiringPiSetup() != 0) {
        fprintf(stderr, "Wiring Pi could not be initialized\n");
        return 9;
    }

    compassHandle=wiringPiI2CSetup(compassAddress);
 
    if (compassHandle<0) {
      fprintf(stderr,"compass i2c could not be initialzed at address 0x%02x\n", compassAddress);
      return 9;
    }

    return 0;
}

int main(int argc, char ** argv) {
  char buf[32];
  int rc;
  
  if (rc=setup()) {
    return rc;
  }

  buf[0]=idRegA;
  write(compassHandle, buf, 1);
  read(compassHandle,  buf, 3);

  int idA = buf[0];
  int idB = buf[1];
  int idC = buf[2];


  if (idA!='H' || idB!='4' || idC!='3') {
    logger.error("compass identification failed: %02x %02x %02x", idA, idB, idC);
    return 9;
  } else { 
    logger.info("compass identification: %c%c%c", idA, idB, idC);
  }

  logger.info("initialization complete");

  while (true) {
    usleep(100*1000);
    
    buf[0]=idRegA;
    buf[1]=0x70;
    write(compassHandle, buf, 2);

    buf[0]=idRegB;
    buf[1]=0xa0;
    write(compassHandle, buf, 2);

    buf[0]=modeReg;
    buf[1]=0x01;
    write(compassHandle, buf, 2);

    usleep(6*1000);

    read(compassHandle, buf, 6);


    int32_t x = buf[0]<<8 | buf[1];
    int32_t z = buf[2]<<8 | buf[3];
    int32_t y = buf[4]<<8 | buf[5];

    int tmp=y;
    y=-x;
    x=tmp;

    // float heading=atan2(x, y)/0.0174532925;
    // if(heading < 0) heading+=360;
    // heading=360-heading; // N=0/360, E=90, S=180, W=270

   int atan2val = 180/M_PI * atan2((float)(x),(float)(y));
   float heading = (-atan2val + 360 ) % 360;

    printf("x=%05d y=%05d z=%05d heading=%.0f\n", x,y,z, heading);

  }
}
