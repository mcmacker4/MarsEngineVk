#include "MarsEngine.h"

namespace eng {

	void startEngine() {
		vkh::vulkanInit();
	}

	void stopEngine() {
		vkh::cleanUp();
	}

}


int main(int argc, const char** argv) {

	try {

		eng::startEngine();

		eng::stopEngine();

	}
	catch (const char* msg) {
		std::cerr << msg << std::endl;
		return -1;
	}

	std::cin.get();
	return 0;
}