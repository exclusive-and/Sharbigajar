
// Sharbigajar.Text

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Effectno.h"
#include "Text.h"


// type MutText

// Create a new empty text object with space for 'n' characters.
//
MutText newMutText (unsigned int n) {
    char *array = (char *) malloc (n * sizeof (char));
    
    array[0] = '\0';

    return (MutText) {
        .array  = array,
        .length = n
    };
}

void freeMutText (MutText mutText) {
    free (mutText.array);
}


// type Text

// Solidify a mutable text object into an immutable one.
Text solidifyText (MutText mutText) {
    return (Text) {
        .array  = mutText.array,
        .length = mutText.length
    };
}

// Free a text object.
//
void freeText (Text text) {
    free (text.array);
}


// Create a text object from a constant string without copying.
//
Text textFromString (const char string[]) {
    return (Text) {
        .array  = string,
        .length = strlen (string)
    };
}

// Copy the contents of a string to a text object.
//
Text copyTextFromString (const char string[]) {
    unsigned int length = strlen (string);
    MutText mutText = newMutText (length + 1);

    memcpy (mutText.array, string, length * sizeof (char));
    mutText.array[length] = '\0';

    return solidifyText (mutText);
}


// Concatenate two text objects together.
//
Text concatText (Text fst, Text snd) {
    unsigned int
        fstLen = fst.length,
        sndLen = snd.length,
        newLen = fstLen + sndLen;

    MutText mutText = newMutText (newLen);

    void *p = mutText.array;
    memcpy (p, fst.array, fstLen * sizeof (char));
    p += fstLen * sizeof (char);
    memcpy (p, snd.array, sndLen * sizeof (char));

    return solidifyText (mutText);
}

// Append a string to the end of a text object.
//
Text appendText (Text fst, const char string[]) {
    return concatText (fst, textFromString (string));
}


// Read the contents of a file to a text object.
//
Text textFromFile (Text filename) {
    FILE *fp = fopen(filename.array, "r");
    if (!fp) {
        effectno = IOError;
        return (Text) {0, 0};
    }

    unsigned int
        size    = 1024,
        total   = 0,
        count;
    char *buf = (char *) malloc (size);

    while ((count = fread (buf + total, 1, size - total, fp))) {
        total += count;
        if (total == size) {
            size += 1024;
            buf = (char *) realloc (buf, size);
        }
    }

    MutText mutText = {
        .array  = buf,
        .length = total
    };

    return solidifyText (mutText);
}
