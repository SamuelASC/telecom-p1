#include "uart.hpp"

void UART_RX::put_samples(const unsigned int *buffer, unsigned int n)
{
    if (!detecting_start_bit)
    {
        detecting_start_bit = detect_start_bit(buffer, n);
    }
    else
    {
        receive_data_bits(buffer, n);
        detect_stop_bit(buffer, n);
    }
}

bool UART_RX::detect_start_bit(const unsigned int *buffer, unsigned int n)
{
    unsigned int consecutive_low_samples = 0;
    for (unsigned int i = 0; i < n; ++i)
    {
        if (buffer[i] == 0)
        {
            consecutive_low_samples++;
            if (consecutive_low_samples >= 25)
            {
                if (i >= 30 && buffer[i - 30] == 0)
                {
                    bit_count = 0;
                    received_byte = 0;
                    return true; // Start bit detected
                }
            }
        }
        else
        {
            consecutive_low_samples = 0;
        }
    }
    return false;
}

void UART_RX::receive_data_bits(const unsigned int *buffer, unsigned int n)
{
    unsigned int bit_samples = SAMPLES_PER_SYMBOL * 160; // 160 times faster clock
    for (unsigned int i = 0; i < n; ++i)
    {
        if (bit_samples % SAMPLES_PER_SYMBOL == 0)
        {
            received_byte |= (buffer[i] << bit_count);
            bit_count++;
            if (bit_count == 8)
            {
                get_byte(received_byte);
                detecting_start_bit = false;
                return;
            }
        }
        bit_samples++;
    }
}

void UART_RX::detect_stop_bit(const unsigned int *buffer, unsigned int n)
{
    for (unsigned int i = 0; i < n; ++i)
    {
        if (buffer[i] == 1)
        {
            detecting_start_bit = false;
            return;
        }
    }
}

void UART_TX::put_byte(uint8_t byte)
{
    samples_mutex.lock();
    put_bit(0); // start bit
    for (int i = 0; i < 8; i++)
    {
        put_bit(byte & 1);
        byte >>= 1;
    }
    put_bit(1); // stop bit
    samples_mutex.unlock();
}

void UART_TX::get_samples(unsigned int *buffer, unsigned int n)
{
    samples_mutex.lock();
    std::vector<unsigned int>::size_type i = 0;
    while (!samples.empty() && i < n)
    {
        buffer[i++] = samples.front();
        samples.pop_front();
    }
    samples_mutex.unlock();

    while (i < n)
    {
        // idle
        buffer[i++] = 1;
    }
}

void UART_TX::put_bit(unsigned int bit)
{
    for (int i = 0; i < SAMPLES_PER_SYMBOL; i++)
    {
        samples.push_back(bit);
    }
}
