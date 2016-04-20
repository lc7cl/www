#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pcap.h>
#include <pthread.h>

#include "config.h"
#include "packet.h"
#include "task.h"
#include "fifo.h"

static int g_stopcap = 0;
static pcap_t *pcap;

#define MAX_TASK_COUNT 5
static struct task* task_array[MAX_TASK_COUNT];
static pthread_t thread_array[MAX_TASK_COUNT];
static struct fifo* pipe_array[MAX_TASK_COUNT];
static int curr;
struct pcap_packet {
    u_int32_t id;
    const struct pcap_pkthdr *pkthdr;
    const char* packet;
};

static int init_pcap(pcap_t **ppcap)
{
    char *dev;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *pcap;
    struct bpf_program filter;  

    if (ppcap == NULL)
        return -1;

    dev = g_config.device;
    if (dev == NULL) {
        dev = pcap_lookupdev(errbuf);
        if (dev == NULL) {
            *ppcap = NULL;
            return -1;
        }    
    }

    pcap = pcap_open_live(dev, 1600, 0, -1, errbuf);
    if (pcap == NULL) {
        *ppcap = NULL;
        return -1;
    }
    if (g_config.filter) {
        if (pcap_compile(pcap, &filter, g_config.filter, 1, 0) == -1)
            return -1;
        if (pcap_setfilter(pcap, &filter) == -1)
            return -1;
    }

    *ppcap = pcap;
    return 0;
}

static void fini_pcap(pcap_t *pcap)
{
    if (pcap)
        pcap_close(pcap);
}

static void* run_task(void* param) 
{
    struct task* t = (struct task*)param;
    struct pcap_packet* pkt;
    dissect_ctx_t ctx;

    while (!t->stop) {
        pkt = (struct pcap_packet*)fifo_get(t->pipe);
        if (pkt) {
            dissect_packet(&ctx, pkt->pkthdr, pkt->packet, t->output);
        } else {
            //log_msg("%s %d\n", __func__, __LINE__);
        }
    }
}

void dispatch(u_char * arg, const struct pcap_pkthdr * pkthdr, const u_char * packet)
{

    struct pcap_packet* pkt;
    int ret;
    static int idx;

    pkt = malloc(sizeof(struct pcap_packet));
    if (pkt == NULL)
        return;
    pkt->pkthdr = pkthdr;
    pkt->packet = packet;
    pkt->id = idx;
    idx++;

    ret = fifo_put(pipe_array[curr], (void*)pkt);
    if (ret == 0) {
        curr++;
        if (curr >= MAX_TASK_COUNT)
            curr = 0;
    } else {
        //log_msg("%s %d\n", __func__, __LINE__);
    }
}

static int start_capture(pcap_t *pcap)
{
    while (!g_stopcap)
        pcap_loop(pcap, -1, dispatch, NULL);
    return 0;
}

static void useage()
{
    printf("-c file   config \n");
    printf("-h help \n");
}

static const char* cfg_file = NULL;
static int parse_cmd(int argc, char** argv)
{
    char ch;
    int ret;

    ret = 0;
    while ((ch = getopt(argc, argv, "c")) != -1) {
    
        switch (ch) {
            case 'c':
                cfg_file = optarg;
                break;
                
            case 'h':
            case '?':
                ret = -1;
                useage();
                break;

            default:
                break;
        }
    }
    return ret;
}

static void deamon(void)
{
    pid_t pid;
    int i = 0, ret;
    char name[512];

    pid = fork();
    if (pid == -1) {
        printf("fork error\n");
        return;
    } 

    if (pid > 0) {
        exit(0);
    } else {
        setsid();
        for (i = 0; i < MAX_TASK_COUNT; i++) {
            pipe_array[i] = fifo_create(10240, sizeof(struct pcap_packet));
            if (pipe_array[i] == NULL)
                exit(-1);
            snprintf(name, 512, "task_%d", i);
            task_array[i] = create_task(name, pipe_array[i], run_task);
            if (task_array[i] == NULL)
                exit(-1);
            ret = pthread_create(&thread_array[i], NULL, task_array[i]->action, task_array[i]);
            if (ret != 0)
                exit(-1);
        }
        start_capture(pcap);
    }
}

int main(int argc, char *argv[])
{

    if (parse_cmd(argc, argv) == -1) {
        return -1;
    }

    if (cfg_file == NULL) {
        cfg_file = "mirror.conf";
    }

    if (parse_cfg(cfg_file) == -1) {
        return -1;
    }

    if (chk_config() == -1) {
        return -1;
    }

    if (init_pcap(&pcap) < 0) {
        printf("failt to init pcap\n");
        return -1;
    }

    deamon();

    close_log();
    fini_pcap(pcap);
    return 0;
}
