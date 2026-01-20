#include <Devices/Actuators/BuzzerWithFeedback.h>

BuzzerWithFeedback::BuzzerWithFeedback(uint8_t outPin, uint8_t feedbackPin,
                                       const BuzzerWithFeedbackConfig& cfg)
    : outPin_(outPin),
      feedbackPin_(feedbackPin),
      friendlyName_(cfg.friendlyName),
      location_(cfg.location),
      useInternalPulldown_(cfg.useInternalPulldown),
      debounceMs_(cfg.debounceMs),
      serialLog_(cfg.serialLog) {}

bool BuzzerWithFeedback::begin() {
    pinMode(outPin_, OUTPUT);
    digitalWrite(outPin_, LOW);

    if (useInternalPulldown_) {
        pinMode(feedbackPin_, INPUT_PULLDOWN);
    } else {
        pinMode(feedbackPin_, INPUT);
    }

    lastRaw_ = digitalRead(feedbackPin_);
    stable_  = lastRaw_;
    active_  = stable_;
    logState_(active_);

    return true;
}

void BuzzerWithFeedback::loop() {
    updateFromFeedback_();
}

void BuzzerWithFeedback::updateFromFeedback_() {
    const bool raw = digitalRead(feedbackPin_);

    if (raw != lastRaw_) {
        lastRaw_ = raw;
        lastEdgeMs_ = millis();
    }

    if ((millis() - lastEdgeMs_) >= debounceMs_ && raw != stable_) {
        stable_ = raw;
        active_ = stable_;
        logState_(active_);
    }
}

void BuzzerWithFeedback::logState_(bool on) {
    if (!serialLog_) return;

    Serial.print("[FB] ");
    Serial.print(friendlyName_);
    Serial.print(" (");
    Serial.print(location_);
    Serial.print(") -> ");
    Serial.println(on ? "ON" : "OFF");
}

// Comandos desde MQTT SET (mando). El estado reportado lo dicta el feedback.
bool BuzzerWithFeedback::applyCommand(const char* command) {
    if (strcmp(command, "ON") == 0) {
        digitalWrite(outPin_, HIGH);
        return true;
    }

    if (strcmp(command, "OFF") == 0) {
        digitalWrite(outPin_, LOW);
        return true;
    }

    if (strcmp(command, "BEEP") == 0) {
        tone(outPin_, 1000, 300);
        return true;
    }

    return false;
}
