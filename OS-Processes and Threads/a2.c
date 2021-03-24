#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>
#include "a2_helper.h"
sem_t thr3;
sem_t semafor1;
sem_t semafor2;
sem_t semafor3;
sem_t semafor4;
sem_t t3_1;
sem_t t1_3;
sem_t t5;
sem_t *sem1;
sem_t *sem2;
typedef struct
{
    int id;
    sem_t s;
} functie3;
typedef struct
{
    int id;
    sem_t s;

} functie5;
typedef struct{
    int id;
    sem_t s;
}func4;
void *th3(void *arg)
{
    functie3 f = *(functie3 *)arg;
    if (f.id == 5)
    {
        sem_wait(sem2);
        info(BEGIN, 3, f.id);
        sem_wait(&thr3);
        sem_wait(&thr3);
        sem_wait(&thr3);
        sem_wait(&thr3);
        info(END, 3, f.id);
        sem_post(sem1);
        return NULL;
    }
    else
    {
        if (sem_trywait(&f.s) == 0)
        {
            //sem_wait(sem2);
            if (f.id == 1)
            {
                sem_wait(&t3_1);
                info(BEGIN, 3, f.id);
                sem_post(&thr3);
                info(END, 3, f.id);
                sem_post(&t1_3);
                return NULL;
            }
            if (f.id == 3)
            {
                
                info(BEGIN, 3, f.id);
                sem_post(&t3_1);
                sem_wait(&t1_3);
                sem_post(&thr3);
                info(END, 3, f.id);
                return NULL;
            }
            if(f.id==5){
                sem_wait(sem2);
                info(BEGIN, 3, f.id);
                sem_post(&thr3);
                info(END, 3, f.id);
                sem_post(sem1);
                return NULL;
            }
            info(BEGIN, 3, f.id);
            sem_post(&thr3);
            info(END, 3, f.id);
            return NULL;
        }
    }
    return NULL;
}

void *funct5(void *arg){
    functie5 f=*(functie5*) arg;
    if(sem_wait(&f.s)==0){
        if(f.id==12){
            info(BEGIN, 5, f.id);
            sem_post(&semafor4);
            sem_wait(&semafor1);
            sem_wait(&semafor1);
            sem_wait(&semafor1);
            info(END, 5, f.id);
            sem_post(&semafor2);
            sem_post(&semafor2);
            sem_post(&semafor2);
            sem_post(&t5);
            return NULL;
        }else{
            sem_wait(&semafor4);
            sem_post(&semafor4);
            sem_wait(&t5);
            info(BEGIN, 5, f.id);
            if(sem_trywait(&semafor3)==0){
                sem_post(&semafor1);
                sem_wait(&semafor2);
            }
            info(END, 5, f.id);
            sem_post(&t5);
            return NULL;
        }
    }
    return NULL;
}
void* functie4(void*arg){
    func4 k=*(func4 *)arg;
    if(sem_trywait(&k.s)==0){
    if(k.id==5){
        sem_wait(sem1);
    }
    info(BEGIN,4,k.id);
    info(END,4,k.id);
    if(k.id==3){
        sem_post(sem2);
    }
    }
    return NULL;
}

int main()
{
    sem_unlink("s1");
    sem_unlink("s2");
    sem1=sem_open("s1",O_CREAT|O_EXCL,0644,0);
    sem2=sem_open("s2",O_CREAT|O_EXCL,0644,0);


    pid_t pid2 = -1, pid3 = -1, pid4 = -1, pid5 = -1, pid6 = -1, pid7 = -1;
    init();
    info(BEGIN, 1, 0);
    if ((pid2 = fork()) == 0)
    {
        info(BEGIN, 2, 0);
        if ((pid3 = fork()) == 0)
        {
            info(BEGIN, 3, 0);
            if ((pid4 = fork()) == 0)
            {
                info(BEGIN, 4, 0);
                
                pthread_t thr4[6];
                func4 f4[6];
                for(int i=0;i<6;i++){
                    f4[i].id=i+1;
                    sem_init(&f4[i].s,0,1);
                }
                for(int i=0;i<6;i++){
                    pthread_create(&thr4[i],NULL,functie4,&f4[i]);
                }
                for(int i=0;i<6;i++){
                    pthread_join(thr4[i],NULL);
                }
                info(END, 4, 0);
                exit(0);
            }
            else
            {
                if ((pid6 = fork()) == 0)
                {
                    info(BEGIN, 6, 0);
                    info(END, 6, 0);
                    exit(0);
                }
            }
            sem_init(&thr3, 0, 0);
            sem_init(&t3_1, 0, 0);
            sem_init(&t3_1, 0, 0);
            pthread_t threads[5];
            sem_t sem[5];
            functie3 f3[5];
            for (int i = 0; i < 5; i++)
            {
                sem_init(&sem[i], 0, 1);
                f3[i].id = i + 1;
                f3[i].s = sem[i];
            }
            sem_wait(&f3[4].s);
            for (int i = 0; i < 5; i++)
            {
                pthread_create(&threads[i], NULL, th3, &f3[i]);
            }
            for (int i = 0; i < 4; i++)
            {
                pthread_join(threads[i], NULL);
            }
            pthread_join(threads[4], NULL);
            wait(NULL);
            wait(NULL);
            info(END, 3, 0);
            sem_destroy(&thr3);
            sem_destroy(&t3_1);
            sem_destroy(&t3_1);
            exit(0);
        }
        wait(NULL);
        info(END, 2, 0);
        exit(0);
    }
    else
    {
        if ((pid5 = fork()) == 0)
        {
            info(BEGIN, 5, 0);
            if ((pid7 = fork()) == 0)
            {
                info(BEGIN, 7, 0);

                info(END, 7, 0);
                exit(0);
            }
            pthread_t fc5[43];
            functie5 func5[43];
            sem_init(&t5, 0, 3);
            sem_init(&semafor1, 0, 0);
            sem_init(&semafor2, 0, 0);
            sem_init(&semafor3, 0, 3);
            sem_init(&semafor4, 0, 0);
            for (int i = 0; i < 43; i++)
            {
                func5[i].id = i + 1;
                sem_init(&func5[i].s, 0, 1);
            }
            //pthread_mutex_lock(&mut2);
            for (int i = 0; i < 43; i++)
            {
                
                    pthread_create(&fc5[i], NULL, funct5, &func5[i]);
            }
            for (int i = 0; i < 43; i++)
            {
                pthread_join(fc5[i], NULL);
            }
            wait(NULL);
            info(END, 5, 0);
            sem_destroy(&t5);

            sem_destroy(&semafor1);
            sem_destroy(&semafor2);
            sem_destroy(&semafor3);
            sem_destroy(&semafor4);
            exit(0);
        }
    }
    wait(NULL);
    wait(NULL);
    info(END, 1, 0);
    sem_close(sem1);
    sem_close(sem1);
    return 0;
}
