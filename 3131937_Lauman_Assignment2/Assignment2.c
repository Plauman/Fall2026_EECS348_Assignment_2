/*
 * Program Name: EECS 348 Assignment 2
 *
 * Description:
 * This program prioritizes emails for a company CEO using a
 * priority queue implemented with a MaxHeap. Emails are first
 * prioritized by sender category. If two emails have the same
 * sender category, the newest email has the higher priority.
 *
 * Input:
 * A text file named test.txt containing the commands
 * EMAIL, NEXT, READ, and COUNT.
 *
 * Output:
 * Displays the next highest-priority email when NEXT is used
 * and displays the number of unread emails when COUNT is used.
 *
 * Author: Peter Lauman
 * Creation Date: September 17, 2026
 * Revision Date: September 17, 2026
 *
 * Collaborators: None
 *
 * Sources:
 * Claude - Generated the original MaxHeap implementation used
 *          as the basis for this program.
 * Google Gemini - Generated a second implementation that was
 *                 analyzed for comparison.
 * ChatGPT - Assisted with comparing the generated programs,
 *           identifying improvements, testing edge cases,
 *           organizing comments, and improving maintainability.
 *
 * Revisions:
 * Improved Claude's original implementation by simplifying the
 * structure, adding detailed comments, improving memory checks,
 * and ensuring the required commands and edge cases are handled.
 */

#include <stdio.h>   // Provides file and terminal input/output functions.
#include <stdlib.h>  // Provides dynamic memory allocation functions.
#include <string.h>  // Provides string manipulation functions.

#define INITIAL_CAPACITY 10 // Initial number of emails the heap can hold.
#define MAX_LINE 512        // Maximum length of one input-file line.
#define MAX_CATEGORY 32     // Maximum length of a sender category.
#define MAX_SUBJECT 256     // Maximum length of an email subject.
#define MAX_DATE 11         // Space for MM-DD-YYYY 

/*
 * Email structure.
 *
 * Source: Based on Claude's generated implementation and modified
 * by the author with assistance from ChatGPT
 */
typedef struct
{
    char category[MAX_CATEGORY]; // Stores the sender category.
    char subject[MAX_SUBJECT];   // Stores the email subject.
    char date[MAX_DATE];         // Stores the date in MM-DD-YYYY format.
} Email;

/*
 * MaxHeap structure.
 *
 * Source: Based on Claude's generated implementation.
 */
typedef struct
{
    Email *data;  // Dynamically allocated array of emails.
    int size;     // Current number of unread emails.
    int capacity; // Current capacity of the array.
} MaxHeap;

/*
 * Returns a numerical priority for a sender category.
 * A larger number means a higher priority.
 *
 * Source: Based on Claude's generated implementation.
 */
int categoryPriority(const char *category)
{
    if (strcmp(category, "Boss") == 0)
    {
        return 5; // Boss is highest priority.
    }

    if (strcmp(category, "Subordinate") == 0)
    {
        return 4; // Subordinate is second priority.
    }

    if (strcmp(category, "Peer") == 0)
    {
        return 3; // Peer is third priority.
    }

    if (strcmp(category, "ImportantPerson") == 0)
    {
        return 2; // ImportantPerson is fourth priority.
    }

    if (strcmp(category, "OtherPerson") == 0)
    {
        return 1; // OtherPerson is lowest priority.
    }

    return 0; // Handles an unexpected category.
}

/*
 * Converts MM-DD-YYYY into YYYYMMDD so dates can be compared.
 *
 * Source: Based on Claude's generated implementation.
 */
long convertDate(const char *date)
{
    int month; // Stores the month.
    int day;   // Stores the day.
    int year;  // Stores the year.

    // Separates the date into month, day, and year.
    sscanf(date, "%d-%d-%d", &month, &day, &year);

    // Returns the date in a form that can be numerically compared.
    return (long)year * 10000L + month * 100L + day;
}

/*
 * Determines whether email a has higher priority than email b.
 *
 * Source: Based on Claude's generated implementation.
 */
int higherPriority(const Email *a, const Email *b)
{
    // Gets the sender priority for the first email.
    int priorityA = categoryPriority(a->category);

    // Gets the sender priority for the second email.
    int priorityB = categoryPriority(b->category);

    // Checks sender category first.
    if (priorityA != priorityB)
    {
        return priorityA > priorityB;
    }

    // If categories match, the newest date has higher priority.
    return convertDate(a->date) > convertDate(b->date);
}

