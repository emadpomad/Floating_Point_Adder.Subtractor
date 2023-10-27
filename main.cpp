#include <iostream>
#include <string.h>

using namespace std;
const int EXPONENT_SIZE = 8;
const int ADDER_NUMBER_SIZE = 25;//One extra bit for sign extension and one for the possible overflow.
const int INPUT_SIZE = 32;
const int EXPONENT_INDEX = 1;
const int FRACTION_INDEX = EXPONENT_INDEX + EXPONENT_SIZE;

bool *adderNumber1Register;
bool *adderNumber2Register;
bool *adderResultRegister;
bool *adderExponent1;
bool *adderExponent2;
bool adderRGS[3] = {0};
bool *adderExponentsDifference;


void
normalizeResult(bool *result, bool *exponent, int resultSize = ADDER_NUMBER_SIZE);

bool
integerAddition(bool *A, bool *B, int sizeOfA, int sizeOfB, bool *resultSpace, int resultSpaceSize, bool carryIn = 0);

void
integerSubtraction(bool *A, bool *B, int sizeOfA, int sizeOfB, bool *resultSpace, int resultSpaceSize);

void
toTwosComplement(bool *number, int numberSize);

void
floatingPointAddition(bool *number1, bool *number2, bool *resultSpace, bool *exponent1, bool *exponent2,
                      int numberSize = ADDER_NUMBER_SIZE,
                      int resultSize = ADDER_NUMBER_SIZE, int exponentSize = EXPONENT_SIZE);

void
floatingPointSubtraction(bool *number1, bool *number2, bool *resultSpace, bool *exponent1, bool *exponent2,
                         int numberSize = ADDER_NUMBER_SIZE,
                         int resultSize = ADDER_NUMBER_SIZE, int exponentSize = EXPONENT_SIZE);

bool shiftToLeft(bool *number, bool inputBit, int numberSize);

bool shiftToRight(bool *number, bool inputBit, int numberSize);

bool equalsZero(bool *number, int numberSize);


bool equalsOne(bool *number, int numberSize);

void roundResult(bool *result, int resultSize = ADDER_NUMBER_SIZE);

bool *populateWithZero(int size);

void printArray(bool *array, int size);

