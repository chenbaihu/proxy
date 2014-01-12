
#include <gtest/gtest.h>

#include <event2/buffer.h>

class EvbufferTest : public ::testing::Test {
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(EvbufferTest, test1) { 
  evbuffer* buffer=evbuffer_new();
  ASSERT_NE(buffer,nullptr);
  const std::string& str="hello";
  //write a string into buffer
  ASSERT_EQ(0,evbuffer_add(buffer,str.c_str(),str.size()));
  
  //copy one char
  char c;
  int bytesCopied=evbuffer_remove(buffer,&c,1);
  ASSERT_EQ(1,bytesCopied);
  ASSERT_EQ(c,'h');

  //free the buffer
  evbuffer_free(buffer);
}


TEST_F(EvbufferTest, test2) { 
  evbuffer* buffer=evbuffer_new();
  ASSERT_NE(buffer,nullptr);
  const std::string& str="hello";
  //write a string into buffer
  ASSERT_EQ(0,evbuffer_add(buffer,str.c_str(),str.size()));
  
  //copy it out
  char buf[1024];
  int bytesCopied=evbuffer_remove(buffer,buf,sizeof(buf));
  ASSERT_EQ(str.size(),bytesCopied);

  //free the buffer
  evbuffer_free(buffer);
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
