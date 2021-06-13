 
#pragma once


#ifdef __cplusplus
extern "C" {
#endif


int set_interface_attribs (int fd, int speed, int parity);
void set_blocking (int fd, int should_block);

#ifdef __cplusplus
}
#endif