int main() {

    string numberAChr = "10101010001111000000011010100000";
    string numberBChr = "10101010001111000000011010011111";
    bool operation = 0;


    bool *input1Register = populateWithZero(INPUT_SIZE);
    bool *input2Register = populateWithZero(INPUT_SIZE);
    cout << "Enter first 32-bit number in the following format: [S(1-bit)][E(8-bits)][F(23-bits)]" << endl;
    cin >> numberAChr;
    cout << "Enter second 32-bit number in the following format: [S(1-bit)][E(8-bits)][F(23-bits)]" << endl;
    cin >> numberBChr;
    cout << "Enter your desired operation (0 for addition , 1 for subtraction)" << endl;
    cin >> operation;


    ////Initializing the registers from the input:
    for (int i = 0; i < INPUT_SIZE; ++i) {
        if (numberAChr[i] == '0') {
            input1Register[i] = 0;
        } else {
            input1Register[i] = 1;
        }

        if (numberBChr[i] == '0') {
            input2Register[i] = 0;
        } else {
            input2Register[i] = 1;
        }

    }

    bool *finalResult = populateWithZero(INPUT_SIZE);
    adderExponent1 = populateWithZero(EXPONENT_SIZE);
    adderExponent2 = populateWithZero(EXPONENT_SIZE);
    adderExponentsDifference = populateWithZero(EXPONENT_SIZE);
    adderNumber1Register = populateWithZero(ADDER_NUMBER_SIZE);
    adderNumber2Register = populateWithZero(ADDER_NUMBER_SIZE);
    adderResultRegister = populateWithZero(ADDER_NUMBER_SIZE);
    bool *adderResultExponent = populateWithZero(8);
    bool &adderResultSign = adderResultRegister[0];
    bool &adderNumber1IntegerBit = adderNumber1Register[1];
    bool &adderNumber2IntegerBit = adderNumber2Register[1];
    bool isExponentABiggerThanB = true;
    bool &input1Sign = input1Register[0];
    bool &input2Sign = input2Register[0];
    bool &finalResultSign = finalResult[0];

    if (input1Sign == 1) {
        cout << "-";
    } else {
        cout << "+";
    }

    printArray(input1Register, INPUT_SIZE);
    cout << endl;
    if (operation == 0) {
        cout << "+";
    } else {
        cout << "-";
    }
    cout << endl;




    if (input2Sign == 1) {
        cout << "-";
    } else {
        cout << "+";
    }

    printArray(input2Register, INPUT_SIZE);
    cout << endl;

    ////Initializing the exponent registers:
    for (int i = 0; i < EXPONENT_SIZE; ++i) {
        adderExponent1[i] = input1Register[i + 1];
        adderExponent2[i] = input2Register[i + 1];
    }


    ////Comparing the exponents in the adder:
    for (int i = 0; i < EXPONENT_SIZE; ++i) {
        if (adderExponent1[i] != adderExponent2[i]) {
            isExponentABiggerThanB = adderExponent1[i];
            break;
        }
    }

    ////Determining final adder exponent:
    if (isExponentABiggerThanB) {
        for (int i = 0; i < EXPONENT_SIZE; ++i) {
            adderResultExponent[i] = adderExponent1[i];
        }
    } else {
        for (int i = 0; i < EXPONENT_SIZE; ++i) {
            adderResultExponent[i] = adderExponent2[i];
        }
    }

    ////Determining the integer part of the inputs:
    for (int i = 0; i < EXPONENT_SIZE; ++i) {
        if (adderExponent1[i] == 1) {
            adderNumber1IntegerBit = 1;
            break;
        } else {
            adderNumber1IntegerBit = 0;
        }
    }
    for (int i = 0; i < EXPONENT_SIZE; ++i) {
        if (adderExponent2[i] == 1) {
            adderNumber2IntegerBit = 1;
            break;
        } else {
            adderNumber2IntegerBit = 0;
        }
    }


    ////Initializing the adder number registers:
    for (int i = 2; i < ADDER_NUMBER_SIZE; ++i) {
        adderNumber1Register[i] = input1Register[i - 2 + 9];
        adderNumber2Register[i] = input2Register[i - 2 + 9];
    }
    cout << endl;


    ////Determining which number is bigger(to decide about the final sign):
    bool isInput1BiggerThanOrEqualToInput2 = true;
    for (int i = 1; i < INPUT_SIZE; ++i) {
        if (input1Register[i] != input2Register[i]) {
            if (input1Register[i] == 0) {
                isInput1BiggerThanOrEqualToInput2 = false;
            }
            break;
        }
    }


    ////Determining the operation on numbers:
    if (operation == 0) {
        //Operation is addition
        if (input1Sign == input2Sign) {
            cout << "Adding..." << endl;
            // (-) + (-) or (+) + (+)
            finalResultSign = input1Sign;
            floatingPointAddition(adderNumber1Register, adderNumber2Register, adderResultRegister, adderExponent1,
                                  adderExponent2);
            printArray(adderResultRegister, ADDER_NUMBER_SIZE);
        } else {
            cout << "Subtracting..." << endl;
            //(+) + (-) or (-) + (+)
            floatingPointSubtraction(adderNumber1Register, adderNumber2Register, adderResultRegister, adderExponent1,
                                     adderExponent2);
            ////Negating the result if it is negative:
            if (adderResultSign == 1) {
                toTwosComplement(adderResultRegister, ADDER_NUMBER_SIZE);
            }

            ////Determining the final result sign*:
            if (isInput1BiggerThanOrEqualToInput2) {
                finalResultSign = input1Sign;
            } else {
                finalResultSign = !input1Sign;
            }

        }
    } else {
        //Operation is subtraction...
        if (input1Sign != input2Sign) {
            cout << "Adding..." << endl;
            // (-) + (-) or (+) + (+)
            finalResultSign = input1Sign;
            floatingPointAddition(adderNumber1Register, adderNumber2Register, adderResultRegister, adderExponent1,
                                  adderExponent2);
        } else {
            cout << "Subtracting..." << endl;
            //(+) + (-) or (-) + (+)
            floatingPointSubtraction(adderNumber1Register, adderNumber2Register, adderResultRegister, adderExponent1,
                                     adderExponent2);
            ////Negating the result if it is negative:
            if (adderResultSign == 1) {
                toTwosComplement(adderResultRegister, ADDER_NUMBER_SIZE);
            }

            ////Determining the final result sign*:
            if (isInput1BiggerThanOrEqualToInput2) {
                finalResultSign = input1Sign;
            } else {
                finalResultSign = !input1Sign;
            }

        }
    }

    normalizeResult(adderResultRegister, adderResultExponent);
    roundResult(adderResultRegister);
    normalizeResult(adderResultRegister, adderResultExponent);

    ////Determining the final result exponent:
    bool *finalResultExponent;
    if (adderResultRegister[1] == 1) {
        ////Number is normalized.
        finalResultExponent = adderResultExponent;
    } else {
        ////Number is de-normalized.
        finalResultExponent = populateWithZero(EXPONENT_SIZE);
    }

    ////Storing the final result in the finalResult register:
    for (int i = EXPONENT_INDEX; i < FRACTION_INDEX; ++i) {
        finalResult[i] = finalResultExponent[i - EXPONENT_INDEX];
    }
    for (int i = FRACTION_INDEX; i < INPUT_SIZE; ++i) {
        finalResult[i] = adderResultRegister[i - FRACTION_INDEX + 2];
    }

    ////Printing the final result:
    cout << "Final 32-bit number in S|E|F format :" << endl;
    for (int i = 0; i < INPUT_SIZE; ++i) {
        if (i == EXPONENT_INDEX || i == FRACTION_INDEX) {
            cout << "|";
        }
        cout << finalResult[i];
    }
    cout << endl;


}

