#include <unistd.h>

#include "memory_region.h"

int main(int argc, char **argv) {
    struct memory_region *init = mr_new("initializer",0x4);
    struct memory_region *foo = mr_new("foo",0x20);
    struct memory_region *bar = mr_new("bar",0x28);
    int i;
    for(i=0;i<0x20;i+=2) {
	mr_mark_name(foo,i,"bar");
	mr_mark_name(foo,i+1,"foo");
    }
    for(i=0;i<0x28;i+=3) {
	mr_mark_name(bar,i,"foo");
	bar->data[i+1] = 0;
	bar->data[i+2] = 0;
    }
    mr_mark_ptr(init,0,foo);
    mr_mark_ptr(init,1,bar);
    mr_mark_name(init,2,"foo");
    mr_mark_name(init,3,"bar");
    void *to_output;
    size_t size;
    relocate_regions(&to_output,&size,init);
    write(1,to_output,size);
}
