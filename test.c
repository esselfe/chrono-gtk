#include "config.h"
#include <stdio.h>
#include <assert.h>
#include "chrono.h"

unsigned int test;

void Test(void) {
	unsigned int lineno_start = __LINE__;
    assert(ParseTimeToSeconds("0") == 0);
    assert(ParseTimeToSeconds("1") == 1);
    assert(ParseTimeToSeconds("2") == 2);
    assert(ParseTimeToSeconds("3") == 3);
    assert(ParseTimeToSeconds("4") == 4);
    assert(ParseTimeToSeconds("5") == 5);
    assert(ParseTimeToSeconds("10") == 10);
    assert(ParseTimeToSeconds("11") == 11);
    assert(ParseTimeToSeconds("15") == 15);
    assert(ParseTimeToSeconds("20") == 20);
    assert(ParseTimeToSeconds("21") == 21);
    assert(ParseTimeToSeconds("25") == 25);
    assert(ParseTimeToSeconds("59") == 59);
    assert(ParseTimeToSeconds("1:0") == 60);
    assert(ParseTimeToSeconds("1:1") == 61);
    assert(ParseTimeToSeconds("1:02") == 62);
    assert(ParseTimeToSeconds("1:5") == 65);
    assert(ParseTimeToSeconds("1:05") == 65);
    assert(ParseTimeToSeconds("1:10") == 70);
    assert(ParseTimeToSeconds("1:59") == 119);
    assert(ParseTimeToSeconds("2:00") == 120);
    assert(ParseTimeToSeconds("2:01") == 121);
    assert(ParseTimeToSeconds("2:05") == 125);
    assert(ParseTimeToSeconds("2:10") == 130);
    assert(ParseTimeToSeconds("1:00:00") == 3600);
    assert(ParseTimeToSeconds("1:00:01") == 3601);
    assert(ParseTimeToSeconds("1:00:11") == 3611);
    assert(ParseTimeToSeconds("1:01:00") == 3660);
    assert(ParseTimeToSeconds("1:01:01") == 3661);
    assert(ParseTimeToSeconds("1:01:11") == 3671);
    assert(ParseTimeToSeconds("1:01:21") == 60*60 + 60 + 21);
    assert(ParseTimeToSeconds("1:1:00") == 3660);
    assert(ParseTimeToSeconds("1:1:01") == 3661);
    assert(ParseTimeToSeconds("1:1:11") == 3671);
    assert(ParseTimeToSeconds("1:1:10") == 60*60 + 60 + 10);
    assert(ParseTimeToSeconds("2:00:00") == 7200);
    assert(ParseTimeToSeconds("2:00:01") == 7201);
    assert(ParseTimeToSeconds("2:00:10") == 7210);
    assert(ParseTimeToSeconds("2:00:11") == 7211);
    assert(ParseTimeToSeconds("2:1:0") == 2*60*60+60);
    assert(ParseTimeToSeconds("2:01:0") == 2*60*60+60);
    assert(ParseTimeToSeconds("2:1:1") == 2*60*60+60+1);
    assert(ParseTimeToSeconds("2:11:1") == 2*60*60+11*60+1);
    assert(ParseTimeToSeconds("10:00:00") == 10*60*60);
    assert(ParseTimeToSeconds("10:00:01") == 10*60*60+1);
    assert(ParseTimeToSeconds("10:00:10") == 10*60*60+10);
    assert(ParseTimeToSeconds("10:00:11") == 10*60*60+11);
    assert(ParseTimeToSeconds("10:00:0") == 10*60*60);
    assert(ParseTimeToSeconds("10:00:1") == 10*60*60+1);
    assert(ParseTimeToSeconds("10:1:0") == 10*60*60+60);
    assert(ParseTimeToSeconds("10:1:1") == 10*60*60+60+1);
    assert(ParseTimeToSeconds("10:1:00") == 10*60*60+60);
    assert(ParseTimeToSeconds("10:1:01") == 10*60*60+60+1);
    assert(ParseTimeToSeconds("1:01:00:00") == 24*(60*60) + 60*60);
    assert(ParseTimeToSeconds("1:10:00:01") == 24*(60*60) + 10*60*60 + 1);
    assert(ParseTimeToSeconds("10:10:10:11") == 10*24*60*60 + 10*60*60 + 10*60 + 11);
	unsigned int lineno_end = __LINE__;
    printf("All %u ParseTimeToSeconds() tests succeeded\n", lineno_end - lineno_start - 1);

	printf("char: %d\n", sizeof(char)*8);
    printf("uchar: %d\n", sizeof(unsigned char)*8);
    printf("short: %d\n", sizeof(short)*8);
    printf("ushort: %d\n", sizeof(unsigned short)*8);
    printf("int: %d\n", sizeof(int)*8);
    printf("uint: %d\n", sizeof(unsigned int)*8);
    printf("long: %d\n", sizeof(long)*8);
    printf("ulong: %d\n", sizeof(unsigned long)*8);
    printf("longlong: %d\n", sizeof(long long)*8);
    printf("ulonglong: %d\n", sizeof(unsigned long long)*8);
    printf("time_t: %d\n", sizeof(time_t)*8);
    struct timeval tvx;
    printf("tv_sec: %d\n", sizeof(tvx.tv_sec)*8);
    printf("tv_usec: %d\n", sizeof(tvx.tv_usec)*8);
    tvx.tv_sec = 0;
    --tvx.tv_sec;
    printf("tv_sec: %ssigned\n", (tvx.tv_sec<0)?"":"un");
    tvx.tv_usec = 0;
    --tvx.tv_usec;
    printf("tv_usec: %ssigned\n", (tvx.tv_usec<0)?"":"un");
}

