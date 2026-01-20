#pragma once

#include <Arduino.h>
#include <Devices/Actuators/Actuator.h>

struct BuzzerWithFeedbackConfig {
    const char* friendlyName = "BuzzerFB";
    const char* location = "room-name";

    // Si NO tienes pull-down externo en el nodo, usa true (INPUT_PULLDOWN).
    // Si tienes pull-down externo, usa false (INPUT).
    bool useInternalPulldown = true;

    // Antirrebote
    uint32_t debounceMs = 40;

    // Log por serial cuando cambie el feedback
    bool serialLog = true;
};

class BuzzerWithFeedback : public Actuator {
public:
    explicit BuzzerWithFeedback(uint8_t outPin, uint8_t feedbackPin,
                                const BuzzerWithFeedbackConfig& cfg = {});

    bool begin() override;
    void loop() override;

    const char* name() const override { return friendlyName_.c_str(); }
    const char* location() const override { return location_.c_str(); }

    bool isActive() const override { return active_; }
    String stateString() const override { return active_ ? "ON" : "OFF"; }

    bool applyCommand(const char* command) override;

private:
    void updateFromFeedback_();
    void logState_(bool on);

    uint8_t outPin_;
    uint8_t feedbackPin_;

    String friendlyName_;
    String location_;

    bool useInternalPulldown_{true};
    uint32_t debounceMs_{40};
    bool serialLog_{true};

    // Estado real (dictado por feedback)
    bool active_{false};

    // Antirrebote
    bool lastRaw_{false};
    bool stable_{false};
    uint32_t lastEdgeMs_{0};
};
