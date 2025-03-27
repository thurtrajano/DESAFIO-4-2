#ifndef __HCF_MP_H
    #define __HCF_MP_H
   // #include "esp_err.h"
 

    void iniciar_MP(void);
    void rotacionar_MP(int horario, float graus);

    void iniciar_driver(int Fim_de_curso_inicial, int Fim_de_curso_final);
    void rotacionar_driver(int abrir, float graus, uint8_t saidas_atual);
#endif