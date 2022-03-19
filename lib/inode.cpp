#include "inode.h"
#include "storage.h"
#include <bitset>
#include <cstring>

const int INODE_SIZE = 64; // sizeof INode()
const int INODE_MEMORY_START = 1 << 11;
const int INODE_MEMORY_END = 1 << 20;
const int INODE_MAX_NUMBER = (INODE_MEMORY_END - INODE_MEMORY_START) / INODE_SIZE;
const INode BLANK_NODE;

std::bitset<INODE_MAX_NUMBER + 1> bs_inode;

INode::INode() {
	id = 0;
	size = 0;
	createAt = 0;
	memset(direct_addr, 0, sizeof direct_addr);
	indirect_addr = 0;
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
	res.createAt = time(NULL);
	bs_inode.set(res.id);
	putBlock(INODE_MEMORY_START + (res.id - 1) * INODE_SIZE, &res, INODE_SIZE);
	return res;
}

INode selectINode(int id) {
	INode res;
	if (bs_inode[id])
		getBlock(INODE_MEMORY_START + (id - 1) * INODE_SIZE, &res, INODE_SIZE);
	return res;
}

bool updateINode(INode n) {
	if (!bs_inode[n.id])
		return false;
	putBlock(INODE_MEMORY_START + (n.id - 1) * INODE_SIZE, &n, INODE_SIZE);
	return true;
}

void deleteINode(int id) {
	putBlock(INODE_MEMORY_START + (id - 1) * INODE_SIZE, &BLANK_NODE, INODE_SIZE);
	bs_inode.reset(id);
}

void inodeInitializer() {
	bs_inode.reset();
	INode tmp;
	for (int i = INODE_MEMORY_START; i < INODE_MEMORY_END; i += INODE_SIZE) {
		getBlock(i, &tmp, INODE_SIZE);
		if (tmp.id != 0)
			bs_inode.set(tmp.id);
	}
	if (bs_inode.none()) {
		INode res = createINode();
		; // Create dir file for `/`
	}
}
