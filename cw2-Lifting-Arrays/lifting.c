#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	const int Number_disks = 8;
	double disks[Number_disks] = {50.00, 20.00, 10.00, 5.00, 2.00, 1.00, 0.50, 0.25};
	int disks_hundred[Number_disks];
	for (int i = 0; i < Number_disks; i++)
	{
		disks_hundred[i] = disks[i] * 100;
	}

	int Unit_of_Weight;
	char Name_Unit[2][4] = {"lbs", "kg"};
	printf("Enter the Unit of Weight ([1] - lbs, [2] - kg): ");
	scanf("%d", &Unit_of_Weight);


	if ((Unit_of_Weight != 1)&&(Unit_of_Weight != 2))
	{
		printf("Invalid Number!\n");
		exit(3);
	}
	else
	{
		double Target_Weight;
		printf("Enter the Target Weight in %s: ", Name_Unit[Unit_of_Weight - 1]);
		scanf("%lg", &Target_Weight);
		int Target_Weight_hundred = Target_Weight * 100;
		
		if (Target_Weight > 10000)
		{
			printf("Too Heavy!\n");
			exit(2);
		}
		else if (Target_Weight < 0)
		{
			printf("Invalid number!\n");
			exit(1);
		}
		else if ((Target_Weight_hundred % disks_hundred[Number_disks - 1]) !=  0 || Target_Weight_hundred != (Target_Weight * 100))
		{
			printf("Impossible!\n");
			exit(0);
		}
		else
		{
			for (int i = 0; i < Number_disks; i++)
			{
				int division_result = Target_Weight_hundred / disks_hundred[i];
				if (division_result >= 1)
				{
					Target_Weight_hundred = Target_Weight_hundred - disks_hundred[i] * division_result;
				}
				printf("%d", division_result);

				if (i != (Number_disks -1))
				{
					printf(",");
				}
				else
				{
					printf("\n");
				}
			}
			exit(0);
		}
	}
}