#include <event2/event.h>
#include <gtest/gtest.h>
#include <iostream>
#include <curl/curl.h>
#include <memory>

#include "libevent_thread.h"
#include "md5.h"
#include "base64.h"
#include "MyThread.h"


class CurlTest : public ::testing::Test {
  virtual void SetUp() {}
  virtual void TearDown() {}
};


//Return the number of bytes actually taken care of.
static size_t curl_write_data(void *buffer, size_t size, size_t nmemb, void *userp){
  std::string* body=(std::string*)userp;
  body->append((const char*)buffer,size*nmemb);
  return size*nmemb;
}

static size_t extract_md5_from_header( void *ptr, size_t size, size_t nmemb, void *userdata){
  std::string* md5=(std::string*)userdata;
  size_t line_length=size*nmemb;
  static const std::string prefix="Content-Md5:";
  if(line_length<=prefix.length()) return line_length;
  if(strncasecmp(prefix.c_str(),(const char*)ptr,prefix.length())) return line_length;
  //found:
  md5->clear();
  char* p=(char*)ptr;
  char* end=p+line_length;
  p+=prefix.length();
  //skip spaces
  while(p!=end && isspace(*p)) p++;
  for(;p!=end && !isspace(*p);++p) md5->append(1,*p);
  return line_length;
}

//proxy string:  socks4://, socks4a://, socks5:// or socks5h://
static void fetchUrl(CURL* curl,const std::string& url,const std::string& proxy){
  bool needDeleteCurl=false;
  if(!curl) {
    curl=curl_easy_init();
    needDeleteCurl=true;
  }
  if(proxy.empty())
    std::cout<<"test fetch without proxy"<<std::endl;
  else
    std::cout<<"test fetch with proxy "<<proxy<<"."<<std::endl;
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl,CURLOPT_VERBOSE,1L);
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  std::string body;
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_data);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
  curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, extract_md5_from_header);
  if(!proxy.empty())
    curl_easy_setopt(curl,CURLOPT_PROXY,proxy.c_str());
  std::string md5base64;
  curl_easy_setopt(curl, CURLOPT_WRITEHEADER, &md5base64);
  CURLcode res = curl_easy_perform(curl);
  ASSERT_EQ(res,CURLE_OK);
  unsigned char md5_output[16];
  unsigned char *md5input=new unsigned char[body.length()];
  memcpy(md5input,body.c_str(),body.length());
  md5(md5input,body.length(),md5_output);
  delete[] md5input;
  
  uint8_t md5fromserver[64];
  int len=base64_pton(md5base64.c_str(),md5fromserver,sizeof(md5fromserver));
  ASSERT_EQ(len,16);
  int ret=memcmp(md5fromserver,md5_output,16);
  ASSERT_EQ(ret,0);
  if(needDeleteCurl) curl_easy_cleanup(curl);
}


static void run_server(){
  std::shared_ptr<IRunnable> r=std::make_shared<LibeventThread>();
  MyThread* thr=new MyThread(r);
  thr->run();
  //unsigned ret=thr.wait();
}

TEST_F(CurlTest, test1) { 
  run_server();
  //EXPECT_EQ(1, 1);
  //use curl to fetch a page from http://jigsaw.w3.org/HTTP/h-content-md5.html
  //then check md5 of this page
  CURL *curl=curl_easy_init();
  ASSERT_NE(curl,nullptr);
  fetchUrl(curl,"http://jigsaw.w3.org/HTTP/h-content-md5.html","");
  //  fetchUrl(curl,"http://jigsaw.w3.org/HTTP/h-content-md5.html","socks4://127.0.0.1:1080");
  //fetchUrl(curl,"http://jigsaw.w3.org/HTTP/h-content-md5.html","socks4a://127.0.0.1:1080");
  //fetchUrl(curl,"http://jigsaw.w3.org/HTTP/h-content-md5.html","socks5://127.0.0.1:1080");
  fetchUrl(curl,"http://jigsaw.w3.org/HTTP/h-content-md5.html","socks5h://127.0.0.1:1080");
  curl_easy_cleanup(curl);

  printf("test with NULL curl handler\n");
  fetchUrl(NULL,"http://jigsaw.w3.org/HTTP/h-content-md5.html","");
  //fetchUrl(NULL,"http://jigsaw.w3.org/HTTP/h-content-md5.html","socks4://127.0.0.1:1080");
  //fetchUrl(NULL,"http://jigsaw.w3.org/HTTP/h-content-md5.html","socks4a://127.0.0.1:1080");
  //fetchUrl(NULL,"http://jigsaw.w3.org/HTTP/h-content-md5.html","socks5://127.0.0.1:1080");
  fetchUrl(NULL,"http://jigsaw.w3.org/HTTP/h-content-md5.html","socks5h://127.0.0.1:1080");
 }

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
