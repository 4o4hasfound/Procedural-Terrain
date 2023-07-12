#ifndef _CHUNK_H_
#define _CHUNK_H_

class Chunk {
public:
	Chunk();
	Chunk(unsigned int size, unsigned int texture);
	~Chunk();
private:
	unsigned int VAO, VBO, EBO;
	
};


#endif