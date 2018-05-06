#include "StackAllocator.h"
#include "XorList.h"

#include <list>
#include <algorithm>
#include <iostream>

using XorListUtility::XorList;

void f1() {
  std::cout << "std::list with std::allocator" << std::endl;
  std::list<int> a;
  double s = clock();
  for (int i(0); i < 10000000; ++i)
    a.push_back(i);
  std::cout << (clock() - s) / CLOCKS_PER_SEC << std::endl;
}

void f2() {
  std::cout << "std::list with StackAllocator" << std::endl;
  std::list<int, StackAllocator<int>> a;
  double s = clock();
  for (int i(0); i < 10000000; ++i)
    a.push_back(i);
  std::cout << (clock() - s) / CLOCKS_PER_SEC  << std::endl;
}

void f3() {
  std::cout << "XorList with std::allocator" << std::endl;
  XorList<int> a;
  double s = clock();
  for (int i(0); i < 10000000; ++i)
    a.push_back(i);
  std::cout << (clock() - s) / CLOCKS_PER_SEC  << std::endl;
}

void f4() {
  std::cout << "XorList with StackAllocator" << std::endl;
  XorList<int, StackAllocator<int>> a;
  double s = clock();
  for (int i(0); i < 10000000; ++i)
    a.push_back(i);
  std::cout << (clock() - s) / CLOCKS_PER_SEC  << std::endl;
}

#include <vector>

void f5() {
  std::cout << "std::vector with std::allocator" << std::endl;
  std::vector<int> a;
  double s = clock();
  for (int i(0); i < 10000000; ++i)
    a.push_back(i);
  std::cout << clock() - s / CLOCKS_PER_SEC  << std::endl;
}

void f6() {
  std::cout << "std::vector with StackAllocator" << std::endl;
  std::vector<int, StackAllocator<int>> a;
  double s = clock();
  for (int i(0); i < 10000000; ++i)
    a.push_back(i);
  std::cout << clock() - s / CLOCKS_PER_SEC  << std::endl;
}

template <typename list>
void show_list(const list& a) {
  std::cout << a.size() << std::endl;
  for (auto& x : a)
    std::cout << x << ' ';
  std::cout << std::endl;
}

template <class list1, class list2>
bool compare_lists(const list1& a, const list2& b) {
  if (a.size() != b.size())
    return false;
  typename list1::const_iterator it_a(a.begin());
  typename list2::const_iterator it_b(b.begin());
  while (it_a != a.end())
  {
    if (*it_a != *it_b)
      return false;
    ++it_a, ++it_b;
  }
  return true;
}



void f7() {
  using namespace std;
  list<int> a;
  XorList<int> b;
  int q; cin >> q;
  while (q--)
  {
    int t; cin >> t;
    if (t == 1) {
      int k; cin >> k;
      a.push_back(k);
      b.push_back(k);
    } else {
      int k; cin >> k;
      a.erase(find(begin(a), end(a), k));
      b.erase(find(begin(b), end(b), k));
    }
    if (!compare_lists(a, b))
      throw "ERROR!!!";
    show_list(a);
    show_list(b);
  }
}

void f8() {
  using namespace std;
  list<int> a;
  XorList<int> b;
  int q(10000); //cin >> q;
  while (q--)
  {
    int t; //cin >> t;
    if (a.empty())
      t = 1;
    else t = (rand() % 100 > 30 ? 0 : 1);
    if (t == 1) {
      int k; k = rand() % 100;//cin >> k;
      a.push_back(k);
      b.push_back(k);
    } else {
      int k; k = rand() % 100;//cin >> k;
      if (find(begin(a), end(a), k) != end(a)) {
        a.erase(find(begin(a), end(a), k));
        b.erase(find(begin(b), end(b), k));
      }
    }
    if (!compare_lists(a, b))
      throw "ERROR!!!";
    //show_list(a);
    //show_list(b);
  }
  cout << "Lists are OK." << endl;
}

int main()
{
  f1();
  f2();
  f3();
  f4();
  //f7();
  f8();
}