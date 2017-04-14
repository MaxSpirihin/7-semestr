#include <stdio.h>
#include <dlfcn.h>

int main (int argc, char *argv[])
{
  const char* fName;
  if (argc < 2)
    fName = "hello";
  else
    fName = argv[1];


  void *handle = dlopen("./Library.so",RTLD_NOW);
  if (handle==0) 
  {
     printf("Library not found\n");
	return 1;
  }
  
  void (*fn)() = dlsym(handle,fName);
  if (fn == 0)
  {
    printf("There is no function \"%s\" in library\n",fName);
    dlclose(handle);
    return 1;
  }

  (*fn)();
  dlclose(handle);
  return 0;
}
