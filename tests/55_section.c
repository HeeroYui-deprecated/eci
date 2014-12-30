//#include <stdio.h>

int main(int argc, char* argv[]) {
	int32_t aaa = 0;
	int32_t bbb = 0;
	if ( aaa == bbb && (aaa & bbb) != 0) {
		for (int32_t iii=0; iii< 562; ++iii) {
			aaa+= 1.0;
		}
	}
	return -1;
}
