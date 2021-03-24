#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <semaphore.h>
#define PIPE_NAME1 "RESP_PIPE_51408"
#define PIPE_NAME2 "REQ_PIPE_51408"
char *err = "ERROR";
char err_s = 5;
char *suc = "SUCCESS";
char suc_s = 7;

int main(int argc, char **argv)
{
    unlink(PIPE_NAME1);
    if (mkfifo(PIPE_NAME1, 0600) != 0)
    {
        perror("cannot create the response pipe\n");
        return 1;
    }
    int citire = -1, scriere = -1;
    citire = open(PIPE_NAME2, O_RDONLY);
    if (citire == -1)
    {
        perror("cannot open the request pipe\n");
        return 1;
    }

    scriere = open(PIPE_NAME1, O_WRONLY);
    char *a = "CONNECT";
    char size = 7;
    write(scriere, &size, sizeof(char));
    write(scriere, a, size * sizeof(char));
    printf("SUCCESS\n");

    int file_shared = -1;
    int fisier = -1;
    off_t size_fis = 0;
    char *mem;
    char *mem1;
    char nr_s;
    for (;;)
    {
        char s = 0;
        read(citire, &s, sizeof(char));

        char *c = (char *)malloc((s + 1) * sizeof(char));
        read(citire, c, s * sizeof(char));
        *(c + s) = 0;

        fflush(stdout);
        if (strcmp(c, "PING") == 0)
        {
            char s1[] = "PING";
            char pat = 4;
            write(scriere, &pat, sizeof(char));
            write(scriere, s1, 4 * sizeof(char));
            strcpy(s1, "PONG");
            write(scriere, &pat, sizeof(char));
            write(scriere, s1, 4 * sizeof(char));
            int val = 51408;
            write(scriere, &val, sizeof(int));
        }

        if (strcmp(c, "CREATE_SHM") == 0)
        {
            unsigned int val = 0;
            read(citire, &val, sizeof(unsigned int));

            file_shared = shm_open("/Iljv2BUl", O_CREAT | O_RDWR, 0664);
            ftruncate(file_shared, val);

            write(scriere, &s, sizeof(char));
            write(scriere, c, s * sizeof(char));
            if (file_shared == -1)
            {
                write(scriere, &err_s, sizeof(char));
                write(scriere, err, err_s * sizeof(char));
            }
            else
            {
                mem = (char *)mmap(0, 3073534, PROT_WRITE | PROT_READ, MAP_SHARED, file_shared, 0);
                write(scriere, &suc_s, sizeof(char));
                write(scriere, suc, suc_s * sizeof(char));
            }
        }
        if (strcmp(c, "WRITE_TO_SHM") == 0)
        {
            unsigned int offset, value;
            read(citire, &offset, sizeof(unsigned int));
            read(citire, &value, sizeof(unsigned int));
            printf("off=%d val=%d\n", offset, value);
            if (offset > 3073534 || (offset + sizeof(unsigned int) > 3073534))
            {
                write(scriere, &s, sizeof(char));
                write(scriere, c, s * sizeof(char));
                write(scriere, &err_s, sizeof(char));
                write(scriere, err, err_s * sizeof(char));
            }
            else
            {
                memcpy(&mem[offset], (void *)&value, sizeof(unsigned int));

                write(scriere, &s, sizeof(char));
                write(scriere, c, s * sizeof(char));
                write(scriere, &suc_s, sizeof(char));
                write(scriere, suc, suc_s * sizeof(char));
            }
        }
        if (strcmp(c, "MAP_FILE") == 0)
        {
            char lung;
            read(citire, &lung, sizeof(char));
            char *nume_fis = malloc((lung + 1) * sizeof(char));
            read(citire, nume_fis, lung * sizeof(char));
            *(nume_fis + lung) = 0;
            fisier = open(nume_fis, O_RDONLY);
            if (fisier == -1)
            {
                write(scriere, &s, sizeof(char));
                write(scriere, c, s * sizeof(char));
                write(scriere, &err_s, sizeof(char));
                write(scriere, err, err_s * sizeof(char));
            }
            else
            {
                size_fis = lseek(fisier, 0, SEEK_END);
                int k = 3073535;
                mem1 = (char *)mmap((void *)&k, size_fis, PROT_READ, MAP_SHARED, fisier, 0);
                if (mem1 == (void *)-1)
                {
                    write(scriere, &s, sizeof(char));
                    write(scriere, c, s * sizeof(char));
                    write(scriere, &err_s, sizeof(char));
                    write(scriere, err, err_s * sizeof(char));
                }
                else
                {
                    write(scriere, &s, sizeof(char));
                    write(scriere, c, s * sizeof(char));
                    write(scriere, &suc_s, sizeof(char));
                    write(scriere, suc, suc_s * sizeof(char));
                }
                close(fisier);
                free(nume_fis);
                nume_fis = NULL;
            }
        }
        if (strcmp(c, "READ_FROM_FILE_OFFSET") == 0)
        {
            unsigned int offset, nr_by;
            read(citire, &offset, sizeof(unsigned int));
            read(citire, &nr_by, sizeof(unsigned int));
            if (offset + nr_by > size_fis)
            {
                write(scriere, &s, sizeof(char));
                write(scriere, c, s * sizeof(char));
                write(scriere, &err_s, sizeof(char));
                write(scriere, err, err_s * sizeof(char));
            }
            else
            {
                off_t k = offset + nr_by;
                char *memo = malloc(nr_by * sizeof(char));
                for (off_t j = offset; j < k; j++)
                {
                    strcpy(&memo[j - offset], &mem1[j]);
                }
                int j = 0;

                for (j = 0; j < nr_by; j++)
                {
                    strcpy(&mem[j], &memo[j]);
                }
                write(scriere, &s, sizeof(char));
                write(scriere, c, s * sizeof(char));
                write(scriere, &suc_s, sizeof(char));
                write(scriere, suc, suc_s * sizeof(char));
                free(memo);
                memo=NULL;
            }
        }
        if (strcmp(c, "READ_FROM_FILE_SECTION") == 0)
        {
            unsigned int offset, section, nr_by;

            read(citire, &section, sizeof(unsigned int));
            read(citire, &offset, sizeof(unsigned int));
            read(citire, &nr_by, sizeof(unsigned int));
            nr_s = *(mem1 + 5);
            if (section < 1 || section > (int)nr_s)
            {
                write(scriere, &s, sizeof(char));
                write(scriere, c, s * sizeof(char));
                write(scriere, &err_s, sizeof(char));
                write(scriere, err, err_s * sizeof(char));
            }
            else
            {
                unsigned int sect_offset, sect_size;
                off_t inde = 6 + (section - 1) * 20 + 12;

                sect_offset = *(unsigned int *)(mem1 + inde);
                sect_size = *(unsigned int *)(mem1 + inde + 4);
                printf("sect_o=%d size_sect=%d nr=%d\n", sect_offset, sect_size, nr_by);
                char *memo = malloc(nr_by * sizeof(char));
                if (sect_offset + nr_by > sect_offset + sect_size)
                {
                    write(scriere, &s, sizeof(char));
                    write(scriere, c, s * sizeof(char));
                    write(scriere, &err_s, sizeof(char));
                    write(scriere, err, err_s * sizeof(char));
                }
                else
                {
                    off_t k = sect_offset + offset + nr_by;
                    for (off_t j = sect_offset + offset; j < k; j++)
                    {
                        strcpy(&memo[j - offset - sect_offset], &mem1[j]);
                    }
                    int j = 0;

                    for (j = 0; j < nr_by; j++)
                    {
                        strcpy(&mem[j], &memo[j]);
                    }
                    write(scriere, &s, sizeof(char));
                    write(scriere, c, s * sizeof(char));
                    write(scriere, &suc_s, sizeof(char));
                    write(scriere, suc, suc_s * sizeof(char));
                }
            }
        }
        if (strcmp(c, "READ_FROM_LOGICAL_SPACE_OFFSET") == 0)
        {
            unsigned int log_offset, nr_by;
            read(citire, &log_offset, sizeof(unsigned int));
            read(citire, &nr_by, sizeof(unsigned int));
            int ok = *(mem1 + 5), ok1 = 0;
            off_t inde = 18;
            off_t log_index = 0, ind = 0;
            unsigned int sect_off, sect_size;

            while (ok > 0)
            {

                sect_off = *(unsigned int *)(mem1 + inde);
                sect_size = *(unsigned int *)(mem1 + inde + 4);
                if (log_index + sect_size > log_offset)
                {
                    
                    ind = log_offset - log_index;
                    ok1 = 1;
                    ok = 0;
                }
                else
                {
                    log_index += sect_size;
                    int k = log_index % 1024;
                    log_index = log_index + 1024 - k;
                }
                ok--;
                inde += 20;
            }
            if (ok1 == 0)
            {
                write(scriere, &s, sizeof(char));
                write(scriere, c, s * sizeof(char));
                write(scriere, &err_s, sizeof(char));
                write(scriere, err, err_s * sizeof(char));
            }
            else
            {
                for(off_t k=0;k<nr_by;k++){
                    strcpy(&mem[k],&mem1[sect_off+ind+k]);
                }
                write(scriere, &s, sizeof(char));
                write(scriere, c, s * sizeof(char));
                write(scriere, &suc_s, sizeof(char));
                write(scriere, suc, suc_s * sizeof(char));
            }
        }
        if (strcmp(c, "EXIT") == 0)
        {
            munmap(mem1, sizeof(char) * size_fis);
            close(scriere);
            close(citire);
            unlink(PIPE_NAME1);
            unlink(PIPE_NAME2);
            munmap(mem, sizeof(char) * 3073534);
            break;
        }
        free(c);
        c = NULL;
    }

    return 0;
}