bool
integerAddition(bool *A, bool *B, int sizeOfA, int sizeOfB, bool *resultSpace, int resultSpaceSize, bool carryIn) {

    if (sizeOfA > resultSpaceSize || sizeOfB > resultSpaceSize) {
        throw string("Can't fit the result into the result space");
    }

    int indexOfResult = resultSpaceSize - 1;

    bool *number1;
    bool *number2;//Smaller number
    int indexOfNumber1;
    int indexOfNumber2;

    if (sizeOfA > sizeOfB) {
        number1 = A;
        indexOfNumber1 = sizeOfA - 1;
        number2 = B;
        indexOfNumber2 = sizeOfB - 1;
    } else {
        number1 = B;
        number2 = A;
        indexOfNumber1 = sizeOfB - 1;
        indexOfNumber2 = sizeOfA - 1;
    }

    bool sum = 0;
    bool carry = carryIn;

    while (indexOfNumber2 >= 0) {

        bool a = number1[indexOfNumber1];
        bool b = number2[indexOfNumber2];
        sum = b ^ a ^ carry;
        carry = (b && a) || (b && carry) || (a && carry);
        resultSpace[indexOfResult] = sum;
        indexOfResult--;
        indexOfNumber1--;
        indexOfNumber2--;
    }

    if (indexOfResult == -1) {
        return carry;
    }

    while (indexOfNumber1 >= 0) {

        bool a = number1[indexOfNumber1];
        bool b = 0;
        sum = a ^ b ^ carry;
        carry = (b && a) || (b && carry) || (a && carry);
        resultSpace[indexOfResult] = sum;
        indexOfResult--;
        indexOfNumber1--;
        if (indexOfResult == -1) {
            return carry;
        }
    }

    resultSpace[indexOfResult] = carry;
    carry = 0;
    indexOfResult--;


    while (indexOfResult >= 0) {
//        bool a = resultSpace[indexOfResult];
//        bool b = 0;
//        sum = a ^ b ^ carry;
//        carry = (b && a) || (b && carry) || (a && carry);
        resultSpace[indexOfResult] = 0;
        indexOfResult--;
    }

    return carry;

}

