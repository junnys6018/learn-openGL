#include "IndexBuffer.h"
#include "debug.h"

IndexBuffer::IndexBuffer()
	:ID(0), count(0) {	}
IndexBuffer::IndexBuffer(unsigned int *data, unsigned int count)
	:count(count)
{
	GLCall(glGenBuffers(1, &ID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
}

IndexBuffer::IndexBuffer(IndexBuffer&& other)
	: ID(other.ID), count(other.count)
{
	ID = 0;
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other)
{
	unsigned int id = ID;
	ID = other.ID;
	count = other.count;
	other.ID = id;
	return *this;
}

IndexBuffer::~IndexBuffer()
{
	GLCall(glDeleteBuffers(1, &ID));
}
void IndexBuffer::Bind()
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID));
}
void IndexBuffer::UnBind()
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
inline unsigned int IndexBuffer::getID()
{
	return ID;
}
unsigned int IndexBuffer::getCount()
{
	return count;
}

