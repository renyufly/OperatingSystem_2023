#include <stdio.h>
int main() {
	int n;
	scanf("%d", &n);
	int ret = n;
	int sum = 0;
	while(n > 0) {
		sum = sum*10 + n % 10;
		n = n / 10;
	}
	if (ret == sum) {
		printf("Y\n");
	} else {
		printf("N\n");
	}
	return 0;
}
