#include <boost/interprocess/managed_shared_memory.hpp>
#include <cstdlib> //std::system
#include <cstddef>
#include <cassert>
#include <utility>

int main(int argc, char *argv[])
{
	using namespace boost::interprocess;

	//Open already created shared memory object.
	shared_memory_object shm(open_only, "MySharedMemory", read_only);
	printf("got Here\n");
	//Map the whole shared memory in this process
	mapped_region region(shm, read_only);
	printf("got Here1\n");
	//Check that memory was initialized to 1
	char *mem = static_cast<char*>(region.get_address());
	printf("got Here2\n");
	for (std::size_t i = 0; i < region.get_size(); ++i)
		if (*mem++ != 1)
			return 1;   //Error checking memory

	return 0;
}