#include <stdio.h>
#define BASE 65536
#define MAX_BLOCKS 208

typedef struct { /*Структура числа*/
	unsigned short digits[MAX_BLOCKS];
	unsigned char ls[2];
} BigInt;

int mulSmallAdd(BigInt* num, int m, int add) {
	int carry = add;
	for (int i = 0; i < num->ls[0]; i++) {
		int current = num->digits[i] * m + carry;
		num->digits[i] = current % BASE;
		carry = current / BASE;
	}
	if (carry > 0) {
		if (num->ls[0] >= MAX_BLOCKS) {
			printf("Error: array index out of bounds");
			return 1;
		}
		num->digits[num->ls[0]] = carry;
		num->ls[0]++;
	}
	return 0;
}

int divSmall(BigInt* num,  int d) {
	 int remainder = 0;
	for (int i = num->ls[0] - 1; i >= 0; i--) {
		 int current = remainder * BASE + num->digits[i];
		num->digits[i] = current / d;
		remainder = current % d;
	}
	while (num->ls[0] > 1 && num->digits[num->ls[0] - 1] == 0) {
		num->ls[0]--;
	}
	return remainder;
}

int readBigInt(BigInt* num) { /*Чтение числа*/
	char input[1002];
	int start = 0;
	scanf_s("%1001s", input, (unsigned)sizeof(input));
	if (input[0] == '-') {
		start = 1;
		num->ls[1] = 0;
	}
	else {
		num->ls[1] = 1;
	}
	num->digits[0] = 0;
	num->ls[0] = 1;
	for (int i = start; input[i] != '\0'; i++) {
		if (mulSmallAdd(num, 10, input[i] - '0')) {
			return 1;
		}
	}
	return 0;
}

void printBigInt(BigInt* num) { /*Вывод числа*/
	BigInt temp = *num;
	int parts[251]; 
	int count = 0;
	while (!(temp.ls[0] == 1 && temp.digits[0] == 0)) {
		parts[count] = divSmall(&temp, 10000);
		count++;
	}
	printf("Result=");
	if (count == 0) {
		printf("0");
		return;
	}
	if (num->ls[1] == 0) {
		printf("-");
	}
	printf("%u", parts[count - 1]);
	for (int i = count - 2; i >= 0; i--) {
		printf("%04u", parts[i]);
	}
}