void
toTwosComplement(bool *number, int numberSize) {
    bool one[1] = {1};
    for (int i = 0; i < numberSize; ++i) {
        number[i] = !number[i];
    }
    integerAddition(number, one, numberSize, 1, number, numberSize);
}

void
integerSubtraction(bool *A, bool *B, int sizeOfA, int sizeOfB, bool *resultSpace, int resultSpaceSize) {

    if (sizeOfB > sizeOfA) {
        toTwosComplement(B, sizeOfB);
        integerAddition(A, B, sizeOfA, sizeOfB, resultSpace, resultSpaceSize);
        if (*B != *resultSpace) {
            toTwosComplement(B, sizeOfB);
        }
    } else if (sizeOfA > sizeOfB) {
        toTwosComplement(A, sizeOfA);
        integerAddition(A, B, sizeOfA, sizeOfB, resultSpace, resultSpaceSize);
        if (*A != *resultSpace) {
            toTwosComplement(A, sizeOfA);
        }
        toTwosComplement(resultSpace, resultSpaceSize);
    } else {
        toTwosComplement(B, sizeOfB);
        integerAddition(A, B, sizeOfA, sizeOfB, resultSpace, resultSpaceSize);
        if (*B != *resultSpace) {
            toTwosComplement(B, sizeOfB);
        }
//        toTwosComplement(B,sizeOfB);
    }

}

bool
shiftToRight(bool *number, bool inputBit, int numberSize) {
    bool deletedBit = number[numberSize - 1];
    for (int i = numberSize - 1; i >= 0; i--) {
        if (i == 0) {
            number[i] = inputBit;
            return deletedBit;
        }
        number[i] = number[i - 1];
    }
    return deletedBit;
}

bool
shiftToLeft(bool *number, bool inputBit, int numberSize) {
    bool deletedBit = number[0];
    for (int i = 0; i < numberSize - 1; ++i) {
        number[i] = number[i + 1];
    }
    number[numberSize - 1] = inputBit;
    return deletedBit;
}

bool equalsZero(bool *number, int numberSize) {
    for (int i = 0; i < numberSize; ++i) {
        if (number[i] == 1) {
            return false;
        }
    }
    return true;
}

void
floatingPointAddition(bool *number1, bool *number2, bool *resultSpace, bool *exponent1, bool *exponent2, int numberSize,
                      int resultSize, int exponentSize) {

    bool isE1BiggerThanOrEqualToE2 = true;
    for (int i = 0; i < EXPONENT_SIZE; ++i) {
        if (exponent1[i] != exponent2[i]) {
            if (exponent1[i] == 0) {
                isE1BiggerThanOrEqualToE2 = false;
            }
            break;
        }
    }
    integerSubtraction(exponent1, exponent2, exponentSize, exponentSize, adderExponentsDifference, exponentSize);
    bool *A = number1;
    bool *B = number2;

    bool one[1] = {1};


    if (!isE1BiggerThanOrEqualToE2) {
        A = number2;
        B = number1;
        toTwosComplement(adderExponentsDifference, exponentSize);
    }


    while (!equalsZero(adderExponentsDifference, exponentSize)) {
        integerSubtraction(adderExponentsDifference, one, exponentSize, 1, adderExponentsDifference, exponentSize);
        bool roundBit = shiftToRight(B, 0, numberSize);
        bool deletedBit = shiftToRight(adderRGS, roundBit, 3);
        adderRGS[2] = adderRGS[2] || deletedBit;
    }

    integerAddition(A, B, numberSize, numberSize, resultSpace, resultSize, 0);

}

