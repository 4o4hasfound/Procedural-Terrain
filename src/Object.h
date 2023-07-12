#pragma once
#ifndef _OBJECT_H_
#define _OBJECT_H_

class Object {
public:
	virtual void Draw() = 0;
	virtual void Update() {};
	
private:

};

#endif