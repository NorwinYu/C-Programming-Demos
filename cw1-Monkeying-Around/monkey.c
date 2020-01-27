#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main (int argc, char *argv[])
{
	int add_or_sub_number; // define the add_or_sub_number to print
	int num_numbers = 3;	// the number of numbers
	int number[num_numbers];	// define an array for loop to save three numbers
	
	for (int i = 0; i < num_numbers; i++)
	{
		printf("Enter number %d: ", i + 1);	// C use 0 to start
		scanf("%d", &number[i]); 
		
		if (number[i] <= 0)	// judge number[i] is positive or not 
		{
			printf("Invalid number!\n");
			exit(1);
		}
	}

	add_or_sub_number = number[1] - pow(-1, number[0]) * number[2];	// count the add_or_sub_number
	if (add_or_sub_number > 10)
	{
		printf("Paul is the Monkey King, he can lift %d jin!\n", add_or_sub_number);
	}
	
	exit(0);
}