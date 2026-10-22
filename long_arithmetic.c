#include <stdio.h>
#include <string.h>
#define BASE 10000
#define MAX_BLOCKS 1000

typedef struct { /*Структура числа*/
	short digits[MAX_BLOCKS];
	int len;
	int sign;
} BigInt;

void readBigInt(BigInt *num) { /*Чтение числа*/
	char input[1002];
	int start = 0;
	scanf_s("%1001s", input, (unsigned)sizeof(input));
	if (input[0] == '-') {
		start = 1;
		num->sign = -1;
	}
	else {
		num->sign = 1;
	}
	int length = strlen(input);
	int block = 0;
	int multiplier = 1;
	int count = 0;
	int index = 0;
	for (int i = length - 1;i >= start;i--) {
		int digit = input[i] - '0';
		block += digit * multiplier;
		multiplier *= 10;
		count++; 
		
		if (count == 4) {
			num->digits[index] = block;
			block = 0;
			multiplier = 1;
			count = 0;
			index++;
		}
	}
	if (count > 0) {
		num->digits[index] = block;
		index++;
	}
	num->len = index;

}

void printBigInt(BigInt* num) { /*Вывод числа*/
	printf("Result=");
	if (num->sign == -1) {
		printf("-");
	}
	printf("%d", num->digits[num->len - 1]);
	for (int i = num->len - 2;i >= 0;i--) {
		printf("%04d", num->digits[i]);
	}
}

int compareAbs(BigInt* a, BigInt* b) { /*Сравнение модулей*/
	if (a->len > b->len) {
		return 1;
	}
	else if (a->len < b->len) {
		return -1;
	}
	for (int i = a->len - 1;i >= 0;i--) {
		if (a->digits[i] > b->digits[i]) {
			return 1;
		}
		else if (a->digits[i] < b->digits[i]) {
			return -1;
		}
	}
	return 0;
}

void substractAbs(BigInt* a, BigInt* b, BigInt* result) {/*Вычитание модулей*/
	int borrow = 0;
	result->len = a->len;
	result->sign = 1;

	for (int i = 0; i < a->len;i++) {
		int x = a->digits[i];
		int y;
		if (i < b->len) {
			y = b->digits[i];
		}
		else {
			y = 0;
		}
		int diff = x - y - borrow;
		if (diff < 0) {
			diff += BASE;
			borrow = 1;
		}
		else {
			borrow = 0;
		}
		result->digits[i] = diff;
	}
	while (result->len > 1 && result->digits[result->len - 1] == 0) {
		result->len--;
	}
}

void addBigInt(BigInt* a, BigInt* b, BigInt* result) { /*Сложение чисел*/
	if (a->sign != b->sign) {
		int cmp = compareAbs(a, b);
		if (cmp == 0) {
			result->digits[0] = 0;
			result->len = 1;
			result->sign = 1;
			return;
		}
		if (cmp > 0) {
			substractAbs(a, b, result);
			result->sign = a->sign;
		}
		else {
			substractAbs(b, a, result);
			result->sign = b->sign;
		}
		return;
	}
	int maxLen = (a->len > b->len) ? a->len : b->len;
	int carry = 0;
	result->len = maxLen;
	result->sign = a->sign;
	if (a->sign == -1 && b->sign == -1) {
		result->sign = -1;
	}
	else {
		result->sign = 1;
	}
	for (int i = 0; i < maxLen;i++) {
		int x;
		if (i < a->len) {
			x = a->digits[i];
		}
		else {
			x = 0;
		}
		int y;
		if (i < b->len) {
			y = b->digits[i];
		}
		else {
			y = 0;
		}
		int sum = x + y + carry;
		result->digits[i] = sum % BASE;
		carry = sum / BASE;
	
		}
	if (carry > 0) {
		result->digits[maxLen] = carry;
		result->len++;
	}
}