/*
 * Initializes an empty MaxHeap.
 *
 * Source: Based on Claude's generated implementation.
 */
void initializeHeap(MaxHeap *heap)
{
    // Allocates memory for the initial email array.
    heap->data = malloc(INITIAL_CAPACITY * sizeof(Email));

    // Checks whether memory allocation failed.
    if (heap->data == NULL)
    {
        fprintf(stderr, "Error: Unable to allocate heap memory.\n");

        exit(EXIT_FAILURE);
    }

    heap->size = 0; // Starts with zero unread emails.

    heap->capacity = INITIAL_CAPACITY; // Sets the initial capacity.
}

/*
 * Releases the dynamically allocated heap memory.
 *
 * Source: Based on Claude's generated implementation.
 */
void freeHeap(MaxHeap *heap)
{
    free(heap->data); // Releases the email array.

    heap->data = NULL; // Prevents use of the freed memory.

    heap->size = 0; // Resets the heap size.

    heap->capacity = 0; // Resets the heap capacity.
}

/*
 * Swaps two Email structures.
 *
 * Source: Based on Claude's generated implementation.
 */
void swapEmails(Email *a, Email *b)
{
    Email temp = *a; // Temporarily saves the first email.

    *a = *b; // Moves the second email into the first position.

    *b = temp; // Moves the original first email into the second position.
}

/*
 * Moves an email upward until the MaxHeap property is restored.
 *
 * Source: Based on Claude's generated implementation.
 */
void heapifyUp(MaxHeap *heap, int index)
{
    // Continues until the root is reached or the heap is correct.
    while (index > 0)
    {
        // Calculates the index of the parent.
        int parent = (index - 1) / 2;

        // Checks whether the child has higher priority than the parent.
        if (higherPriority(&heap->data[index],
                           &heap->data[parent]))
        {
            // Swaps the child and parent.
            swapEmails(&heap->data[index],
                       &heap->data[parent]);

            // Continues checking from the parent's old position.
            index = parent;
        }
        else
        {
            break; // Stops because the heap property is correct.
        }
    }
}

/*
 * Moves an email downward until the MaxHeap property is restored.
 *
 * Source: Based on Claude's generated implementation.
 */
void heapifyDown(MaxHeap *heap, int index)
{
    // Continues until the email reaches the correct position.
    while (1)
    {
        // Calculates the left-child index.
        int left = 2 * index + 1;

        // Calculates the right-child index.
        int right = 2 * index + 2;

        // Initially assumes the current email is highest priority.
        int highest = index;

        // Checks whether the left child exists and has higher priority.
        if (left < heap->size &&
            higherPriority(&heap->data[left],
                           &heap->data[highest]))
        {
            highest = left;
        }

        // Checks whether the right child exists and has higher priority.
        if (right < heap->size &&
            higherPriority(&heap->data[right],
                           &heap->data[highest]))
        {
            highest = right;
        }

        // If the current email is already highest, stop.
        if (highest == index)
        {
            break;
        }

        // Swaps the current email with the higher-priority child.
        swapEmails(&heap->data[index],
                   &heap->data[highest]);

        // Continues checking from the new position.
        index = highest;
    }
}

/*
 * Doubles the size of the heap array when it becomes full.
 *
 * Source: Based on Claude's generated implementation.
 */
void growHeap(MaxHeap *heap)
{
    // Calculates the new capacity.
    int newCapacity = heap->capacity * 2;

    // Attempts to resize the email array.
    Email *newData =
        realloc(heap->data, newCapacity * sizeof(Email));

    // Checks whether realloc failed.
    if (newData == NULL)
    {
        fprintf(stderr, "Error: Unable to resize heap memory.\n");

        freeHeap(heap);

        exit(EXIT_FAILURE);
    }

    // Updates the heap's array pointer.
    heap->data = newData;

    // Updates the heap capacity.
    heap->capacity = newCapacity;
}

/*
 * Inserts a new email into the MaxHeap.
 *
 * Source: Based on Claude's generated implementation and corrected
 * by the author with assistance from ChatGPT.
 */
