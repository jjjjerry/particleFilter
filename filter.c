#include <random>
#include <stdio.h>
#include <cmath>
#include <numeric>

int main(int argc, char **argv) {
  printf("Hello world\n");
  float x = 0.1;
  float z;
  float x_N = 1;
  float x_R = 1;
  int T = 200;
  int N = 100;

  float V = 10;
  float x_p[N];
  float x_p_update[N];
  float z_update[N];
  float p_w[N];
  float cumsum[N];
  float sum;

  std::default_random_engine generator;
  std::normal_distribution<float> normal_start(0, V);
  std::normal_distribution<float> normal_process(0, x_N);
  std::normal_distribution<float> normal_meas(0, x_R);
  std::uniform_real_distribution<float> uniform_resamp(0, 1);

  float z_out = x*x/20 + normal_meas(generator);
  float x_out = x;
  float x_est = x;
  float x_est_out = x_est;

  for (int i = 0; i < N; i++) {
    x_p[i] = x + normal_start(generator);
  }

  for(int t = 0; t < T; t++) {
    x = 0.5*x + 25*x/(1 + x*x) + 8*cos(1.2*t) + normal_process(generator);
    z = x*x/20 + normal_meas(generator);
    sum = 0.0f;
    for(int i = 0; i < N; i++) {
      x_p_update[i] = 0.5*x_p[i] + 25*x_p[i]/(1+x_p[i]*x_p[i]) + 8*cos(1.2*t) + normal_process(generator);
      z_update[i] = x_p_update[i]*x_p_update[i]/20;
      p_w[i] = (1/sqrt(2*M_PI*x_R)) * exp(-1*pow(z - z_update[i],2)/(2*x_R));
      sum = sum + p_w[i];
    }
    for(int i = 0; i < N; i++) {
      p_w[i] = p_w[i]/sum;
    }
    std::partial_sum(p_w, p_w+N, cumsum);
    sum = 0.0f;
    for(int i = 0; i < N; i++) {
      float uniform = uniform_resamp(generator);
      for(int j = 0; j < N; j++) {
        if(uniform <= cumsum[j]) {
          x_p[i] = x_p_update[j];
          sum = sum + x_p[i];
          break;
        }
      }
    }
    x_est = sum/N;
    printf("estimation: %4.2f real: %4.2f \n",x_est,x);
  }
}
