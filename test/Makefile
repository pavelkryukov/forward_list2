test.o: test.cpp ../forward_list2.hpp
	$(CXX) $< -o $@ -c -Wall -Wextra $(CXXFLAGS) -O0

test: test.o 
	$(CXX) $^ -o $@ $(LDFLAGS) -lgtest -lgtest_main -lpthread
  
clean:
	rm test *.o
