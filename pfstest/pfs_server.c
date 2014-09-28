
//gcc -I`pwd`/include -I`pwd`/memory -I`pwd`/util -I`pwd`/thread -I`pwd`/io -I`pwd`/packet/http -I. -c libeasytest.c -o libeasytest.o
//gcc libeasytest.o .libs/libeasy.a -lpthread -lm -o libeasytest
#include <easy_io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "pfs_alloc.h"


#define LOG_FILE "/root/test.log"

easy_thread_pool_t *work_task_queue;
easy_thread_pool_t *slow_work_task_queue;

void easy_log_print_test(const char *message)
{
    int fd = open(LOG_FILE, O_CREAT|O_RDWR|O_APPEND, 0777);
    if (fd<0) {
        return;
    }

    int nwrite=write(fd, message, strlen(message));


    close(fd);
}



int easy_io_initialize();
int easy_io_old() ;
int get32bit(const uint8_t** rptr);

int main(int argc, char* argv[]){

    //easy_io_old() ;
    easy_io_initialize();


    return 0;
}

static int io_thread_count=4;
static int work_thread_count=4;


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

void* decode_cb(easy_message_t *m) {
    const uint8_t* rptr;
    uint8_t* wptr;
    easy_buf_t *eb;

    int len;
    void *packet;


    if (NULL == m) {
        easy_fatal_log("m is NULL");
        return NULL;
    }

    if (NULL == m->input) {
        easy_fatal_log("m->input is NULL");
        return NULL;
    }

    if (m->input->last - m->input->pos < 36) {
//        easy_fatal_log("last=%p, pos=%p, len=%u",
//            m->input->last, m->input->pos, m->input->last-m->input->pos);
        return NULL;
    }

    if ((len=(m->input->last-m->input->pos)) >= 36) {
        rptr=m->input->pos+4;
        int datalen=get32bit(&rptr);

        rptr=m->input->pos+16;
        int writeid=get32bit(&rptr);

        rptr=m->input->pos+8;
        int inode=get32bit(&rptr);


//        easy_fatal_log("last=%p, pos=%p, len=%u, writeid=%d, datalen=%d",
//            m->input->last, m->input->pos, m->input->last-m->input->pos, writeid, datalen);

        m->input->pos += 36;

        if (m->input->last - m->input->pos < datalen) {
            m->next_read_len = (datalen) - (m->input->last - m->input->pos);
            m->input->pos -=36;
//            easy_fatal_log("need read more data next_read_len=%d", m->next_read_len);
            return NULL;
        }

        // make packet
        // alloc packet
        packet = easy_pool_calloc(m->pool, (4+4+4+datalen));
        if (NULL == packet) {
            m->status = EASY_ERROR;
            return NULL;
        }

        wptr=packet;
        put32bit(&wptr, datalen);
        put32bit(&wptr, writeid);
        put32bit(&wptr, inode);
        easy_fatal_log("in decode_cb easy_pool_calloc m=%p, m->pool=%p, packet=%p, inode=%d,writeid=%d,datalen=%d",
            m,m->pool,packet,inode,writeid,datalen);
        memcpy(wptr, m->input->pos, datalen);

        m->input->pos += datalen;
//        easy_fatal_log("decode_cb message len = %d, writeid = %d \n", len, writeid);
    }

    //assert(m->input->pos <= m->input->last);



    return packet;
}


int encode_cb(easy_request_t *r, void *packet) {

    return EASY_OK;
}


int process_cb(easy_request_t *r) {
    const uint8_t* rptr;
    int len;
    int writeid;
    char filename[256];
    uint8_t* packet = (uint8_t*)r->ipacket;
    if (NULL == packet || r->ms->status==EASY_MESG_DESTROY) {
        return EASY_ERROR;
    }


    easy_connection_t *c = r->ms->c;
    if (c) {
        easy_fatal_log("in process_cb push to r=%p, r->ipacket=%p, c=%p, c->status=%d", r, r->ipacket, r->ms->c, c->status);
    }
    else {
        easy_fatal_log("in process_cb push to r=%p, r->ipacket=%p, c=%p", r, r->ipacket, r->ms->c);
    }

    easy_thread_pool_push(work_task_queue, r, easy_hash_key((uint64_t)(long)r));
    return EASY_AGAIN;
}


