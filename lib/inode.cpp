#include "inode.h"
#include "storage.h"
#include "directory.h"
#include <bitset>
#include <cstdlib>
#include <cstring>

INode root_inode;
const INode &ROOT_INODE = root_inode;

std::bitset<INODE_MAX_NUMBER + 1> used_inode;

INode::INode() {
	memset(this, 0, sizeof (*this));
}

inline bool isValidINodeNumber(int num) {
	return 1 <= num <= INODE_MAX_NUMBER;
}

int getFreeINodeNumber() {
	if (used_inode.all())
		return 0;
	for (int i = 1; i <= INODE_MAX_NUMBER; ++i)
		if (!used_inode[i])
			return i;
	return 0;
}

INode createINode() {
	INode res;
	res.num = getFreeINodeNumber();
	if (!isValidINodeNumber(res.num))
		return BLANK_INODE;
	res.create_time = time(NULL);
	if (putBlock(INODE_MEMORY_START + (res.num - 1) * INODE_SIZE, &res, INODE_SIZE))
		return BLANK_INODE;
	used_inode.set(res.num);
	return res;
}

INode selectINode(int num) {
	if (!isValidINodeNumber(num) || !used_inode[num])
		return BLANK_INODE;
	INode res;
	if (getBlock(INODE_MEMORY_START + (num - 1) * INODE_SIZE, &res, INODE_SIZE))
		return BLANK_INODE;
	return res;
}

bool updateINode(const INode &inode) {
	if (!isValidINodeNumber(inode.num) || !used_inode[inode.num])
		return 1;
	return putBlock(INODE_MEMORY_START + (inode.num - 1) * INODE_SIZE, &inode, INODE_SIZE);
}

bool deleteINode(int num) {
	if (!isValidINodeNumber(num) || putBlock(INODE_MEMORY_START + (num - 1) * INODE_SIZE, &BLANK_INODE, INODE_SIZE))
		return 1;
	used_inode.reset(num);
	return 0;
}

void inodeInitializer() {
	used_inode.reset();
	INode tmp;
	for (int i = INODE_MEMORY_START; i < INODE_MEMORY_END; i += INODE_SIZE) {
		if (getBlock(i, &tmp, INODE_SIZE))
			continue;
		if (tmp.num != 0)
			used_inode.set(tmp.num);
	}
	if (used_inode.none()) {
		// root_inode = createDir(0); // Create directory for root
		root_inode = createINode();
		if (tmp.num == 0) {
			printf("Error: Fail to create the INode for '/'!\n");
			exit(-1);
		}
	} else {
		root_inode = selectINode(1);
		if (root_inode.num == 0) {
			printf("Error: Fail to select the INode for '/'!\n");
			exit(-1);
		}
	}
}
