#ifndef COMMON_H
#define COMMON_H

namespace sim {

// Sensor and actuator neuron indices
enum NeuronID {
    // Sensor Neurons (Inputs)
    SENSOR_LOC_X = 0,
    SENSOR_LOC_Y,
    SENSOR_AGE,
    SENSOR_RANDOM,
    
    NUM_SENSORS,

    // Actuator Neurons (Outputs)
    ACTION_MOVE_FWD = NUM_SENSORS,
    ACTION_TURN_RIGHT,
    ACTION_TURN_LEFT,
    
    NUM_ACTIONS,

    FIRST_INTERNAL_NEURON = NUM_SENSORS + NUM_ACTIONS
};

} // namespace sim

#endif // COMMON_H
