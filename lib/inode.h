#ifndef _INODE_H
#define _INODE_H

#include <ctime>

const int INODE_SIZE = 64; // sizeof INode()
const int INODE_MEMORY_START = 1 << 11;
const int INODE_MEMORY_END = 1 << 20;
const int INODE_MAX_NUMBER = (INODE_MEMORY_END - INODE_MEMORY_START) / INODE_SIZE;

struct INode
{
	int id; // Should be readonly
	int size;
	unsigned long long createAt;
	int direct_addr[10];
	int indirect_addr;
	INode();
};

const INode BLANK_INODE;

INode createINode();
INode selectINode(int id);
bool updateINode(INode n);
bool deleteINode(int id);

void inodeInitializer();

#endif // _INODE_H