void substractBigInt(BigInt* a, BigInt* b, BigInt* result) {/*Вычитание чисел*/
	if (a->sign != b->sign) {
		BigInt tempA = *a;
		BigInt tempB = *b;
		tempA.sign = 1;
		tempB.sign = 1;

		addBigInt(&tempA, &tempB, result);
		result->sign = a->sign;
		return;
	}
	int cmp = compareAbs(a, b);
	if (cmp == 0) {
		result->digits[0] = 0;
		result->len = 1;
		result->sign = 1;
		return;
	}
	if (cmp > 0) {
		substractAbs(a, b, result);
		result->sign = a->sign;
	}
	else {
		substractAbs(b, a, result);
		result->sign = -a->sign;
	}
}

void multiplyBigInt(BigInt* a, BigInt* b, BigInt* result) { /*Умножение чисел*/
	result->len = a->len + b->len;
	result->sign = a->sign * b->sign;
	for (int i = 0;i < result->len;i++) {
		result->digits[i] = 0;
	}
	for (int i = 0;i < a->len;i++) {
		int carry = 0;
		for (int j = 0;j < b->len;j++) {
			int current = result->digits[i + j] 
				+ a->digits[i] * b->digits[j] 
				+ carry;
			result->digits[i + j] = current % BASE;
			carry = current / BASE;
		}
		int pos = i + b->len;
		while (carry > 0) {
			int current = result->digits[pos] + carry;
			result->digits[pos] = current % BASE;
			carry = current / BASE;
			pos++;
		}
	}
	while (result->len > 1 && result->digits[result->len - 1] == 0) {
		result->len--;
	}
	if (result->len == 1 && result->digits[0] == 0) {
		result->sign = 1;
	}
}

void powerBigInt(BigInt* a, BigInt* b, BigInt* result) {/*Степень*/
	int power = b->digits[0];
	result->digits[0] = 1;
	result->len = 1;
	result->sign = 1;

	for (int i = 0;i < power;i++) {
		BigInt temp;
		multiplyBigInt(result, a, &temp);
		*result = temp;
	}
}

void factorialBigInt(BigInt* a, BigInt* result){/*Факториал*/ 
	int n = a->digits[0];
	result->digits[0] = 1;
	result->len = 1;
	result->sign = 1;

	for (int i = 2;i <= n;i++) {
		BigInt temp;
		BigInt num;
		num.digits[0] = i;
		num.len = 1;
		num.sign = 1;
		multiplyBigInt(result, &num, &temp);
		*result = temp;

	}
}

void divideBy2(BigInt* num) {
	int remainder = 0;

	for (int i = num->len - 1;i >= 0;i--) {
		int current = remainder * BASE + num->digits[i];
		num->digits[i] = current / 2;
		remainder = current % 2;
	}

	while (num->len > 1 && num->digits[num->len - 1] == 0) {
		num->len--;
	}

	if (num->len == 1 && num->digits[0] == 0){
		num->sign = 1;
	}
}

void sumBigInt(BigInt* a, BigInt* b, BigInt* result) {
	BigInt sum;
	BigInt dif;
	BigInt count;
	BigInt product;
	BigInt one;

	one.digits[0] = 1;
	one.len = 1;
	one.sign = 1;

	addBigInt(a, b, &sum);

	substractBigInt(b, a, &dif);

	addBigInt(&dif, &one, &count);

	multiplyBigInt(&sum, &count, &product);

	divideBy2(&product);

	*result = product;
	
}
int main(void) {
	BigInt a,b,result;
	char op;
	scanf_s("%c", &op);
	readBigInt(&a);
	readBigInt(&b);
	switch (op) {
	case '+' :
		addBigInt(&a, &b, &result);
		break;
	case '-':
		substractBigInt(&a, &b, &result);
		break;
	case '*':
		multiplyBigInt(&a, &b, &result);
		break;
	case '^':
		powerBigInt(&a, &b, &result);
		break;
	case '!':
		factorialBigInt(&a, &result);
		break;
	case 'S':
	case 's':
		sumBigInt(&a, &b, &result);
		break;
	}
	
	printBigInt(&result);
	return 0;
}