int compareAbs(BigInt* a, BigInt* b) { /*Сравнение модулей*/
	if (a->ls[0] > b->ls[0]) {
		return 1;
	}
	else if (a->ls[0] < b->ls[0]) {
		return -1;
	}
	for (int i = a->ls[0] - 1;i >= 0;i--) {
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
	result->ls[0] = a->ls[0];
	result->ls[1] = 1;

	for (int i = 0; i < a->ls[0];i++) {
		int x = a->digits[i];
		int y;
		if (i < b->ls[0]) {
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
	while (result->ls[0] > 1 && result->digits[result->ls[0] - 1] == 0) {
		result->ls[0]--;
	}
}

int addBigInt(BigInt* a, BigInt* b, BigInt* result) { /*Сложение чисел*/
	if (a->ls[1] != b->ls[1]) {
		int cmp = compareAbs(a, b);
		if (cmp == 0) {
			result->digits[0] = 0;
			result->ls[0] = 1;
			result->ls[1]  = 1;
			return 0;
		}
		if (cmp > 0) {
			substractAbs(a, b, result);
			result->ls[1] = a->ls[1];
		}
		else {
			substractAbs(b, a, result);
			result->ls[1] = b->ls[1];
		}
		return 0;
	}
	else if (a->ls[1] == 0 && b->ls[1] == 0) {
		result->ls[1] = 0;
	}
	else {
		result->ls[1] = 1;
	}
	int maxLen = (a->ls[0] > b->ls[0]) ? a->ls[0] : b->ls[0];
	int carry = 0;
	result->ls[0] = maxLen;
	for (int i = 0; i < maxLen;i++) {
		int x;
		if (i < a->ls[0]) {
			x = a->digits[i];
		}
		else {
			x = 0;
		}
		int y;
		if (i < b->ls[0]) {
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
		if (maxLen >= MAX_BLOCKS) {
			printf("Error: array index out of bounds");
			return 1;
		}
		result->digits[maxLen] = carry;
		result->ls[0]++;
	}
	return 0;
}

int substractBigInt(BigInt* a, BigInt* b, BigInt* result) {/*Вычитание чисел*/
	if (a->ls[1] != b->ls[1]) {
		BigInt tempA = *a;
		BigInt tempB = *b;
		tempA.ls[1] = 1;
		tempB.ls[1] = 1;

		if (addBigInt(&tempA, &tempB, result)) {
			return 1;
		}
		result->ls[1] = a->ls[1];
		return 0;
	}
	int cmp = compareAbs(a, b);
	if (cmp == 0) {
		result->digits[0] = 0;
		result->ls[0] = 1;
		result->ls[1] = 1;
		return 0;
	}
	if (cmp > 0) {
		substractAbs(a, b, result);
		result->ls[1] = a->ls[1];
	}
	else {
		substractAbs(b, a, result);
		result->ls[1] = 1 - a->ls[1];
	}
	return 0;
}

int multiplyBigInt(BigInt* a, BigInt* b, BigInt* result) { /*Умножение чисел*/
	int len = a->ls[0] + b->ls[0];
	result->ls[1] = a->ls[1] * b->ls[1];
	for (int i = 0;i < len && i < MAX_BLOCKS;i++) {
		result->digits[i] = 0;
	}
	for (int i = 0;i < a->ls[0];i++) {
		unsigned int carry = 0;
		for (int j = 0;j < b->ls[0];j++) {
			unsigned int current =  (unsigned int)a->digits[i] * b->digits[j] + carry;
			if (i + j >= MAX_BLOCKS) {
				if (current != 0) {
					printf("Error: array index out of bounds");
					return 1;
				}
				continue;
			}
			current += result->digits[i + j];
			result->digits[i + j] = current % BASE;
			carry = current / BASE;
		}
		int pos = i + b->ls[0];
		while (carry > 0) {
			if (pos >= MAX_BLOCKS) {
				printf("Error: array index out of bounds");
				return 1;
			}
			unsigned int current = result->digits[pos] + carry;
			result->digits[pos] = current % BASE;
			carry = current / BASE;
			pos++;
		}
	}
	if (len > MAX_BLOCKS) {
		len = MAX_BLOCKS;
	}
	while (len > 1 && result->digits[len - 1] == 0) {
		len--;
	}
	result->ls[0] = len;
	return 0;
}

int powerBigInt(BigInt* a, BigInt* b, BigInt* result) {/*Степень*/
	int power = b->digits[0];
	result->digits[0] = 1;
	result->ls[0] = 1;
	result->ls[1] = 1;

	for (int i = 0;i < power;i++) {
		BigInt temp;
		if (multiplyBigInt(result, a, &temp)) {
			return 1;
		}
		*result = temp;
	}
	return 0;
}

int factorialBigInt(BigInt* a, BigInt* result){/*Факториал*/ 
	int n = a->digits[0];
	result->digits[0] = 1;
	result->ls[0] = 1;
	result->ls[1] = 1;

	for (int i = 2;i <= n;i++) {
		BigInt temp;
		BigInt num;
		num.digits[0] = i;
		num.ls[0] = 1;
		num.ls[1] = 1;
		if (multiplyBigInt(result, &num, &temp)) {
			return 1;
		}
		*result = temp;
	}
	return 0;
}

void divideBy2(BigInt* num) {
	int remainder = 0;

	for (int i = num->ls[0] - 1;i >= 0;i--) {
		int current = remainder * BASE + num->digits[i];
		num->digits[i] = current / 2;
		remainder = current % 2;
	}

	while (num->ls[0] > 1 && num->digits[num->ls[0] - 1] == 0) {
		num->ls[0]--;
	}
}

int sumBigInt(BigInt* a, BigInt* b, BigInt* result) {
	BigInt sum;
	BigInt dif;
	BigInt count;
	BigInt product;
	BigInt one;

	one.digits[0] = 1;
	one.ls[0] = 1;
	one.ls[1] = 1;

	if (addBigInt(a, b, &sum)) {
		return 1;
	}
	if (substractBigInt(b, a, &dif)) {
		return 1;
	}
	if (addBigInt(&dif, &one, &count)) {
		return 1;
	}
	if (multiplyBigInt(&sum, &count, &product)) {
		return 1;
	}
	divideBy2(&product);
	*result = product;
	return 0;
}
int main(void) {
	BigInt a,b,result;
	char op;
	int err = 0;
	scanf_s("%c", &op);
	if (readBigInt(&a) || readBigInt(&b)) {
		return 1;
	}
	switch (op) {
	case '+' :
		err=addBigInt(&a, &b, &result);
		break;
	case '-':
		err=substractBigInt(&a, &b, &result);
		break;
	case '*':
		err=multiplyBigInt(&a, &b, &result);
		break;
	case '^':
		err=powerBigInt(&a, &b, &result);
		break;
	case '!':
		err=factorialBigInt(&a, &result);
		break;
	case 'S':
	case 's':
		err=sumBigInt(&a, &b, &result);
		break;
	}
	if (err) {
		return 1;
	}
	printBigInt(&result);
	return 0;
}
