/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sndfile.h>
#include <pthread.h>
#include <getopt.h>
#include <jack/jack.h>
#include <jack/ringbuffer.h>
*/

#include "jack_stuff.h"
#include "midi.hh"

//#include "refactor.hh"

//#include <iostream>

char* buffer;
int bsize;

MidiFactor::MidiFactor ()
{
    // initialize the jack midi connection
    int result = init ();
}

MidiFactor::~MidiFactor ()
{
    delete buffer;
}

int MidiFactor::pull_current (char* buffer, int bsize)
{
    int result;

    result = get_current_patch (buffer, bsize);

    return result;
}
