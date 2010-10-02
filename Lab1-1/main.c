/*
 * 1. labor:
 * Írjunk C programot, mely paraméterként megadott állományneveket keres a
 * felhasználó keresési útvonalában ($PATH) megadott katalógusokban.
 *
 * A parancssor minden egyes paraméterére megvizsgálja a keresési útvonalban
 * szereplő összes katalógust, hogy van-e benne az adott paraméterrel
 * megegyező nevű állomány.
 *
 * Pal Csongor · 521
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

// counts the number of paths in the path string, separated by colons
int nr_of_paths(char* s)
{
        int c = 0;
        for (; *s != '\0'; s++)
                if (*s == ':')
                        c++;
        return c + 1;
}

// chops up the path string at the colons
void split_paths(char *path_string, int pathc, char *pathv[])
{
        int i = 0;
        char *curpath = strtok(path_string, ":");
        while (curpath != NULL) {
                pathv[i] = malloc((strlen(curpath) + 1) * sizeof(char *));
                strcpy(pathv[i], curpath);
                curpath = strtok(NULL, ":");
                i++;
        }
}

int main(int argc, char *argv[])
{

        // getting the path string from the system
        char *path_string;
        path_string = malloc((strlen(getenv("PATH")) + 1) * sizeof(char));
        strcpy(path_string, getenv("PATH"));

        // the number of paths is needed to allocate the path array
        int pathc = nr_of_paths(path_string);

        // put the paths in an array
        char** pathv;
        pathv = malloc(pathc * sizeof(char *));
        split_paths(path_string, pathc, pathv);

        // dump stderr
        dup2(open("/dev/null", O_WRONLY), 2);

        // executes the "ls" for all paths and all parameters
        int i, j;
        char *curpath;
        for (i = 1; i < argc; i++) {
                for (j = 0; j < pathc; j++) {
                        // allocating memory for current path (+2 for the
                        // extra '/' character and the \0 at the end)
                        curpath = malloc((strlen(pathv[j]) +
                                          strlen(argv[i]) + 2) * sizeof(char));
                        curpath[0] = '\0';

                        strcat(curpath, pathv[j]);
                        strcat(curpath, "/");
                        strcat(curpath, argv[i]);

                        // forking to call the exec in a child process
                        if (fork())
                                execlp("ls", "ls", "-l", curpath, NULL);

                        free(curpath);
                }
        }

        // cleanup
        for (i = 0; i < pathc; i++)
                free(pathv[i]);
        free(pathv);
        free(path_string);

        return EXIT_SUCCESS;
}
