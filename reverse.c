#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    FILE *in = NULL, *out = NULL;
    if (argc == 1) {
        in = stdin;
        out = stdout;
    } else if (argc == 2) {
        in = fopen(argv[1], "r");
        if (!in) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[1]);
            return 1;
        }
        out = stdout;
    } else if (argc == 3) {
        const char *inPath = argv[1];
        const char *outPath = argv[2];
        if (strcmp(inPath, outPath) == 0) {
            fprintf(stderr, "El archivo de entrada y salida deben diferir\n");
            return 1;
        }
        struct stat sa, sb;
        int haveIn = (stat(inPath, &sa) == 0);
        int haveOut = (stat(outPath, &sb) == 0);
        if (haveIn && haveOut) {
            if (sa.st_dev == sb.st_dev && sa.st_ino == sb.st_ino) {
                fprintf(stderr, "El archivo de entrada y salida deben diferir\n");
                return 1;
            }
        }
        in = fopen(inPath, "r");
        if (!in) {
            fprintf(stderr, "error: cannot open file '%s'\n", inPath);
            return 1;
        }
        out = fopen(outPath, "w");
        if (!out) {
            fprintf(stderr, "error: cannot open file '%s'\n", outPath);
            fclose(in);
            return 1;
        }
    } else {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        return 1;
    }

    typedef struct Node { char *line; struct Node *next; } Node;
    Node *head = NULL;
    char *buf = NULL;
    size_t cap = 0;
    ssize_t nread;

    while ((nread = getline(&buf, &cap, in)) != -1) {
        Node *node = (Node*)malloc(sizeof(Node));
        if (!node) {
            fprintf(stderr, "malloc failed\n");
            free(buf);
            while (head) { Node *nx = head->next; free(head->line); free(head); head = nx; }
            if (in && in != stdin) fclose(in);
            if (out && out != stdout) fclose(out);
            return 1;
        }
        char *copy = (char*)malloc((size_t)nread + 1);
        if (!copy) {
            fprintf(stderr, "malloc failed\n");
            free(node);
            free(buf);
            while (head) { Node *nx = head->next; free(head->line); free(head); head = nx; }
            if (in && in != stdin) fclose(in);
            if (out && out != stdout) fclose(out);
            return 1;
        }
        memcpy(copy, buf, (size_t)nread);
        copy[nread] = '\0';
        node->line = copy;
        node->next = head;
        head = node;
    }
    free(buf);

    for (Node *p = head; p; p = p->next) {
        if (fputs(p->line, out) == EOF) {
            while (head) { Node *nx = head->next; free(head->line); free(head); head = nx; }
            if (in && in != stdin) fclose(in);
            if (out && out != stdout) fclose(out);
            return 1;
        }
    }

    while (head) { Node *nx = head->next; free(head->line); free(head); head = nx; }
    if (in && in != stdin) fclose(in);
    if (out && out != stdout) fclose(out);
    return 0;
}