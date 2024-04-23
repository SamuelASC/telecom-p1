#ifndef UART_HPP
#define UART_HPP

#include <functional>
#include <deque>
#include <mutex>
#include <stdint.h>
#include "config.hpp"

class UART_RX
{
public:
    UART_RX(std::function<void(uint8_t)> get_byte) : get_byte(get_byte) {}
    void put_samples(const unsigned int *buffer, unsigned int n);

private:
    std::function<void(uint8_t)> get_byte;
    bool detecting_start_bit = false;
    unsigned int bit_count = 0;
    uint8_t received_byte = 0;

    bool detect_start_bit(const unsigned int *buffer, unsigned int n);
    void receive_data_bits(const unsigned int *buffer, unsigned int n);
    void detect_stop_bit(const unsigned int *buffer, unsigned int n);
};

class UART_TX
{
public:
    void put_byte(uint8_t byte);
    void get_samples(unsigned int *buffer, unsigned int n);

private:
    std::deque<unsigned int> samples;
    std::mutex samples_mutex;
    void put_bit(unsigned int bit);
};

#endif
