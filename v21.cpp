#include <math.h>
#include <numbers>
#include "v21.hpp"

void V21_RX::demodulate(const float *in_analog_samples, unsigned int n)
{
    unsigned int digital_samples[n];

    for (unsigned int i = 0; i < n; ++i) {
        // Implemente seu código de demodulação FSK aqui

       
        float mark_bandpass = /* resultado do filtro passa-banda para o tom de marca */;
        float space_bandpass = /* resultado do filtro passa-banda para o tom de espaço */;

        // Calcula a diferença entre as amostras filtradas
        float diff_filtered = mark_bandpass - space_bandpass;

        // Aplica um filtro passa-baixas à diferença filtrada
        float diff_filtered_low_pass = low_pass_filter(diff_filtered);

        // Verifica se a energia na frequência de espaço é maior que na frequência de marca
        digital_samples[i] = (diff_filtered_low_pass > prev_diff_filtered) ? 0 : 1;
        prev_diff_filtered = diff_filtered_low_pass;

        // Atualiza a presença da portadora
        if (mark_bandpass > 0.0 || space_bandpass > 0.0) {
            carrier_present = true;
            consecutive_zeroes = 0;
        } else {
            // Incrementa o contador de zeros consecutivos
            consecutive_zeroes++;
            // Se não houver portadora por um tempo prolongado, assume que a portadora está ausente
            if (consecutive_zeroes > 100) {
                carrier_present = false;
            }
        }
    }

    // Chama get_digital_samples uma vez ao final do método
    get_digital_samples(digital_samples, n);
}

float V21_RX::low_pass_filter(float sample)
{
    // Implemente seu código de filtro passa-baixas aqui
    // Por enquanto, retorna a amostra original
    / Coeficiente de suavização
    const float alpha = 0.1;

    // Variável para armazenar a saída filtrada
    static float filtered_output = 0.0;

    // Aplica o filtro de média móvel
    filtered_output = alpha * sample + (1 - alpha) * filtered_output;

    return filtered_output;
}

void V21_TX::modulate(const unsigned int *in_digital_samples, float *out_analog_samples, unsigned int n)
{
    while (n--) {
        *out_analog_samples++ = sin(phase);
        phase += (*in_digital_samples++ ? omega_mark : omega_space) * SAMPLING_PERIOD;

        // Evita que phase cresça indefinidamente, o que causaria perda de precisão
        phase = remainder(phase, 2 * std::numbers::pi);
    }
}
