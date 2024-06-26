#include <iostream>
#include "Util/logger.h"
using namespace std;
using namespace toolkit;


int start_main(int argc, char *argv[])
{
    // 设置日志
    Logger::Instance().add(std::make_shared<ConsoleChannel>("ConsoleChannel", LInfo));
}

#ifndef DISABLE_MAIN
int main(int argc, char *argv[])
{
    return start_main(argc, argv);
}
#endif // DISABLE_MAIN
