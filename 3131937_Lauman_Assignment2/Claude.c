/*
 * email_priority.c
 *
 * A CEO email prioritization system built on a MaxHeap (binary heap)
 * implemented from scratch using a dynamically-sized array.
 *
 * Priority rules:
 *   1. Sender category determines primary priority, from highest to lowest:
 *        Boss > Subordinate > Peer > ImportantPerson > OtherPerson
 *   2. Within the same category, a newer email (later date) has higher
 *      priority than an older one.
 *
 * Commands (read one per line from a text file):
 *   EMAIL <sender category>,<subject line>,<date>
 *   NEXT
 *   READ
 *   COUNT
 *
 * Usage:
 *   ./email_priority [command_file]
 *   If no command_file is given, "commands.txt" is used by default.
 *
 * Standard C (C89/C99-compatible), no external heap libraries.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LEN     1024
#define MAX_CATEGORY_LEN 32
#define MAX_SUBJECT_LEN  512
#define MAX_DATE_LEN     16
#define INITIAL_CAPACITY 16

typedef struct {
    char category[MAX_CATEGORY_LEN];
    char subject[MAX_SUBJECT_LEN];
    char date[MAX_DATE_LEN];
} Email;

typedef struct {
    Email *data;
    int size;
    int capacity;
} MaxHeap;

static int categoryRank(const char *category) {
    if (strcmp(category, "Boss") == 0)            return 5;
    if (strcmp(category, "Subordinate") == 0)     return 4;
    if (strcmp(category, "Peer") == 0)            return 3;
    if (strcmp(category, "ImportantPerson") == 0) return 2;
    if (strcmp(category, "OtherPerson") == 0)     return 1;
    return 0;
}

static long dateToComparable(const char *date) {
    int month = 0, day = 0, year = 0;
    sscanf(date, "%d-%d-%d", &month, &day, &year);
    return (long)year * 10000L + (long)month * 100L + (long)day;
}

static int hasHigherPriority(const Email *a, const Email *b) {
    int rankA = categoryRank(a->category);
    int rankB = categoryRank(b->category);

    if (rankA != rankB) {
        return rankA > rankB;
    }

    return dateToComparable(a->date) > dateToComparable(b->date);
}

static void heapInit(MaxHeap *heap) {
    heap->data = (Email *)malloc(sizeof(Email) * INITIAL_CAPACITY);

    if (heap->data == NULL) {
        fprintf(stderr, "Fatal error: could not allocate memory for heap.\n");
        exit(EXIT_FAILURE);
    }

    heap->size = 0;
    heap->capacity = INITIAL_CAPACITY;
}

static void heapFree(MaxHeap *heap) {
    free(heap->data);
    heap->data = NULL;
    heap->size = 0;
    heap->capacity = 0;
}

static void heapGrow(MaxHeap *heap) {
    int newCapacity = heap->capacity * 2;
    Email *newData =
        (Email *)realloc(heap->data, sizeof(Email) * newCapacity);

    if (newData == NULL) {
        fprintf(stderr, "Fatal error: could not grow heap memory.\n");
        exit(EXIT_FAILURE);
    }

    heap->data = newData;
    heap->capacity = newCapacity;
}

static void swapEmails(Email *a, Email *b) {
    Email temp = *a;
    *a = *b;
    *b = temp;
}

static void siftUp(MaxHeap *heap, int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;

        if (hasHigherPriority(&heap->data[i], &heap->data[parent])) {
            swapEmails(&heap->data[i], &heap->data[parent]);
            i = parent;
        } else {
            break;
        }
    }
}

static void siftDown(MaxHeap *heap, int i) {
    for (;;) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int largest = i;

        if (left < heap->size &&
            hasHigherPriority(&heap->data[left], &heap->data[largest])) {
            largest = left;
        }

        if (right < heap->size &&
            hasHigherPriority(&heap->data[right], &heap->data[largest])) {
            largest = right;
        }

        if (largest == i) {
            break;
        }

        swapEmails(&heap->data[i], &heap->data[largest]);
        i = largest;
    }
}

static void heapInsert(MaxHeap *heap, const Email *email) {
    if (heap->size == heap->capacity) {
        heapGrow(heap);
    }

    heap->data[heap->size] = *email;
    siftUp(heap, heap->size);
    heap->size++;
}

static const Email *heapPeek(const MaxHeap *heap) {
    if (heap->size == 0) {
        return NULL;
    }

    return &heap->data[0];
}

static int heapExtractMax(MaxHeap *heap) {
    if (heap->size == 0) {
        return 0;
    }

    heap->data[0] = heap->data[heap->size - 1];
    heap->size--;

    if (heap->size > 0) {
        siftDown(heap, 0);
    }

    return 1;
}

static void trimLineEnding(char *str) {
    size_t len = strlen(str);

    while (len > 0 &&
          (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
        len--;
    }
}

static void trimWhitespace(char *str) {
    char *start = str;
    char *end;
    size_t len;

    while (isspace((unsigned char)*start)) {
        start++;
    }

    len = strlen(start);

    if (start != str) {
        memmove(str, start, len + 1);
    }

    if (len == 0) {
        return;
    }

    end = str + len - 1;

    while (end > str && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
}

static int parseEmailFields(char *payload, Email *outEmail) {
    char *category, *subject, *date;

    category = strtok(payload, ",");
    subject = strtok(NULL, ",");
    date = strtok(NULL, ",");

    if (category == NULL || subject == NULL || date == NULL) {
        return 0;
    }

    trimWhitespace(category);
    trimWhitespace(subject);
    trimWhitespace(date);

    strncpy(outEmail->category, category, MAX_CATEGORY_LEN - 1);
    outEmail->category[MAX_CATEGORY_LEN - 1] = '\0';

    strncpy(outEmail->subject, subject, MAX_SUBJECT_LEN - 1);
    outEmail->subject[MAX_SUBJECT_LEN - 1] = '\0';

    strncpy(outEmail->date, date, MAX_DATE_LEN - 1);
    outEmail->date[MAX_DATE_LEN - 1] = '\0';

    return 1;
}

static void handleEmailCommand(MaxHeap *heap, char *payload) {
    Email email;

    if (parseEmailFields(payload, &email)) {
        heapInsert(heap, &email);
    } else {
        fprintf(stderr,
                "Warning: malformed EMAIL command ignored: %s\n",
                payload);
    }
}

static void handleNextCommand(const MaxHeap *heap) {
    const Email *top = heapPeek(heap);

    if (top == NULL) {
        printf("No emails to read.\n");
        return;
    }

    printf("Next email:\n");
    printf("Sender: %s\n", top->category);
    printf("Subject: %s\n", top->subject);
    printf("Date: %s\n", top->date);
}

static void handleReadCommand(MaxHeap *heap) {
    if (!heapExtractMax(heap)) {
        printf("No emails to read.\n");
    }
}

static void handleCountCommand(const MaxHeap *heap) {
    printf("There are %d emails to read.\n", heap->size);
}

static void processCommandFile(MaxHeap *heap, const char *filename) {
    FILE *file;
    char line[MAX_LINE_LEN];

    file = fopen(filename, "r");

    if (file == NULL) {
        fprintf(stderr,
                "Error: could not open command file '%s'.\n",
                filename);
        return;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        trimLineEnding(line);

        if (line[0] == '\0') {
            continue;
        }

        if (strncmp(line, "EMAIL ", 6) == 0) {
            handleEmailCommand(heap, line + 6);
        } else if (strcmp(line, "NEXT") == 0) {
            handleNextCommand(heap);
        } else if (strcmp(line, "READ") == 0) {
            handleReadCommand(heap);
        } else if (strcmp(line, "COUNT") == 0) {
            handleCountCommand(heap);
        } else {
            fprintf(stderr,
                    "Warning: unrecognized command ignored: %s\n",
                    line);
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    MaxHeap heap;
    const char *filename = "commands.txt";

    if (argc > 1) {
        filename = argv[1];
    }

    heapInit(&heap);
    processCommandFile(&heap, filename);
    heapFree(&heap);

    return 0;
}