void insertEmail(MaxHeap *heap, Email email)
{
    // Checks whether more storage space is required.
    if (heap->size == heap->capacity)
    {
        growHeap(heap);
    }

    // Places the new email at the end of the heap.
    heap->data[heap->size] = email;

    // Increases the number of emails in the heap.
    heap->size++;

    // Moves the newly inserted email to its correct position.
    heapifyUp(heap, heap->size - 1);
}

/*
 * Returns the highest-priority email without removing it.
 *
 * Source: Based on Claude's generated implementation.
 */
Email *nextEmail(MaxHeap *heap)
{
    // Checks whether the heap is empty.
    if (heap->size == 0)
    {
        return NULL;
    }

    // The root of a MaxHeap contains the highest-priority email.
    return &heap->data[0];
}

/*
 * Removes the highest-priority email from the MaxHeap.
 *
 * Source: Based on Claude's generated implementation.
 */
void readEmail(MaxHeap *heap)
{
    // Safely handles READ when no emails exist.
    if (heap->size == 0)
    {
        return;
    }

    // Replaces the root with the last email in the heap.
    heap->data[0] = heap->data[heap->size - 1];

    // Decreases the unread email count.
    heap->size--;

    // Restores the MaxHeap if emails remain.
    if (heap->size > 0)
    {
        heapifyDown(heap, 0);
    }
}

/*
 * Main function.
 *
 * Reads commands from test.txt and performs each requested operation.
 *
 * Source: Based on Claude's generated implementation and modified
 * by the author with assistance from ChatGPT.
 */
int main(void)
{
    MaxHeap heap; // Creates the MaxHeap.

    FILE *file; // Stores the test-file pointer.

    char line[MAX_LINE]; // Stores one line from the test file.

    // Opens test.txt from the same directory as the program.
    file = fopen("test.txt", "r");

    // Checks whether test.txt was successfully opened.
    if (file == NULL)
    {
        fprintf(stderr, "Error: Could not open test.txt.\n");

        return 1;
    }

    // Initializes the empty MaxHeap.
    initializeHeap(&heap);

    // Reads each line from the test file.
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // Removes newline characters from the end of the line.
        line[strcspn(line, "\r\n")] = '\0';

        // Checks whether the command is EMAIL.
        if (strncmp(line, "EMAIL ", 6) == 0)
        {
            Email email; // Creates a new Email structure.

            // Gets the sender category.
            char *category = strtok(line + 6, ",");

            // Gets the subject line.
            char *subject = strtok(NULL, ",");

            // Gets the email date.
            char *date = strtok(NULL, ",");

            // Makes sure all required email fields were found.
            if (category != NULL &&
                subject != NULL &&
                date != NULL)
            {
                // Copies the category into the Email structure.
                strncpy(email.category,
                        category,
                        MAX_CATEGORY - 1);

                // Guarantees the category string is null terminated.
                email.category[MAX_CATEGORY - 1] = '\0';

                // Copies the subject into the Email structure.
                strncpy(email.subject,
                        subject,
                        MAX_SUBJECT - 1);

                // Guarantees the subject string is null terminated.
                email.subject[MAX_SUBJECT - 1] = '\0';

                // Copies the date into the Email structure.
                strncpy(email.date,
                        date,
                        MAX_DATE - 1);

                // Guarantees the date string is null terminated.
                email.date[MAX_DATE - 1] = '\0';

                // Inserts the email into the MaxHeap.
                insertEmail(&heap, email);
            }
        }

        // Checks whether the command is NEXT.
        else if (strcmp(line, "NEXT") == 0)
        {
            // Gets the highest-priority email without removing it.
            Email *email = nextEmail(&heap);

            // Displays the email if one exists.
            if (email != NULL)
            {
                printf("Next email:\n");

                printf("Sender: %s\n",
                       email->category);

                printf("Subject: %s\n",
                       email->subject);

                printf("Date: %s\n",
                       email->date);
            }
        }

        // Checks whether the command is READ.
        else if (strcmp(line, "READ") == 0)
        {
            // Removes the highest-priority email.
            readEmail(&heap);
        }

        // Checks whether the command is COUNT.
        else if (strcmp(line, "COUNT") == 0)
        {
            // Displays the current number of unread emails.
            printf("There are %d emails to read.\n",
                   heap.size);
        }
    }

    // Closes the input file.
    fclose(file);

    // Releases the dynamically allocated heap memory.
    freeHeap(&heap);

    // Indicates successful execution.
    return 0;
}