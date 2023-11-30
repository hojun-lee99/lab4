#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *cancle_thread(void *arg)
{
    int i, state;

    for ( i = 0;; i++)
    {
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &state);
        printf("Thread: cancel state disable\n");
        sleep(1);

        pthread_setcancelstate(state, &state);
        printf("Thread: cancel state restored\n");
        if (i % 5 == 0)
        {
            pthread_testcancel();
        }
    }
    return arg;
}

int main(int argc, char *argv[])
{
    
}