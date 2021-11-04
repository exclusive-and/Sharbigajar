
#ifndef TEXT_H
#define TEXT_H

// Mutable text object.
//
typedef struct MutText MutText;

struct MutText {
    char *array;
    unsigned int length;
};

MutText newMutText (unsigned int);
void freeMutText (MutText);


// Immutable text object.
//
typedef struct Text Text;

struct Text {
    const char *array;
    unsigned int length;
};

Text solidifyText (MutText);
void freeText (Text);


Text textFromString (const char []);
Text copyTextFromString (const char []);


Text concatText (Text, Text);
Text appendText (Text, const char []);


Text textFromFile (Text);

#endif


