#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    (void)argv;
    FILE *in = NULL;

    if (argc == 1) {
        in = stdin;
    } else if (argc == 2) {
        in = fopen(argv[1], "r");
        if (!in) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[1]);
            return 1;
        }
    } else {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        return 1;
    }

    typedef struct Node {
        char *line;
        struct Node *next;
    } Node;

    Node *head = NULL;
    char *buf = NULL;
    size_t cap = 0;
    ssize_t n;

    while ((n = getline(&buf, &cap, in)) != -1) {
        Node *node = (Node*)malloc(sizeof(Node));
        if (!node) {
            fprintf(stderr, "malloc failed\n");
            free(buf);
            while (head) { Node *nx = head->next; free(head->line); free(head); head = nx; }
            if (in && in != stdin) fclose(in);
            return 1;
        }
        char *copy = (char*)malloc((size_t)n + 1);
        if (!copy) {
            fprintf(stderr, "malloc failed\n");
            free(node);
            free(buf);
            while (head) { Node *nx = head->next; free(head->line); free(head); head = nx; }
            if (in && in != stdin) fclose(in);
            return 1;
        }
        memcpy(copy, buf, (size_t)n);
        copy[n] = '\0';
        node->line = copy;
        node->next = head;
        head = node;
    }
    free(buf);

    for (Node *p = head; p; p = p->next) {
        if (fputs(p->line, stdout) == EOF) {
            while (head) { Node *nx = head->next; free(head->line); free(head); head = nx; }
            if (in && in != stdin) fclose(in);
            return 1;
        }
    }

    while (head) { Node *nx = head->next; free(head->line); free(head); head = nx; }
    if (in && in != stdin) fclose(in);
    return 0;
}