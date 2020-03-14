#ifndef _SOURCE_H__
#define _SOURCE_H__

#include <string>
#include <cstring>

class Source
{
public:
  enum Type
  {
    DEVICE,
    MONITOR,
    APP,
    MIC,
  };

public:
	Source(Type type, int id, const std::string& name)
	{
		this->type = type;
		this->id = id;
		this->name = name;
		memset(this->unique_id, '\0', 1024);
	}
	
  Source(Type type, int id, const std::string& name, char unique_id[1024])
  {
    this->type = type;
    this->id = id;
    this->name = name;
	strcpy(this->unique_id, unique_id);
  }
	
  Type type;
  //Position in device list
  uint32_t id;
  std::string name;
  //unique device id
  char unique_id[1024];
};
#endif
