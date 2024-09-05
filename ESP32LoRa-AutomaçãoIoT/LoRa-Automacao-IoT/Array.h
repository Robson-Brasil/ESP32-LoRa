// Array.h
#ifndef ARRAY_H
#define ARRAY_H

const int RelayPins[] = {RelayPin1, RelayPin2, RelayPin3, RelayPin4, RelayPin5, RelayPin6, RelayPin7, RelayPin8};
const char* pubTopics[] = {pub0, pub1, pub2, pub3, pub4, pub5, pub6, pub7, pub8, pub9};
const char* subTopics[] = {sub0, sub1, sub2, sub3, sub4, sub5, sub6, sub7, sub8, sub9};
const int numRelays = sizeof(RelayPins) / sizeof(RelayPins[0]);

#endif
