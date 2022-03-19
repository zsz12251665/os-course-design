#include "inode.h"
#include "storage.h"
#include <bitset>
#include <cstdlib>
#include <cstring>

std::bitset<INODE_MAX_NUMBER + 1> bs_inode;

INode::INode() {
	id = 0;
	size = 0;
	createAt = 0;
	memset(direct_addr, 0, sizeof direct_addr);
	indirect_addr = 0;
}

inline bool isValidINodeId(int id) {
	return 1 <= id <= INODE_MAX_NUMBER;
}

int getFreeId() {
	if (bs_inode.all())
		return -1;
	for (int i = 1; i <= INODE_MAX_NUMBER; ++i)
		if (!bs_inode[i])
			return i;
	return -1;
}

INode createINode() {
	INode res;
	res.id = getFreeId();
	if (!isValidINodeId(res.id))
		return BLANK_INODE;
	res.createAt = time(NULL);
	if (putBlock(INODE_MEMORY_START + (res.id - 1) * INODE_SIZE, &res, INODE_SIZE))
		return BLANK_INODE;
	bs_inode.set(res.id);
	return res;
}

INode selectINode(int id) {
	if (!isValidINodeId(id) || !bs_inode[id])
		return BLANK_INODE;
	INode res;
	if (getBlock(INODE_MEMORY_START + (id - 1) * INODE_SIZE, &res, INODE_SIZE))
		return BLANK_INODE;
	return res;
}

bool updateINode(INode n) {
	if (!isValidINodeId(n.id) || !bs_inode[n.id])
		return 1;
	return putBlock(INODE_MEMORY_START + (n.id - 1) * INODE_SIZE, &n, INODE_SIZE);
}

bool deleteINode(int id) {
	if (!isValidINodeId(id) || putBlock(INODE_MEMORY_START + (id - 1) * INODE_SIZE, &BLANK_INODE, INODE_SIZE))
		return 1;
	bs_inode.reset(id);
	return 0;
}

void inodeInitializer() {
	bs_inode.reset();
	INode tmp;
	for (int i = INODE_MEMORY_START; i < INODE_MEMORY_END; i += INODE_SIZE) {
		if (getBlock(i, &tmp, INODE_SIZE))
			continue;
		if (tmp.id != 0)
			bs_inode.set(tmp.id);
	}
	if (bs_inode.none()) {
		if(createINode().id == 0) { // Create INode for `/`
			printf("Error: Fail to create INode for '/'!\n");
			exit(-1);
		}
	}
}
