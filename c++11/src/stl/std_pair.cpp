#include "stl.h"
#include <utility> //std::pair, std::make_pair

template <class T>
T append(T a, T b) {
	return a + b;
}

int pair_main()
{
	std::cout << "---------------std pair\n";
	std::pair<std::string, double> product1;
	std::pair<std::string, double> product2("tomatoes", 2.30);
	std::pair<std::string, double> product3(product2);

	//һ��make_pair ��ʹ������Ҫpair��������λ�ã�����ֱ�ӵ���make_pair����pair������һ����make_pair���Խ�����ʽ������ת��
	//template pair make_pair(T1 a, T2 b) { return pair(a, b); }

	//std::pair<int, float>(1, 1.1) 
	//std::make_pair(1, 1.1)  Ĭ��second��double
	product1 = std::make_pair(std::string("lightbulbs"), 0.99);

	product2.first = "shoes";
	product2.second = 39.90;

	std::cout << "the price of " << product1.first << " is $" << product1.second << "\n";
	std::cout << "the price of " << product2.first << " is $" << product2.second << "\n";
	std::cout << "the price of " << product3.first << " is $" << product3.second << "\n";

	return 0;
}