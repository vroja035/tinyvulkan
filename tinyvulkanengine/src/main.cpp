/*
	Entry point for the application.
*/

#include <tv_engine.h>

int main(int argc, char* argv[])
{
	TinyVulkan engine;

	engine.init();	
	
	engine.run();	

	engine.cleanup();	

	return 0;
}