int slow_packet_handler(void *packet) {
    const uint8_t* rptr;
    int len;
    int writeid,inode;
    char filename[256];
    if (NULL==packet) {
        easy_fatal_log("in slow_packet_handler packet is NULL");
       return EASY_ERROR;
    }

    rptr=packet;
    len=get32bit(&rptr);
    writeid=get32bit(&rptr);
    inode=get32bit(&rptr);

    int i=(writeid%4+1);
    sprintf(filename, "/data%d/64K_%08d",i, writeid);
    int fd = open(filename, O_CREAT|O_RDWR|O_APPEND, 0777);

    int nwrite = write(fd, packet, len+8);

    if (nwrite > 0) {
        easy_fatal_log("slow_packet_handler packet=%p len=%d, inode=%d writeid=%d, %d bytes is written to %s success",
            packet,len, inode,writeid, nwrite,filename);
    }
    else {
        easy_fatal_log("slow_packet_handler packet=%p len=%d, inode=%d writeid=%d, %d bytes is written to %s failed",
            packet,len, inode, writeid, nwrite,filename);
    }

    //easy_fatal_log("free packet=%p", packet);
    //free(packet);
    close(fd);
    return EASY_OK;
}

//int worker_request_handler(easy_request_t *r, void *args) {
//    const
    //return process_cb(r);
//}

int worker_request_cb(easy_request_t *r, void *args) {
    //return worker_request_handler(r, args);
    ((void)(args));

easy_fatal_log("in worker_request_cb r=%p, r->ipacket=%p", r, r->ipacket);
    if (NULL==r|| r->ipacket==NULL) {
        easy_fatal_log("NULL==r NULL==r->ipacket");
        return EASY_ERROR;
    }
    void* packet = r->ipacket;

    //easy_fatal_log("entering in worker_request_cb r=%p, r->ipacket=%p", r, r->ipacket);
    return slow_packet_handler(packet);
}

uint64_t get_packet_id_cb(easy_connection_t *c, void *packet) {
    return EASY_OK;
}

int clean_cb(easy_request_t *r, void *packet) {
    if (r==NULL && packet==NULL) {
        return EASY_OK;
    }

#if 0
    easy_fatal_log("clean_cb r=%p, r->ipacket=%p, r->opacket=%p, packet=%p", r, r->ipacket,r->opacket,packet);
    if (packet!=NULL) {
        free(packet);
        packet=NULL;
    }
    if (r!=NULL) {
        if (r->ipacket!=NULL) {
            free(r->ipacket);
            r->ipacket=NULL;
        }
    }

    if (r!=NULL) {
        if (r->ipacket!=NULL) {
            free(r->ipacket);
            r->ipacket=NULL;
        }

        if (r->opacket!=NULL) {
            free(r->opacket);
            r->opacket=NULL;
        }
    }
#endif
    return EASY_OK;
}


int on_connect_cb(easy_connection_t *c) {
    // disconnect when idle for 15min
    c->idle_time = 15 * 60 * 1000;
    //c->life_idle = 1;

    return EASY_OK;
}

int on_disconnect_cb(easy_connection_t *c) {
    return EASY_OK;
}

int on_idle_cb(easy_connection_t *c) {
    return EASY_ERROR;
}

int easy_io_initialize() {
    easy_io_t eio;
    easy_io_handler_pt eio_handler;


    init_pfs_alloc();
    easy_pool_set_allocator(easy_pool_pfs_realloc);


    memset(&eio, 0, sizeof(eio));

    memset(&eio_handler, 0, sizeof(eio_handler));


    easy_log_set_print(easy_log_print_test);

    eio_handler.cleanup=clean_cb;
    eio_handler.on_connect=on_connect_cb;
    eio_handler.on_disconnect=on_disconnect_cb;
    eio_handler.on_idle=on_idle_cb;

    eio_handler.get_packet_id=get_packet_id_cb;
    eio_handler.encode=encode_cb;
    eio_handler.decode=decode_cb;
    eio_handler.process=process_cb;

    easy_io_create(&eio, io_thread_count);

    //eio.do_signal = 0;
    eio.no_redispatch=0;
    //eio.no_reuseport=1;
    eio.tcp_nodelay=1;
    eio.tcp_cork=0;
    eio.affinity_enable=1;


    easy_io_add_listen(&eio, NULL, 9429, &eio_handler);


    //easy_request_thread_create(&eio, work_thread_count, NULL, NULL);

    work_task_queue=easy_request_thread_create(&eio,work_thread_count,worker_request_cb,NULL);

    //slow_work_task_queue=easy_request_thread_create(&eio,10,NULL,NULL);


    easy_io_start(&eio);


    easy_io_wait(&eio);
    return 0;
}



