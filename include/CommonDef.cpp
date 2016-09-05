#include "CommonDef.h"

bool TestError(int error, std::string where, bool fatal) {
  if (error) {
    if (fatal)
        std::cerr << "***FATAL ";
    std::cerr << "ERROR, code " << error;
    
    if (where != "")
        std::cerr << " @ "<< where << std::endl;
    else std::cerr << std::endl;
    
    if (fatal)
        exit(-1);
    
    return 0;
  }
  
  else return 1;
}

string show_hex(unsigned int number, int size){
  ostringstream to_return;
  to_return<<"0x";
  int i=size-1;
  if(size==-1){
    while(number>=pow(16.0,i))i++;
    i--;
  }
  for (int k=i; k>-1; k--){
    for (int j=1; j<18; j++){
      if (number<j*pow(16.0,k)){
	  switch (j){
	    case 11:
	      to_return<<"A";
	      break;
	    case 12:
	      to_return<<"B";
	      break;
	    case 13:
	      to_return<<"C";
	      break;
	    case 14:
	      to_return<<"D";
	      break;
	    case 15:
	      to_return<<"E";
	      break;
	    case 16:
	      to_return<<"F";
	      break;
	    default:
	      to_return<<j-1;
	  }
	  number=number-(j-1)*pow(16.0,k);
	  break;
      }
    }
  }
  return(to_return.str());
}



unsigned int digit(unsigned int data, int begin, int end)
{
    if (begin<end){return -1;}
    data=data%(int)pow(2.0,begin+1);
    data=data-data%(int)pow(2.0,end);
    return (data/pow(2.0,end));
}

unsigned int digit(unsigned int data, int position)
{
    return digit(data, position, position);
}
