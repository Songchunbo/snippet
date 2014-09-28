#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>


//#define IPSTR "10.211.55.5"
//#define IPSTR "192.168.7.110"
#define IPSTR "192.168.7.160"

//struct sockaddr {
//    uint8_t       sin_len;
//    sa_family_t   sin_family;
//    char          sa_data[14];
//};
//
//struct sockaddr_in {
//    uint8_t           sin_len;
//    sa_family_t       sin_family;
//    in_port_t         sin_port;
//    struct in_addr    sin_addr;
//    char              sin_zero[8];
//};

//struct in_addr {
//    in_addr_t s_addr;
//}

// int inet_aton(const char* strptr, struct in_addr *addrptr);
// in_addr_t inet_addr(const char* strptr);
// char* inet_ntoa(struct in_addr addr)
//
//

void put32bit(uint8_t **ptr, uint32_t val) {
    (*ptr)[0]=((val)>>24)&0xFF;
    (*ptr)[1]=((val)>>16)&0xFF;
    (*ptr)[2]=((val)>>8)&0xFF;
    (*ptr)[3]=(val)&0xFF;
    (*ptr)+=4;
}


int get32bit(const uint8_t** rptr) {

    //int value = ((*rptr)[3]<<24) + ((*rptr)[2]<<16)+ ((*rptr)[1]<<8) + ((*rptr)[0]) ;
    int value = ((*rptr)[0]<<24) + ((*rptr)[1]<<16)+ ((*rptr)[2]<<8) + ((*rptr)[3]) ;

    (*rptr)+=4;
    return value;
}



uint8_t* make_packet(uint32_t _writeid) {
      uint8_t* buff;

      uint8_t* wptr;
      buff=(uint8_t*)malloc(36+65536);

      uint32_t type=100;
      uint32_t size=65536;
      uint32_t inode=1;
      uint32_t chindx=1;
      uint32_t writeid=1;
      uint32_t pos=128;
      uint32_t from=0;
      uint32_t len=1000;
      uint32_t crc=59773;

      writeid=_writeid;
      wptr=buff;

      put32bit(&wptr, type);
      put32bit(&wptr, size);
      put32bit(&wptr, inode);
      put32bit(&wptr, chindx);
      put32bit(&wptr, writeid);
      put32bit(&wptr, pos);
      put32bit(&wptr, from);
      put32bit(&wptr, len);
      put32bit(&wptr, crc);

      memset(wptr, '1', 65536);
      return buff;
}

int main(int argc, char* argv[]) {
    int nwrite;
    int i;
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    uint32_t ip = inet_addr(IPSTR);
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(struct sockaddr_in));
    sa.sin_family=AF_INET;
    sa.sin_port=htons(9429);
    sa.sin_addr.s_addr=ip;


    uint8_t* buf;

    if (0==connect(fd, (const struct sockaddr*)&sa, sizeof(sa))) {
        printf("connect to 10.211.55.5 success.\n");
    }


    for (i=0; i < 200000; ++i) {
        buf=make_packet(i);
        //usleep(1000); // sleep 10ms
        nwrite= write(fd, buf, 36+65536);
        if (nwrite > 0) {
            printf("%d bytes is written.writeid=%d\n", nwrite, i);
        }
        else {
            printf("written failed nwrite=%d, %s:%d\n", nwrite, strerror(errno), errno);
        }


        free(buf);
    }

    close(fd);



    return 0;
}
