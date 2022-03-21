#ifndef _INODE_H
#define _INODE_H

const int INODE_SIZE = 64; // sizeof INode()
const int INODE_MEMORY_START = 1 << 11;
const int INODE_MEMORY_END = 1 << 20;
const int INODE_MAX_NUMBER = (INODE_MEMORY_END - INODE_MEMORY_START) / INODE_SIZE;

const int DIR_INODE_TYPE = 0;
const int FILE_INODE_TYPE = 1;

struct INodeError {
	int inode_num;
	const char* message;
	const char* when;
};

struct INode
{
	int num; // Should be readonly
	int size;  // in Bytes
	unsigned long long create_time;
	int direct_addr[10];
	int indirect_addr;
	int type;
	INode();
};

typedef int AddrBlock[256];

const INode BLANK_INODE;
extern const INode &ROOT_INODE;

INode createINode();
INode selectINode(int num);
void updateINode(const INode &inode);
void deleteINode(int num);

void inodeInitializer();

#endif // _INODE_H
