catch.o: catch.cpp catch.hpp
	$(CXX) $< -o $@ -c -Wall -Wextra $(CXXFLAGS) -O0

test.o: test.cpp ../forward_list2.hpp
	$(CXX) $< -o $@ -c -Wall -Wextra $(CXXFLAGS) -O0

test: test.o catch.o
	$(CXX) $^ -o $@ $(LDFLAGS)
  
clean:
	rm test *.o
