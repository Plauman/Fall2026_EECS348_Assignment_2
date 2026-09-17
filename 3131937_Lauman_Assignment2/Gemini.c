#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 10
#define MAX_LINE_LENGTH 256

// Enum representing sender category with numerical values for easy comparison
typedef enum {
    BOSS = 5,
    SUBORDINATE = 4,
    PEER = 3,
    IMPORTANT_PERSON = 2,
    OTHER_PERSON = 1,
    UNKNOWN = 0
} SenderCategory;

// Structure representing an Email
typedef struct {
    SenderCategory category;
    char categoryStr[32];
    char subject[128];
    char date[11];
    int year;
    int month;
    int day;
} Email;

// Structure for the MaxHeap
typedef struct {
    Email *data;
    int size;
    int capacity;
} MaxHeap;

// Function Prototypes
SenderCategory parseCategory(const char *catStr);
void parseDate(const char *dateStr, int *month, int *day, int *year);
int compareEmails(const Email *a, const Email *b);

MaxHeap* createHeap();
void freeHeap(MaxHeap *heap);
void swap(Email *a, Email *b);
void heapifyUp(MaxHeap *heap, int index);
void heapifyDown(MaxHeap *heap, int index);
void insertEmail(MaxHeap *heap, Email email);
void removeMax(MaxHeap *heap);
Email* getNextEmail(MaxHeap *heap);

SenderCategory parseCategory(const char *catStr) {
    if (strcmp(catStr, "Boss") == 0) return BOSS;
    if (strcmp(catStr, "Subordinate") == 0) return SUBORDINATE;
    if (strcmp(catStr, "Peer") == 0) return PEER;
    if (strcmp(catStr, "ImportantPerson") == 0) return IMPORTANT_PERSON;
    if (strcmp(catStr, "OtherPerson") == 0) return OTHER_PERSON;
    return UNKNOWN;
}

void parseDate(const char *dateStr, int *month, int *day, int *year) {
    sscanf(dateStr, "%2d-%2d-%4d", month, day, year);
}

int compareEmails(const Email *a, const Email *b) {
    if (a->category != b->category) {
        return a->category - b->category;
    }

    if (a->year != b->year) {
        return a->year - b->year;
    }

    if (a->month != b->month) {
        return a->month - b->month;
    }

    return a->day - b->day;
}

MaxHeap* createHeap() {
    MaxHeap *heap = (MaxHeap*)malloc(sizeof(MaxHeap));
    heap->capacity = INITIAL_CAPACITY;
    heap->size = 0;
    heap->data = (Email*)malloc(heap->capacity * sizeof(Email));
    return heap;
}

void freeHeap(MaxHeap *heap) {
    if (heap) {
        free(heap->data);
        free(heap);
    }
}

void swap(Email *a, Email *b) {
    Email temp = *a;
    *a = *b;
    *b = temp;
}

void heapifyUp(MaxHeap *heap, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;

        if (compareEmails(&heap->data[index],
                          &heap->data[parent]) > 0) {
            swap(&heap->data[index], &heap->data[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

void heapifyDown(MaxHeap *heap, int index) {
    int largest = index;

    while (1) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < heap->size &&
            compareEmails(&heap->data[left],
                          &heap->data[largest]) > 0) {
            largest = left;
        }

        if (right < heap->size &&
            compareEmails(&heap->data[right],
                          &heap->data[largest]) > 0) {
            largest = right;
        }

        if (largest != index) {
            swap(&heap->data[index], &heap->data[largest]);
            index = largest;
        } else {
            break;
        }
    }
}

void insertEmail(MaxHeap *heap, Email email) {
    if (heap->size == heap->capacity) {
        heap->capacity *= 2;
        heap->data =
            (Email*)realloc(heap->data,
                            heap->capacity * sizeof(Email));
    }

    heap->data[heap->size] = email;
    heapifyUp(heap, heap->size);
    heap->size++;
}

void removeMax(MaxHeap *heap) {
    if (heap->size == 0) {
        return;
    }

    heap->data[0] = heap->data[heap->size - 1];
    heap->size--;

    if (heap->size > 0) {
        heapifyDown(heap, 0);
    }
}

Email* getNextEmail(MaxHeap *heap) {
    if (heap->size == 0) {
        return NULL;
    }

    return &heap->data[0];
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");

    if (!file) {
        perror("Error opening file");
        return 1;
    }

    MaxHeap *queue = createHeap();
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0;

        if (strlen(line) == 0) continue;

        if (strncmp(line, "EMAIL ", 6) == 0) {
            Email newEmail;
            char *payload = line + 6;

            char *catStr = strtok(payload, ",");
            char *subjectStr = strtok(NULL, ",");
            char *dateStr = strtok(NULL, ",");

            if (catStr && subjectStr && dateStr) {
                strncpy(newEmail.categoryStr,
                        catStr,
                        sizeof(newEmail.categoryStr) - 1);

                newEmail.categoryStr[
                    sizeof(newEmail.categoryStr) - 1] = '\0';

                newEmail.category = parseCategory(catStr);

                strncpy(newEmail.subject,
                        subjectStr,
                        sizeof(newEmail.subject) - 1);

                newEmail.subject[
                    sizeof(newEmail.subject) - 1] = '\0';

                strncpy(newEmail.date,
                        dateStr,
                        sizeof(newEmail.date) - 1);

                newEmail.date[
                    sizeof(newEmail.date) - 1] = '\0';

                parseDate(dateStr,
                          &newEmail.month,
                          &newEmail.day,
                          &newEmail.year);

                insertEmail(queue, newEmail);
            }
        } else if (strcmp(line, "NEXT") == 0) {
            Email *next = getNextEmail(queue);

            if (next) {
                printf("Next email:\n");
                printf("Sender: %s\n", next->categoryStr);
                printf("Subject: %s\n", next->subject);
                printf("Date: %s\n", next->date);
            } else {
                printf("No unread emails.\n");
            }
        } else if (strcmp(line, "READ") == 0) {
            removeMax(queue);
        } else if (strcmp(line, "COUNT") == 0) {
            printf("There are %d emails to read.\n",
                   queue->size);
        }
    }

    fclose(file);
    freeHeap(queue);

    return 0;
}