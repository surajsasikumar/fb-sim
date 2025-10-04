#include "FBSIM.H"
#include <iostream>
#include <unistd.h>
#include <ctime>
using namespace std;

float aGoal = 2.33f;
int rep = -1;
double simSlow = 1.6;

int main() {
 srand((unsigned)time(nullptr)); // seed randomness ONCE

 int ch; char ch1;
 cout << "welcome to crxbyt soccer club management\n";
 do {
  cout << "1.manage \n2.play match\n";
  cout << "enter your choice: ";
  cin >> ch;
  switch (ch) {
   case 1: intro(); break;
   case 2: playMatch(); break;
  }
  cout << "do you want to continue (y/n): ";
  cin >> ch1;
 } while (ch1 == 'y' || ch1 == 'Y');
 return 0;
}
