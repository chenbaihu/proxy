#include "mydata.h"
#include <string.h> //memcpy
#include <event2/buffer.h>
#include "commontypes.h"

MyData::MyData(size_t cap):cap_(cap){
  if(cap>0){
    this->data_=new uint8_t[cap];
  } else {
    this->data_=nullptr;
  }
  this->rdptr_=this->data_;
  this->wptr_=this->data_;
}

bool MyData::read(uint8_t& out){
  if(!isEOF()){
    out=*rdptr_++;
    return true;
  } else return false;
}

void MyData::ensure(size_t len) {
  const size_t r=remain();
  if(r>=len) return;
  const size_t need=cap_ + (r-len);
  const size_t newcap=MY_ROUNDUP(need,2);
  uint8_t* newdata=new uint8_t[newcap];
  memcpy(newdata,data_,cap_);
  const size_t rdpos=rdptr_ - data_;
  const size_t wpos=wptr_ - data_;
  delete[] this->data_;
  this->data_=newdata;
  this->rdptr_ = data_+rdpos;
  this->wptr_ = data_+wpos;
  this->cap_=newcap;
  
}
MyData::MyData(const MyData& obj):cap_(obj.cap_) {
  if(cap_>0){
    this->data_=new uint8_t[cap_];
    memcpy(this->data_,obj.data_,obj.cap_);                
    this->rdptr_=this->data_+(obj.rdptr_ - obj.data_) ;
    this->wptr_=this->data_+(obj.wptr_ - obj.data_);
  } else{
    this->data_=NULL;
    this->rdptr_=this->data_;
    this->wptr_=this->data_;
  }
}

void MyData::writeBuffer(evbuffer *input){
  
}
