#pragma once
#include <Arduino.h>

struct TopicInfo{
    String action;
    String device;
    String type;
    int id;
    bool valid;
};

class TopicParser {
    public:
        static TopicInfo parse(const String &topic);
};