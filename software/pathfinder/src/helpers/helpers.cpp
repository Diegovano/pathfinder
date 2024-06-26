#include "helpers.h"

std::string decToBinary(int n)
{
    // Array to store binary number
    int binaryNum[10];
 
    // Counter for binary array
    int i = 0;
    while (n > 0 && i < 10) {
        // Storing remainder in binary
        // array
        binaryNum[i] = n % 2;
        n = n / 2;
        i++;
    }
 
    std::stringstream myStr;

    // Printing binary array in reverse
    // order
    for (int j = i - 1; j >= 0; j--)
        myStr << binaryNum[j];

    return myStr.str();
}

int* cpyArray(int *src, int size)
{
    int *cpy = new int[size];

    for (int i = 0; i < size; i++) cpy[i] = src[i];

    return cpy;
}