bool equalsOne(bool *number, int numberSize) {
    bool output = number[numberSize - 1];
    for (int i = numberSize - 2; i >= 0; --i) {
        output = output & !number[i];
    }
    return output;
}

void normalizeResult(bool *result, bool *exponent, int resultSize) {
    bool one[1] = {1};
    bool isNumberNormalized = (result[1] == 1);
    bool numberNeedsShiftToRight = (result[0] == 1);
    if (numberNeedsShiftToRight) {
        bool roundBit = shiftToRight(result, 0, resultSize);
        bool deletedBit = shiftToRight(adderRGS, roundBit, 3);
        integerAddition(exponent, one, EXPONENT_SIZE, 1, exponent, EXPONENT_SIZE);
        adderRGS[2] = adderRGS[2] || deletedBit;
    } else {
        if (!isNumberNormalized) {
            while (!equalsOne(exponent, EXPONENT_SIZE) && !equalsZero(exponent, EXPONENT_SIZE)) {
                isNumberNormalized = result[1];
                if (isNumberNormalized) {
                    break;
                }
                bool inputBit = shiftToLeft(adderRGS, 0, 3);
                shiftToLeft(result, inputBit, resultSize);
                integerSubtraction(exponent, one, EXPONENT_SIZE, 1, exponent, EXPONENT_SIZE);
            }
        }
    }
}

void roundResult(bool *result, int resultSize) {
    bool one[1] = {1};
    if (adderRGS[0] == 1) {
        if (adderRGS[1] == 1) {
            integerAddition(result, one, resultSize, 1, result, resultSize);
        } else {
            if (adderRGS[2] == 1) {
                integerAddition(result, one, resultSize, 1, result, resultSize);
            } else {
                if (result[resultSize - 1] == 1) {
                    integerAddition(result, one, resultSize, 1, result, resultSize);
                }
            }
        }
    }
    adderRGS[0] = 0;
    adderRGS[1] = 0;
    adderRGS[2] = 0;
}

void
floatingPointSubtraction(bool *number1, bool *number2, bool *resultSpace, bool *exponent1, bool *exponent2,
                         int numberSize, int resultSize, int exponentSize) {

    integerSubtraction(exponent1, exponent2, exponentSize, exponentSize, adderExponentsDifference, exponentSize);
    bool *A = number1;
    bool *B = number2;

    bool one[1] = {1};

    if (adderExponentsDifference[0] == 1) {
        A = number2;
        B = number1;
        toTwosComplement(adderExponentsDifference, exponentSize);
    }



    //Shifting the smaller number:
    while (!equalsZero(adderExponentsDifference, exponentSize)) {
        integerSubtraction(adderExponentsDifference, one, exponentSize, 1, adderExponentsDifference, exponentSize);
        bool roundBit = shiftToRight(B, 0, numberSize);
        bool deletedBit = shiftToRight(adderRGS, roundBit, 3);
        adderRGS[2] = adderRGS[2] || deletedBit;
    }

    //Flipping the bits of number2(B and adderRGS):
    for (int i = 0; i < 3; ++i) {
        adderRGS[i] = !adderRGS[i];
    }
    for (int i = 0; i < numberSize; ++i) {
        B[i] = !B[i];
    }

    //Adding one to number2(B and adderRGS) to get the 2's complement of the number2:
    bool zero[] = {0};
    bool carry = integerAddition(adderRGS, one, 3, 1, adderRGS, 3);
    integerAddition(B, zero, numberSize, 1, B, numberSize, carry);

    //Performing A-B:
    integerAddition(A, B, numberSize, numberSize, resultSpace, resultSize, 0);


}

void printArray(bool *array, int size) {
    for (int i = 0; i < size; ++i) {
        cout << array[i];
    }
}

bool *populateWithZero(int size) {

    bool *array = new bool[size];
    for (int i = 0; i < size; ++i) {
        array[i] = 0;
    }
    return array;

}