int easy_io_old() {
    easy_io_handler_pt handler;
    easy_io_t* eio;
    easy_thread_pool_t *task_queue;

    eio=NULL;
    eio=easy_io_create(eio, 10);

    easy_listen_t *listen1 = easy_io_add_listen(eio, NULL, 59773, &handler);
    easy_listen_t *listen2 = easy_io_add_listen(eio, NULL, 59774, &handler);
    easy_listen_t *listen3 = easy_io_add_listen(eio, NULL, 59775, &handler);
    easy_listen_t *listen4 = easy_io_add_listen(eio, NULL, 59776, &handler);
    easy_listen_t *listen5 = easy_io_add_listen(eio, NULL, 59777, &handler);

    easy_io_start(eio);

    easy_io_wait(eio);

    return 0;
}



//(gdb) p work_task_queue
//$1 = (easy_thread_pool_t *) 0x24d5058
//(gdb) p *work_task_queue
//$2 = {thread_count = 10, member_size = 144, last_number = 0, list_node = {next = 0x7fff1c9f96a8, prev = 0x242c588}, next = 0x0, last = 0x24d5628 "", data = 0x24d5058 "\n"}
//(gdb) p &tp[0]
//No symbol "tp" in current context.
//(gdb) p &work_task_queue->data[0]
//$3 = 0x24d5088 "\264FA"
//(gdb) p &work_task_queue->data[144]
//$4 = 0x24d5118 "\264FA"
//(gdb) p *(easy_request_thread_t*)&work_task_queue->data[144]
//$5 = {on_start = 0x4146b4 <easy_baseth_on_start>, tid = 140719513790208, idx = 1, iot = 0, loop = 0x24d5fd0, thread_watcher = {active = 1, pending = 0, priority = 0, data = 0x24d5118, cb = 0x4130ed <easy_request_on_wakeup>, sent = 0},
//  thread_lock = 0, eio = 0x7fff1c9f9660, task_list_count = 0, task_list = {next = 0x24d5178, prev = 0x24d5178}, session_list = {next = 0x24d5188, prev = 0x24d5188}, process = 0x402c12 <worker_request_cb>, args = 0x0}
//(gdb) p task_list_count
//No symbol "task_list_count" in current context.
//(gdb) p *(easy_request_thread_t*)&work_task_queue->data[0]
//$6 = {on_start = 0x4146b4 <easy_baseth_on_start>, tid = 140719524280064, idx = 0, iot = 0, loop = 0x24d5660, thread_watcher = {active = 1, pending = 0, priority = 0, data = 0x24d5088, cb = 0x4130ed <easy_request_on_wakeup>, sent = 0},
//  thread_lock = 0, eio = 0x7fff1c9f9660, task_list_count = 0, task_list = {next = 0x24d50e8, prev = 0x24d50e8}, session_list = {next = 0x24d50f8, prev = 0x24d50f8}, process = 0x402c12 <worker_request_cb>, args = 0x0}
//(gdb) p *(easy_request_thread_t*)&work_task_queue->data[2*144]
//$7 = {on_start = 0x4146b4 <easy_baseth_on_start>, tid = 140719503300352, idx = 2, iot = 0, loop = 0x24d6940, thread_watcher = {active = 1, pending = 0, priority = 0, data = 0x24d51a8, cb = 0x4130ed <easy_request_on_wakeup>, sent = 0},
//  thread_lock = 0, eio = 0x7fff1c9f9660, task_list_count = 0, task_list = {next = 0x24d5208, prev = 0x24d5208}, session_list = {next = 0x24d5218, prev = 0x24d5218}, process = 0x402c12 <worker_request_cb>, args = 0x0}
//(gdb) p *(easy_request_thread_t*)&work_task_queue->data[3*144]
//$8 = {on_start = 0x4146b4 <easy_baseth_on_start>, tid = 140719492810496, idx = 3, iot = 0, loop = 0x24d72b0, thread_watcher = {active = 1, pending = 0, priority = 0, data = 0x24d5238, cb = 0x4130ed <easy_request_on_wakeup>, sent = 0},
//  thread_lock = 0, eio = 0x7fff1c9f9660, task_list_count = 0, task_list = {next = 0x24d5298, prev = 0x24d5298}, session_list = {next = 0x24d52a8, prev = 0x24d52a8}, process = 0x402c12 <worker_request_cb>, args = 0x0}
//(gdb) p *(easy_request_thread_t*)&work_task_queue->data[4*144]
//$9 = {on_start = 0x4146b4 <easy_baseth_on_start>, tid = 140719482320640, idx = 4, iot = 0, loop = 0x24d7c20, thread_watcher = {active = 1, pending = 0, priority = 0, data = 0x24d52c8, cb = 0x4130ed <easy_request_on_wakeup>, sent = 0},
//  thread_lock = 0, eio = 0x7fff1c9f9660, task_list_count = 0, task_list = {next = 0x24d5328, prev = 0x24d5328}, session_list = {next = 0x24d5338, prev = 0x24d5338}, process = 0x402c12 <worker_request_cb>, args = 0x0}
//(gdb) p *(easy_request_thread_t*)&work_task_queue->data[5*144]
//$10 = {on_start = 0x4146b4 <easy_baseth_on_start>, tid = 140719471830784, idx = 5, iot = 0, loop = 0x24d8790, thread_watcher = {active = 1, pending = 0, priority = 0, data = 0x24d5358, cb = 0x4130ed <easy_request_on_wakeup>, sent = 0},
//  thread_lock = 0, eio = 0x7fff1c9f9660, task_list_count = 0, task_list = {next = 0x24d53b8, prev = 0x24d53b8}, session_list = {next = 0x24d53c8, prev = 0x24d53c8}, process = 0x402c12 <worker_request_cb>, args = 0x0}
//(gdb) p *(easy_request_thread_t*)&work_task_queue->data[6*144]
//$11 = {on_start = 0x4146b4 <easy_baseth_on_start>, tid = 140719461340928, idx = 6, iot = 0, loop = 0x24d9300, thread_watcher = {active = 1, pending = 0, priority = 0, data = 0x24d53e8, cb = 0x4130ed <easy_request_on_wakeup>, sent = 0},
//  thread_lock = 0, eio = 0x7fff1c9f9660, task_list_count = 0, task_list = {next = 0x24d5448, prev = 0x24d5448}, session_list = {next = 0x24d5458, prev = 0x24d5458}, process = 0x402c12 <worker_request_cb>, args = 0x0}
//(gdb) p *(easy_request_thread_t*)&work_task_queue->data[7*144]
//$12 = {on_start = 0x4146b4 <easy_baseth_on_start>, tid = 140719450851072, idx = 7, iot = 0, loop = 0x24d9e70, thread_watcher = {active = 1, pending = 0, priority = 0, data = 0x24d5478, cb = 0x4130ed <easy_request_on_wakeup>, sent = 0},
//  thread_lock = 0, eio = 0x7fff1c9f9660, task_list_count = 0, task_list = {next = 0x24d54d8, prev = 0x24d54d8}, session_list = {next = 0x24d54e8, prev = 0x24d54e8}, process = 0x402c12 <worker_request_cb>, args = 0x0}
//(gdb) p *(easy_request_thread_t*)&work_task_queue->data[8*144]
//$13 = {on_start = 0x4146b4 <easy_baseth_on_start>, tid = 140719440361216, idx = 8, iot = 0, loop = 0x24da9e0, thread_watcher = {active = 1, pending = 0, priority = 0, data = 0x24d5508, cb = 0x4130ed <easy_request_on_wakeup>, sent = 0},
//  thread_lock = 0, eio = 0x7fff1c9f9660, task_list_count = 0, task_list = {next = 0x24d5568, prev = 0x24d5568}, session_list = {next = 0x24d5578, prev = 0x24d5578}, process = 0x402c12 <worker_request_cb>, args = 0x0}
//(gdb) p *(easy_request_thread_t*)&work_task_queue->data[9*144]
//$14 = {on_start = 0x4146b4 <easy_baseth_on_start>, tid = 140719429871360, idx = 9, iot = 0, loop = 0x24db550, thread_watcher = {active = 1, pending = 0, priority = 0, data = 0x24d5598, cb = 0x4130ed <easy_request_on_wakeup>, sent = 0},
//  thread_lock = 0, eio = 0x7fff1c9f9660, task_list_count = 0, task_list = {next = 0x24d55f8, prev = 0x24d55f8}, session_list = {next = 0x24d5608, prev = 0x24d5608}, process = 0x402c12 <worker_request_cb>, args = 0x0}
