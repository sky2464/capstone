//C++ program to decrypt a string

#include <iostream>
using namespace std;

void encryption (char str[100])
{
   int i, x;


        for(i = 0; (i < 100 && str[i] != '\0'); i++)
        str[i] = str[i] - 7; //the key for Decryption is 7 that is added to ASCII value

         cout << "\nDecrypted string: " << str << endl;
         
}

int main()
{
   char str[100];
   cin >> str;

   encryption (str);
   


  
   return 0;
}