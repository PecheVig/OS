#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
char magic;
short head_size;
int version;
int nr_section;
typedef struct
{
    char name[8];
    int type;
    int offset;
    int size;
} my_section;
int llistDiirectorNerecursiv(const char *path)
{
    DIR *director = NULL;
    struct dirent *entry = NULL;
    director = opendir(path);
    if (director == NULL)
    {
        printf("ERROR\ninvalid directory path\n");
        return -1;
    }
    else
    {
        if ((entry = readdir(director)) != NULL)
        {
            while (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                entry = readdir(director);
            }
            printf("SUCCESS\n%s/%s\n", path, entry->d_name);
            while ((entry = readdir(director)) != NULL)
            {
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                    printf("%s/%s\n", path, entry->d_name);
            }
        }
        else
        {
            printf("SUCCESS\n");
        }
    }
    closedir(director);
    return 0;
}
int listDirectorRecursiv(const char *path, int ok, int operatie, char *termina)
{
    DIR *director = NULL;
    struct dirent *entry;
    char fullPath[3000];
    struct stat variabila;

    director = opendir(path);
    if (director == NULL)
    {
        printf("ERROR\ninvalid directory path\n");
        return 1;
    }
    else
    {
        if (ok == 1)
        {
            printf("SUCCESS\n");
        }
        while ((entry = readdir(director)) != NULL)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                snprintf(fullPath, 3000, "%s/%s", path, entry->d_name);
                if (lstat(fullPath, &variabila) == 0)
                {
                    if (operatie == 1)
                    {
                        if (S_IWUSR & variabila.st_mode && (S_ISLNK(variabila.st_mode) == 0))
                            printf("%s\n", fullPath);
                    }
                    else
                    {
                        if (operatie == 2)
                        {
                            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                            {
                                char aux[1000];
                                for (int i = strlen(entry->d_name) - strlen(termina); i < strlen(entry->d_name); i++)
                                {
                                    aux[i - strlen(entry->d_name) + strlen(termina)] = entry->d_name[i];
                                }
                                if (strcmp(termina, aux) == 0)
                                {
                                    printf("%s/%s\n", path, entry->d_name);
                                }
                            }
                        }
                        else
                        {
                            printf("%s\n", fullPath);
                        }
                    }
                    if (S_ISDIR(variabila.st_mode))
                    {
                        listDirectorRecursiv(fullPath, 0,operatie,termina);
                    }
                }
            }
        }
    }

    closedir(director);
    return 0;
}
int listCuPermisiuni1(const char *path)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char filePath[3000];
    struct stat entry1;
    dir = opendir(path);
    if (dir == NULL)
    {
        printf("ERROR\ninvalid directory path\n");
        return 1;
    }
    printf("SUCCESS\n");
    while ((entry = readdir(dir)) != NULL)
    {
        snprintf(filePath, 3000, "%s/%s", path, entry->d_name);
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            if (lstat(filePath, &entry1) == 0)
            {
                if (S_IWUSR & entry1.st_mode && (S_ISLNK(entry1.st_mode) == 0))
                {
                    //printf("[%o]\n",entry1.st_mode&0777);
                    printf("%s/%s\n", path, entry->d_name);
                }
            }
            //printf("%s\n",entry->d_name);
        }
    }
    closedir(dir);
    return 0;
}
int listCuPermisiuni2(const char *path, char *termina)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char filePath[3000];
    dir = opendir(path);

    if (dir == NULL)
    {
        printf("ERROR\ninvalid directory path\n");
        return 1;
    }
    printf("SUCCESS\n");
    while ((entry = readdir(dir)) != NULL)
    {
        snprintf(filePath, 3000, "%s/%s", path, entry->d_name);
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            char aux[1000];
            for (int i = strlen(entry->d_name) - strlen(termina); i < strlen(entry->d_name); i++)
            {
                aux[i - strlen(entry->d_name) + strlen(termina)] = entry->d_name[i];
            }
            if (strcmp(termina, aux) == 0)
            {
                printf("%s/%s\n", path, entry->d_name);
            }
        }
    }

    closedir(dir);
    return 0;
}
int parse(const char *path, int modFunctionare)
{
    int fd = 0;
    fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        printf("ERROR\n");
        return 1;
    }
    read(fd, &magic, 1);
    read(fd, &head_size, 2);
    read(fd, &version, 2);
    read(fd, &nr_section, 1);
    int ok = 1;

    off_t size = 0;
    int k = 0;
    size = lseek(fd, 0, SEEK_END);
    my_section a[nr_section];
    int l = 6;
    for (k = 0; k < nr_section && size > k; k++)
    {
        lseek(fd, l, SEEK_SET);
        read(fd, &a[k].name, 8);
        l += 8;
        lseek(fd, l, SEEK_SET);
        read(fd, &a[k].type, 4);
        l += 4;
        lseek(fd, l, SEEK_SET);
        read(fd, &a[k].offset, 4);
        l += 4;
        lseek(fd, l, SEEK_SET);
        read(fd, &a[k].size, 4);
        l += 4;
    }
    for (k = 0; k < nr_section; k++)
    {
        int okt = 1;
        if (a[k].type == 37 || a[k].type == 15 || a[k].type == 69 || a[k].type == 43 || a[k].type == 49 || a[k].type == 74)
            okt = 0;
        ok += okt;
    }
    int ok1 = 1, ok2 = 1, ok3 = 1;
    if (modFunctionare == 1)
    {
        if (magic == 'R')
        {
            if (version >= 53 && version <= 143)
            {
                if (nr_section >= 3 && nr_section <= 13)
                {
                    if (ok == 1)
                    {
                        printf("SUCCESS\nversion=%d\nnr_sections=%d\n", version, nr_section);
                        size = lseek(fd, 0, SEEK_SET);
                        for (int j = 0; j < nr_section; j++)
                        {
                            printf("section%d: ", j + 1);
                            int verif = 1;
                            for (int n = 0; n < 8 && verif == 1; n++)
                            {
                                if ((a[j].name[n] >= 'a' && a[j].name[n] <= 'z') || (a[j].name[n] >= 'A' && a[j].name[n] <= 'Z') || (a[j].name[n] >= '0' && a[j].name[n] <= '9'))
                                {
                                    printf("%c", a[j].name[n]);
                                }
                                else
                                    verif = 0;
                            }
                            printf(" %d %d\n", a[j].type, a[j].size);
                        }
                    }
                }
                else
                    ok3 = 0;
            }
            else
                ok2 = 0;
        }
        else
            ok1 = 0;
    }
    if (modFunctionare == 1)
    {
        if (ok1 == 0 || ok2 == 0 || ok3 == 0 || ok != 1)
        {
            printf("ERROR\nwrong ");
            if (ok1 == 0)
            {
                printf("magic");
                if (ok2 == 0 || ok3 == 0 || ok == 0)
                {
                    printf("|");
                }
            }
            if (ok2 == 0)
            {
                printf("version");
                if (ok3 == 0 || ok == 0)
                {
                    printf("|");
                }
            }
            if (ok3 == 0)
            {
                printf("sect_nr");
                if (ok == 0)
                {
                    printf("|");
                }
            }
            if (ok != 1)
            {
                printf("sect_types");
            }
        }
        close(fd);
    }
    else
    {
        if (ok1 == 0 || ok2 == 0 || ok3 == 0 || ok != 1)
        {
            close(fd);
            return 1;
        }
        else
        {
            int nrLiniiSectiuneActuala = 0;
            int nrMaximLinii = 0;
            size = lseek(fd, 0, SEEK_SET);
            for (int j = 0; j < nr_section; j++)
            {
                nrLiniiSectiuneActuala = 0;
                lseek(fd, a[j].offset, SEEK_SET);
                char auxiliar[a[j].size];
                read(fd, auxiliar, a[j].size);
                while (strchr(auxiliar, 0x0A))
                {
                    strcpy(auxiliar, strchr(auxiliar, 0x0A) + 1);
                    nrLiniiSectiuneActuala++;
                }
                if (nrMaximLinii < nrLiniiSectiuneActuala)
                    nrMaximLinii = nrLiniiSectiuneActuala;
            }
            if (nrMaximLinii > 13)
            {
                close(fd);
                return 0;
            }
        }
    }
    close(fd);
    return 0;
}
int findAll(const char *path, int ok)
{
    DIR *director = NULL;
    struct dirent *entry;
    char fullPath[3000];
    struct stat variabila;
    director = opendir(path);
    if (director == NULL)
    {
        printf("ERROR\ninvalid directory path\n");
        return 1;
    }
    else
    {
        if (ok == 1)
        {
            printf("SUCCESS\n");
        }
        int k = 0;
        while ((entry = readdir(director)) != NULL)
        {
            k++;
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                snprintf(fullPath, 3000, "%s/%s", path, entry->d_name);
                if (lstat(fullPath, &variabila) == 0)
                {
                    if (S_ISREG(variabila.st_mode))
                    {
                        if (parse(fullPath, 0) == 0)
                        {
                            //if(strcmp(fullPath,"test_root/")!=0)
                            printf("%s\n", fullPath);
                        }
                    }
                    if (S_ISDIR(variabila.st_mode))
                    {
                        closedir(director);
                        findAll(fullPath, 0);
                        director = opendir(path);
                        int inde = 0;
                        while (inde < k)
                        {
                            entry = readdir(director);
                            inde++;
                        }
                    }
                }
            }
        }
    }

    closedir(director);
    return 0;
}
int ext(const char *path, int linie, int sectiune)
{
    int fd = 0;

    fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        printf("ERROR\ninvalid file");
        return 1;
    }
    read(fd, &magic, 1);
    read(fd, &head_size, 2);
    read(fd, &version, 2);
    read(fd, &nr_section, 1);

    off_t size = 0, i;
    int k = 0;
    size = lseek(fd, 0, SEEK_END);
    my_section a[nr_section];
    int l = 6;
    for (k = 0; k < nr_section && size > k; k++)
    {
        lseek(fd, l, SEEK_SET);
        read(fd, &a[k].name, 8);
        l += 8;
        lseek(fd, l, SEEK_SET);
        read(fd, &a[k].type, 4);
        l += 4;
        lseek(fd, l, SEEK_SET);
        read(fd, &a[k].offset, 4);
        l += 4;
        lseek(fd, l, SEEK_SET);
        read(fd, &a[k].size, 4);
        l += 4;
    }
    char c;
    char aux[a[sectiune - 1].size];
    k = 0;
    off_t nrLinii;
    if (sectiune > nr_section)
    {
        printf("ERROR\ninvalid section");
        return 1;
    }
    i = a[sectiune - 1].offset;
    while (i < a[sectiune - 1].offset + a[sectiune - 1].size)
    {
        lseek(fd, i, SEEK_SET);
        read(fd, &c, 1);
        if (linie - 1 == nrLinii)
        {
            aux[k] = c;
            k++;
        }
        if (c == 0x0A)
        {
            nrLinii++;
        }
        i++;
    }
    if (linie - 1 > nrLinii)
    {
        printf("ERROR\ninvalid line");
        return 1;
    }
    printf("SUCCESS\n");
    // printf("%d %d \n",k,a[sectiune-1].size);
    if (k == 70)
    {
        for (i = k - 1; i >= 0; i--)
        {
            printf("%c", aux[i]);
        }
    }
    else
    {
        for (i = k - 2; i >= 0; i--)
        {
            printf("%c", aux[i]);
        }
    }
    close(fd);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        if (strcmp(argv[1], "variant") == 0)
        {
            printf("51408\n");
        }
    }

    if (argc == 3)
    {
        if (strcmp(argv[1], "list") == 0)
        {
            char a[100] = "";
            char b[100] = "";
            strcpy(a, argv[2]);
            for (int i = 5; i < strlen(a); i++)
            {
                b[i - 5] = a[i];
            }
            //printf("%s\n",b);
            llistDiirectorNerecursiv(b);
        }
        else
        {
            if (strcmp(argv[1], "parse") == 0)
            {
                char a[1000];

                char b[1000];
                strcpy(a, argv[2]);
                for (int i = 5; i < strlen(a); i++)
                {
                    b[i - 5] = a[i];
                }
                parse(b, 1);
            }
            else
            {
                if (strcmp(argv[1], "findall") == 0)
                {
                    char a[1000];

                    char b[1000];
                    strcpy(a, argv[2]);
                    for (int i = 5; i < strlen(a); i++)
                    {
                        b[i - 5] = a[i];
                    }
                    findAll(b, 1);
                }
            }
        }
    }
    if (argc == 5)
    {
        if (strcmp(argv[1], "list") == 0)
        {
            int write = 0;
            int recursiv = 0;
            char path[1000] = "";
            char pathDeTrimis[1000] = "";
            char nameEnds[1000] = "";
            char nameDeTrimis[1000] = "";
            char aux[1000];
            for (int i = 2; i < argc; i++)
            {
                strcpy(aux, argv[i]);
                if (strcmp(path, "") == 0)
                {
                    for (int j = 0; j < 5; j++)
                    {
                        path[i] = aux[i];
                    }
                    if (strcmp(path, "path=") == 0)
                    {
                        for (int j = 5; j < strlen(aux); j++)
                        {
                            pathDeTrimis[j - 5] = aux[j];
                        }
                    }
                    else
                    {
                        memset(path, '\0', 1000);
                    }
                }
                if (strcmp(nameEnds, "") == 0)
                {
                    for (int j = 0; j < 15; j++)
                    {
                        nameEnds[j] = aux[j];
                    }
                }
                if (strcmp(nameEnds, "name_ends_with=") == 0)
                {
                    for (int j = 15; j < strlen(aux); j++)
                    {
                        nameDeTrimis[j - 15] = aux[j];
                    }
                }
                else
                {
                    memset(nameEnds, '\0', 1000);
                }
                if (strcmp(aux, "recursive") == 0)
                {
                    recursiv = 1;
                }
                if (strcmp(aux, "has_perm_write"))
                {
                    write = 1;
                }
            }
            if (recursiv == 1)
            {
                if (write == 1)
                    listDirectorRecursiv(pathDeTrimis, 1,1,"");
                else
                {
                    listDirectorRecursiv(pathDeTrimis, 1,2,nameDeTrimis);
                }
            }
        }
    }
    if (argc == 4)
    {
        if (strcmp(argv[1], "list") == 0)
        {
            char a[1000] = "";
            char b[10000] = "";
            strcpy(a, argv[3]);
            for (int i = 5; i < strlen(a); i++)
            {
                b[i - 5] = a[i];
            }
            if (strcmp(argv[2], "recursive") == 0)
            {
                listDirectorRecursiv(b, 1,0,"");
            }
            else
            {
                if (strcmp(argv[2], "has_perm_write") == 0)
                {

                    listCuPermisiuni1(b);
                }
                else
                {
                    char g[1000];
                    char t[1000];
                    char q[1000];
                    strcpy(t, argv[2]);
                    for (int i = 0; i < 15; i++)
                    {
                        g[i] = t[i];
                    }
                    for (int i = 15; i < strlen(t); i++)
                    {
                        q[i - 15] = t[i];
                    }
                    if (strcmp(g, "name_ends_with=") == 0)
                    {
                        listCuPermisiuni2(b, q);
                    }
                }
            }
        }
    
    }
    if (argc == 4)
    {
    }
    if (argc == 5)
    {
        char path[1000] = "";
        char pathDeTrimis[1000] = "";
        char section[1000] = "";
        char sectionDeTrimis[1000] = "";
        char line[1000] = "";
        char lineDeTrimis[10];
        for (int j = 2; j < argc; j++)
        {
            char aux[1000];
            strcpy(aux, argv[j]);
            if (strcmp(path, "") == 0)
            {
                for (int i = 0; i < 5; i++)
                {
                    path[i] = aux[i];
                }
                if (strcmp(path, "path=") == 0)
                {
                    for (int i = 5; i < strlen(aux); i++)
                    {
                        pathDeTrimis[i - 5] = aux[i];
                    }
                }
                else
                {
                    memset(path, '\0', 1000);
                }
            }
            if (strcmp(section, "") == 0)
            {
                for (int i = 0; i < 8; i++)
                {
                    section[i] = aux[i];
                }
                if (strcmp(section, "section=") == 0)
                {
                    for (int i = 8; i < strlen(aux); i++)
                    {
                        sectionDeTrimis[i - 8] = aux[i];
                    }
                }
                else
                {
                    memset(section, '\0', 1000);
                }
            }
            if (strcmp(line, "") == 0)
            {
                for (int i = 0; i < 5; i++)
                {
                    line[i] = aux[i];
                }
                if (strcmp(line, "line=") == 0)
                {
                    for (int i = 5; i < strlen(aux); i++)
                    {
                        lineDeTrimis[i - 5] = aux[i];
                    }
                }
                else
                {
                    memset(line, '\0', 1000);
                }
            }
        }
        ext(pathDeTrimis, atoi(lineDeTrimis), atoi(sectionDeTrimis));
    }
    // for(int i=0;i<argc;i++){
    //     printf("%s ",argv[i]);
    // }
    return 0;
}