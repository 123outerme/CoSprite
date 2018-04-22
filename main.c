#include "pistonMain.h"

int main(int argc, char* argv[])
{
    int code = initPiston("cb.bmp", "Piston Test", 960, 480, "", 24);
    waitForKey();
    return code;
}
