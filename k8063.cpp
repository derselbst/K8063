#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <cstring>
#include <fcntl.h>
#include <cstdio>
#include <thread>
#include <cstdint>
#include <vector>
#include <chrono>

#include "com.h"

using namespace std::chrono_literals;


enum Seg : uint8_t
{
    // top horizontal segment
    Top_=32,
    // middle horizontal segment
    Middle_=2,
    // bottom horizontal segment
    Bottom_=128,
    // top left vertical segment
    TLI=1,
    // bottom left vertical segment
    BLI=64,
    // top right vertical segment
    TRI=4,
    // top right vertical segment
    BRI=8,
    LI= TLI | BLI,
    RI= TRI | BRI,
    // dot segment
    Dot=16
};

template<size_t SegCount>
class K8063
{
    int fd;
    
public:
    K8063(const char* port)
    {
        this->fd = open (port, O_RDWR | O_NOCTTY | O_SYNC);
        if (fd < 0)
        {
            fprintf(stderr, "error %d opening %s: %s", errno, port, strerror (errno));
            throw std::runtime_error("");
        }

        // set speed to 2400 bps, 8n1 (no parity)
        if (set_interface_attribs (fd, B2400, 0) < 0)
        {
            throw std::runtime_error("set_interface_attribs");
        }

        // set no blocking
        set_blocking (fd, 0);
    }
    
    ~K8063()
    {
        close(this->fd);
    }
    
    void sendRawCommand(uint8_t addr, uint8_t cmd, uint8_t parm, uint8_t repeat=3)
    {
        unsigned char buf[5];
        unsigned int i=0;
        unsigned int sum=0;
        sum += buf[i++] = 13;
        sum += buf[i++] = addr;
        sum += buf[i++] = cmd;
        sum += buf[i++] = parm;
        buf[i++] = ~sum + 1;
        
        for(auto r=0; r<repeat;r++)
        {
            write (fd, buf, i);
        }
    }
    
    void strobe()
    {
        std::this_thread::sleep_for(100ms);
        this->sendRawCommand(0xCC, 'S', 0xCD);
    }
    
    void dim(uint8_t fullyDimmed)
    {
        for(size_t i=0; i<SegCount; i++)
        {
            this->sendRawCommand(i+1, 'I', fullyDimmed, 4);
        }
    }
    
    void draw(const std::vector<uint8_t>& pattern)
    {
        auto cnt = std::min(SegCount, pattern.size());
        for(unsigned int i=0; i < cnt; i++)
        {
            uint8_t addr = cnt - i;
            this->sendRawCommand(addr, 'B', pattern[i]);
        }
    }
    
    void print(const char* buf)
    {
        auto cnt = 6u;
        for(unsigned int i=0; i < cnt; i++)
        {
            uint8_t addr = cnt - i;
            this->sendRawCommand(addr, 'A', buf[i]);
        }
    }
};

int main()
{
    K8063<6> k8063("/dev/ttyS0");
    
    std::vector<uint8_t> hallo;
    hallo.push_back(Seg::Middle_ | Seg::LI | Seg::RI);
    hallo.push_back(Seg::Top_ | Seg::Middle_ | Seg::LI | Seg::RI);
    hallo.push_back(Seg::LI | Seg::Bottom_);
    hallo.push_back(Seg::LI | Seg::Bottom_);
    hallo.push_back(Seg::Top_ | Seg::Bottom_ | Seg::LI | Seg::RI);
    hallo.push_back(Seg::Dot | Seg::RI);
    
    k8063.draw(hallo);
    k8063.strobe();
    
    std::this_thread::sleep_for(2s);
    k8063.dim(255);
    std::this_thread::sleep_for(2s);
    k8063.dim(0);
    std::this_thread::sleep_for(2s);
    
    time_t rawtime;
    struct tm * timeinfo;
    char buffer [80];

    while(true)
    {
        time (&rawtime);
        timeinfo = localtime (&rawtime);
        strftime (buffer,8,"%H%M%S",timeinfo);
        k8063.print(buffer);
        k8063.strobe();
        std::this_thread::sleep_for(900ms);
    }

    return 0;
}
