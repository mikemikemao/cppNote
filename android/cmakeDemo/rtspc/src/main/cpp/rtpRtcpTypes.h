#ifndef __RTP_RTCP_TYPES_H__
#define __RTP_RTCP_TYPES_H__

#define lit_endian 0x0123
#define big_endian 0x3210
#define vod_byte_order lit_endian
#define RTP_VERSION 2

#define RTP_SEQ_MOD     (1 << 16)
#define RTP_MAX_SDES    255
#define MAX_DROPOUT     3000
#define MAX_MISORDER    100
#define MIN_SEQUENTIAL  2

enum rtcp_type
{
    RTCP_SR     = 200,
    RTCP_RR     = 201,
    RTCP_SDES   = 202,
    RTCP_BYE    = 203,
    RTCP_APP    = 204
};

enum rtcp_sdes_type
{
    RTCP_SDES_END	= 0,
    RTCP_SDES_CNAME = 1,
    RTCP_SDES_NAME	= 2,
    RTCP_SDES_EMAIL = 3,
    RTCP_SDES_PHONE = 4,
    RTCP_SDES_LOC	= 5,
    RTCP_SDES_TOOL	= 6,
    RTCP_SDES_NOTE	= 7,
    RTCP_SDES_PRIV	= 8
};

struct rtp_hdr
{
#if vod_byte_order == big_endian
    unsigned char version   : 2;
    unsigned char padding   : 1;
    unsigned char extension : 1;
    unsigned char count     : 4;
    unsigned char marker    : 1;
    unsigned char payload   : 7;
#elif vod_byte_order == lit_endian
    unsigned char count     : 4;		
    unsigned char extension : 1;
    unsigned char padding   : 1;
    unsigned char version   : 2;
    unsigned char payload   : 7;	
    unsigned char marker    : 1;
#endif

    unsigned short sequence;
    unsigned int timestamp;
    unsigned int ssrc;
};

struct rtcp_common
{
#if vod_byte_order == big_endian
    unsigned char version : 2;
    unsigned char padding : 1;
    unsigned char count   : 5;
#elif vod_byte_order == lit_endian
    unsigned char count   : 5;
    unsigned char padding : 1;
    unsigned char version : 2;
#endif

    unsigned char type;
    unsigned short length;
};

struct rtp_ext_hdr
{
    unsigned short id;
    unsigned short len;
};

struct rtcp_report
{
    unsigned int ssrc;
    unsigned int fraction :8;
    int lost : 24;
    unsigned int last_seq;
    unsigned int jitter;
    unsigned int lsr;
    unsigned int dlsr;
};

struct rtcp_rr
{
    unsigned int ssrc;
    rtcp_report report[1];
};

struct rtcp_sr 
{
    unsigned int ssrc;
    unsigned int ntp_sec;
    unsigned int ntp_frac;
    unsigned int rtp_ts;
    unsigned int psent;
    unsigned int osent;
    rtcp_report  report[1];
};

struct rtcp_sdes_item
{
    unsigned char type;
    unsigned char length;
    unsigned char data[RTP_MAX_SDES+1];
};

struct rtcp_sdes
{
    unsigned int ssrc;
    rtcp_sdes_item item[1];
};

struct rtcp_bye
{
    unsigned int ssrc[1];
};

struct rtcp_app 
{
    unsigned int ssrc;
    char name[4];
    unsigned char app_data[1];
};

struct rtcp_packet
{
    rtcp_common common;
    union {
            rtcp_sr sr;
            rtcp_rr rr;
            rtcp_sdes sdes;
            rtcp_bye bye;
            rtcp_app app;
    } r;
};

struct rtp_source
{
    unsigned short max_seq;
    unsigned int cycles;
    unsigned int base_seq;
    unsigned int bad_seq;
    unsigned int probation;
    unsigned int received;
    unsigned int expected_prior;
    unsigned int received_prior;
    unsigned int transit;
    unsigned int jitter;
};


struct rtcp_info
{
    unsigned int ssrc;
    rtp_source src;
    rtcp_sr sr;
    rtcp_rr to;
    rtcp_rr from;
    rtcp_sdes_item cname;	

    unsigned int   tLSRmyTime;
};


#endif

