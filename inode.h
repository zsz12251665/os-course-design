#ifndef _INODE_H
#define _INODE_H

#include <ctime>

struct INode
{
	int id; // Should be readonly
	int size;
	time_t createAt;
	int direct_addr[10];
	int indirect_addr;
	INode();
};

INode createINode();
INode selectINode(int id);
bool updateINode(INode n);
void deleteINode(int id);

void inodeInitializer();

#endif // _INODE_H
