#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void printMatrix();
void fillMatrix(int arr[], int height, int width);

int main(void) {
  int velocidades_empleados[256];
  int velocidad_dron = 0;

  // ---largo----
  //            |
  //            |
  //            ancho
  //            |
  //            |
  int largo_parcela = 0;
  int ancho_parcela = 0;
  int ticks_por_segundo = 1;
  printf("Ingresa el largo y ancho de la parcela:\n");
  fscanf(stdin, "%d %d", &largo_parcela, &ancho_parcela);
  fgetc(stdin); // Quita el \n del final

  int index = 0;
  printf("Ingresa las velocidades de los empleados:\n");
  while ((fscanf(stdin, "%d", &velocidades_empleados[index])) == 1) {
    fgetc(stdin); // Quitar el \n del final.
    index++;
  }
  fgetc(stdin); // Quitar \n después del '-'
  printf("Ingresa la velocidad del dron:\n");
  fscanf(stdin, "%d", &velocidad_dron);
  printf("Ingresa los ticks por segundo:\n");
  fscanf(stdin, "%d", &ticks_por_segundo);

  for (int i = 0; i < index; i++) {
    printf("La velocidad del empleado %d es %d\n", i, velocidades_empleados[i]);
  }

  int empleados_terminaron = 0;
  int dron_termino = 0;

  int dron_tick_count = 0;
  int empleados_tick_count = 0;

  int parcela_empleados[largo_parcela * ancho_parcela];
  int parcela_dron[largo_parcela * ancho_parcela];
  uint wait_seconds = 10 / ticks_por_segundo;
  int total_a_fumigar = largo_parcela * ancho_parcela;

  fillMatrix(parcela_dron, largo_parcela, ancho_parcela);
  fillMatrix(parcela_empleados, largo_parcela, ancho_parcela);

  int secciones_por_tick = 0;
#pragma omp parallel for reduction(+ : secciones_por_tick)
  for (int empleadoI = 0; empleadoI < index; empleadoI++) {
    secciones_por_tick += velocidades_empleados[empleadoI];
  }

  printf("Los empleados en conjunto fumigan %d secciones por tick\n",
         secciones_por_tick);

#pragma omp parallel sections shared(empleados_terminaron, dron_termino,       \
                                     parcela_dron, parcela_empleados)
  {
#pragma omp section
    {
      // Mostrar matrices...
      while (!empleados_terminaron && !dron_termino) {
        // TODO Limpiar la pantalla
        printf("\033[2H");
        // Mostrar parcela empleados
        printf("\033[92mParcela empleados:\n");
        for (int i = 0; i < ancho_parcela; i++) {
          for (int j = 0; j < largo_parcela; j++) {
            int celda_fumigada = parcela_empleados[ancho_parcela * i + j];

            if (celda_fumigada) {
              // TODO Imprimir con color
              printf("\033[31m 1 ");
            } else {
              printf("\033[91m 0 ");
            }
          }
          printf("\n");
        }
        // alternativa:
        // printMatrix(parcela_dron, largo_parcela, ancho_parcela)

        // TODO Mostrar parcela dron...
        printf("\nParcela dron:\n");
        for (int i = 0; i < ancho_parcela; i++) {
          for (int j = 0; j < largo_parcela; j++) {
            int celda_fumigada = parcela_empleados[ancho_parcela * i + j];

            if (celda_fumigada) {
              // TODO Imprimir con color
              printf("1 ");
            } else {
              printf("0 ");
            }
          }
          printf("\n");
        }
      }

      printf("Mostrando parcelas...");
    }

#pragma omp section
    {
      int seccion_sin_fumigar = 0;
      // Drone fumiga parcela...
      while (!dron_termino) {
        for (dron_tick_count += 1; seccion_sin_fumigar <= secciones_por_tick;
             seccion_sin_fumigar++) {
          parcela_dron[seccion_sin_fumigar] = 1;
          dron_termino = (total_a_fumigar - 1);
          if (dron_termino) {
            break;
          }
        }

        sleep(wait_seconds);
      }
    }

#pragma omp section
    {
      int primera_seccion_sin_fumigar = 0;

      while (!empleados_terminaron) {
        empleados_tick_count += 1;

        // Fumigar
        for (; primera_seccion_sin_fumigar <= secciones_por_tick;
             primera_seccion_sin_fumigar++) {
          parcela_empleados[primera_seccion_sin_fumigar] = 1;
          empleados_terminaron =
              primera_seccion_sin_fumigar == (total_a_fumigar - 1);

          if (empleados_terminaron) {
            break;
          }
        }

        sleep(wait_seconds);
      }
    }
  }

  return 0;
}

void printMatrix(int arr[], int height, int width) {
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int index = i * width + j;
      printf("%d ", arr[index]);
    }
    printf("\n"); // Moverse a la siguiente fila
  }
}

void fillMatrix(int arr[], int height, int width) {
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      arr[i * width + j] = 0;
    }
  }
}
