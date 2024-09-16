#include "swap.h"

void Swap(char *left, char *right)
{

	char temp = *left; // Вводим временную переенную temp
	*left = *right; // Ставим левый элемент на место правого
	*right = temp; // ставим правый элемент на место временной переменной
}
