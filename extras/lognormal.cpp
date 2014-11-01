// lognormal_distribution
#include <iostream>
#include <random>

int main()
{
  const int nrolls=10000;  // number of experiments
  const int nstars=100;    // maximum number of stars to distribute

  std::default_random_engine generator;
  std::lognormal_distribution<double> distribution(0.50, 0.85);

  int p[50]={};
  int in=0, out=0;

  for (int i=0; i<nrolls; ++i) {
    double number = distribution(generator);
   
    if ((number>=.1)&&(number<1)) {
      in++;
      //printf("%f\n", number);
      ++p[int(number)];
    }
    else
      out++;
  }
  printf("IN: %d OUT: %d\n\n", in, out);

  std::cout << "lognormal_distribution (0.0,1.0):" << std::endl;

  for (int i=0; i<10; ++i) {
    std::cout << i << "-" << (i+1) << ": ";
    std::cout << std::string(p[i]*nstars/nrolls,'*') << std::endl;
  }

  return 0;
}