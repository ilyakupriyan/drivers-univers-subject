#include <iostream>
#include <fstream>

#define SIZE_BUF 80

int main ()
{
    std::ifstream fd;
    char buf[SIZE_BUF];
    fd.open("/dev/dev_md");
    fd.read(buf, SIZE_BUF);
    //fd = open("dev/dev_md", O_RDWR);
    std::cout << buf << std::endl;
    //read(fd, buf, SIZE_BUF);

    fd.close();
    //close(fd);
    return 0;
}